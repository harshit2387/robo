import cv2
import serial
import time

# --- Configure serial port ---
# Make sure COM5 is correct (check Arduino IDE → Tools → Port)
ser = serial.Serial('COM5', 9600, timeout=1)
time.sleep(2)  # wait for Arduino reset

# --- Load Haar cascade for face detection ---
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalface_default.xml")

# --- Load pre-trained emotion detector (OpenCV has smile cascade) ---
smile_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_smile.xml")

cap = cv2.VideoCapture(0)

eyes_open = False  # track if eyes are open

while True:
    ret, frame = cap.read()
    if not ret:
        break

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, 1.3, 5)

    if len(faces) > 0:
        # If face detected → open eyes
        if not eyes_open:
            ser.write(b"OPEN\n")
            eyes_open = True

        for (x, y, w, h) in faces:
            roi_gray = gray[y:y + h, x:x + w]
            roi_color = frame[y:y + h, x:x + w]

            # Detect smile (happy face)
            smiles = smile_cascade.detectMultiScale(roi_gray, 1.8, 20)

            if len(smiles) > 0:
                cv2.putText(frame, "Happy", (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)
                ser.write(b"HAPPY\n")
            else:
                cv2.putText(frame, "Sad", (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2)
                ser.write(b"SAD\n")

            cv2.rectangle(frame, (x, y), (x + w, y + h), (255, 0, 0), 2)

    else:
        # If no face → close eyes
        if eyes_open:
            ser.write(b"CLOSE\n")
            eyes_open = False

    cv2.imshow("Face + Mood Detector", frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
ser.close()

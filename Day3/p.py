import cv2
import serial
import time

# -------------------------
# Serial setup (Arduino Nano)
# -------------------------
arduino_port = "COM7"  # Change if needed
arduino = serial.Serial(arduino_port, 9600, timeout=1)
time.sleep(2)  # wait for Arduino to reset

# -------------------------
# ESP32-CAM stream setup
# -------------------------
esp_url = "http://192.168.31.168:80/stream"  # Your working stream
cap = cv2.VideoCapture(esp_url)

if not cap.isOpened():
    print("❌ Could not open ESP32-CAM stream")
    exit()

# -------------------------
# Face detection setup
# -------------------------
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + "haarcascade_frontalface_default.xml")

# Get frame center
ret, frame = cap.read()
if not ret:
    print("❌ Failed to grab initial frame")
    exit()

frame_height, frame_width = frame.shape[:2]
center_x = frame_width // 2
center_y = frame_height // 2

# -------------------------
# Main loop
# -------------------------
while True:
    ret, frame = cap.read()
    if not ret:
        continue

    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, 1.3, 5)

    command = "default\n"  # Default if no face detected

    for (x, y, w, h) in faces:
        # Draw rectangle for debug
        cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

        face_center_x = x + w // 2
        face_center_y = y + h // 2

        # Calculate offset from frame center
        dx = face_center_x - center_x
        dy = face_center_y - center_y

        # Decide mood based on face position
        if abs(dx) > 60:  # Horizontal movement
            command = "happy\n" if dx > 0 else "angry\n"
        elif abs(dy) > 60:  # Vertical movement
            command = "surprised\n" if dy < 0 else "sad\n"
        else:
            command = "default\n"  # Center

        break  # Only track first face

    # Send command to Arduino
    arduino.write(command.encode())
    print("Sent:", command.strip())

    # Show frame for debugging
    cv2.imshow("ESP32-CAM Face Tracker", frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Cleanup
cap.release()
cv2.destroyAllWindows()
arduino.close()

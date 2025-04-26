from flask import Flask, request
import threading
import time
import cv2
import numpy as np
import io
from PIL import Image
import os
from emotion_detector import detect_emotion_and_annotate

# Setup
app = Flask(__name__)
last_frame = None
frame_lock = threading.Lock()

SAVE_DIR = "saved_frames"
os.makedirs(SAVE_DIR, exist_ok=True)

# Load OpenCV's built-in face detector
face_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_frontalface_default.xml')

def process_frame(image_data):
    """Convert raw image bytes to OpenCV frame, flip, and detect face."""
    try:
        image = Image.open(io.BytesIO(image_data))
        image = image.transpose(Image.FLIP_TOP_BOTTOM)  # Flip vertically
        frame = cv2.cvtColor(np.array(image), cv2.COLOR_RGB2BGR)
        frame = detect_and_draw_faces(frame)
        return frame
    except Exception as e:
        print(f"[ERROR] Failed to process frame: {e}")
        return None

def detect_and_draw_faces(frame):
    """Detect faces and draw bounding boxes for emotion detection."""
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    faces = face_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30))

    for (x, y, w, h) in faces:
        # Emotion detection will be added below; remove green box for regular face detection
        pass

    return frame


@app.route('/upload', methods=['POST'])
def upload():
    global last_frame
    try:
        frame = process_frame(request.get_data())
        if frame is not None:
            with frame_lock:
                last_frame = frame.copy()
        return "OK", 200
    except Exception as e:
        print(f"[ERROR] {e}")
        return "Error", 500

def video_display():
    global last_frame
    while True:
        with frame_lock:
            frame = last_frame.copy() if last_frame is not None else None
        
        if frame is not None:
            # Add emotion detection
            annotated_frame = detect_emotion_and_annotate(frame)

            cv2.imshow("ESP32-CAM Stream", annotated_frame)
            # Remove or comment out the line below
            # cv2.displayOverlay("ESP32-CAM Stream", "Press 'q' to quit", 1000)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
        else:
            time.sleep(0.05)

    cv2.destroyAllWindows()


def start_server():
    app.run(host='0.0.0.0', port=8000)

if __name__ == '__main__':
    threading.Thread(target=video_display, daemon=True).start()
    start_server()

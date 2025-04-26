import cv2
from fer import FER

# Initialize the detector globally
emotion_detector = FER()

def detect_emotion_and_annotate(frame):
    results = emotion_detector.detect_emotions(frame)
    for result in results:
        box = result['box']
        emotions = result['emotions']
        if emotions:
            top_emotion = max(emotions, key=emotions.get)
            x, y, w, h = box
            cv2.rectangle(frame, (x, y), (x + w, y + h), (255, 0, 0), 2)
            cv2.putText(frame, top_emotion, (x, y - 10),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.9, (255, 0, 0), 2)
    return frame

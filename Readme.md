# ESP32-CAM Live Emotion Detection System

This project streams live video from an **ESP32-CAM** module to a **Flask** server running on a PC.  
The server processes incoming frames, applies **emotion detection** using the `FER` library, and displays the annotated video feed in real-time.

---

## 🛠 Project Structure

- **ESP32-CAM firmware** (Arduino C++)
  - Captures frames and sends them via HTTP POST to the server.
- **Flask server** (Python)
  - Receives image frames, detects emotions using `FER`, and displays live video.

---

## ⚡ Features

- Live camera streaming from ESP32-CAM.
- Face detection using OpenCV Haar cascades.
- Emotion detection using the `FER` deep learning library.
- Real-time display with bounding boxes and emotion labels.

---

## 🧩 Requirements

### Hardware
- ESP32-CAM board (e.g., AI-Thinker ESP32-CAM)
- USB-to-Serial adapter (for programming ESP32-CAM)

### Software
- Arduino IDE with ESP32 board support
- Python 3.7+
- Install required Python packages:
  ```bash
  pip install flask opencv-python-headless fer Pillow

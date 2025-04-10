# Object Detection and Tracking in C++

# 🧠 Multiclass Object Detection and Tracking using OpenCV

This project implements **real-time object detection and tracking** using the MobileNet SSD model and OpenCV's CSRT tracker. It focuses on detecting and tracking **"person"** and **"car"** classes in a video.

---

## 📂 File Structure

```plaintext
├── object_tracking.cpp                         # Source code
├── MobileNetSSD_deploy.prototxt    # Network architecture
├── MobileNetSSD_deploy.caffemodel  # Pre-trained weights
├── cars.mp4                         # Input video
├── output1.avi                      # Output video with tracked boxes
```

---

## ⚙️ Features

- 🔍 **Object Detection** using [MobileNet SSD](https://github.com/chuanqi305/MobileNet-SSD)
- 🎯 **Object Tracking** with **CSRT** tracker
- 🔁 **Periodic Redetection** to avoid tracker drift
- 🎞️ Saves annotated video with tracked objects
- 👤 Tracks only selected classes: `"person"` and `"car"` etc.,

---

## 🛠️ Requirements

- OpenCV 4.x (including `opencv_dnn` and `opencv_tracking`)
- C++17 or higher
- A sample video (`cars.mp4`)

---

## 🚀 How It Works

### 🔍 Detection
- Performed every `30` frames or when trackers are empty
- Converts the frame to a blob
- Forwards through MobileNet SSD
- Filters detections by:
  - Confidence (`> 0.5`)
  - Class name (`person`, `car`)

### 🎯 Tracking
- Each valid detection gets a **CSRT** tracker
- Every frame:
  - Trackers are updated
  - Bounding boxes are drawn
  - Objects that go off-frame are marked as "Lost"

### 🧠 Redetection
- Redetection happens every `30` frames to refresh trackers

---

## 📈 Output Format

- 🟩 **Green box**: New detections
- 🟦 **Blue box**: Tracked object
- ❌ **"Lost"**: Tracker failed

---

## 📽️ Sample Output

A new video file `output1.avi` will be generated with all detections and tracking results overlaid.

---

## 🔧 Enhancements (Optional Ideas)

- ✅ Assign unique tracking IDs
- ⚡ Use advanced trackers (Deep SORT, ByteTrack)
- 🎨 Add FPS display, better UI
- 📦 Extend to more object classes

---

## 📚 Acknowledgements

- MobileNet SSD Caffe model by [chuanqi305](https://github.com/chuanqi305/MobileNet-SSD)
- OpenCV DNN + Tracking modules

---

## 🧾 License

This project is open-source and available under the [MIT License](LICENSE).

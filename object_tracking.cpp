#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/tracking.hpp>
#include <iostream>

using namespace cv;
using namespace dnn;
using namespace std;

// Defining Classes names
const vector<string> classNames = {
    "background", "aeroplane", "bicycle", "bird", "boat",
    "bottle", "bus", "car", "cat", "chair", "cow", "diningtable",
    "dog", "horse", "motorbike", "person", "pottedplant",
    "sheep", "sofa", "train", "tvmonitor"
};

// Only track these classes
const vector<string> classesToTrack = { "person", "car" };

// Redetection every n frames
const int REDETECTION_INTERVAL = 30;

vector<Ptr<Tracker>> trackers;
vector<Rect> boxes;

// helper function for checking if a perticular calss has to be tracked
bool shouldTrackClass(const string& className) {
    return find(classesToTrack.begin(), classesToTrack.end(), className) != classesToTrack.end();
}

int main() {
    // Loading Models
    Net net = readNetFromCaffe("MobileNetSSD_deploy.prototxt", "MobileNetSSD_deploy.caffemodel");


    // Capturing the frames through OpenCV
    VideoCapture cap("cars.mp4");
    if (!cap.isOpened()) {
        cerr << "Error opening video\n";
        return -1;
    }

    // Saving the video
    int frame_width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    int frame_height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    VideoWriter writer("output1.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 30, Size(frame_width, frame_height));

    Mat frame;
    int frameCount = 0;

    // main loop
    while (cap.read(frame)) {
        frameCount++;

        if (frameCount % REDETECTION_INTERVAL == 1 || trackers.empty()) {
            // Re-detection 
            trackers.clear();
            boxes.clear();

            Mat blob = blobFromImage(frame, 0.007843, Size(300, 300), Scalar(127.5,127.5,127.5), false);
            net.setInput(blob);
            Mat detections = net.forward();
            Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());
            float confidenceThreshold = 0.5;

            for (int i = 0; i < detectionMat.rows; i++) {
                float confidence = detectionMat.at<float>(i, 2);

                // Parsing the detected frames
                if (confidence > confidenceThreshold) {
                    int classId = static_cast<int>(detectionMat.at<float>(i, 1));
                    string className = classNames[classId];
                    if (!shouldTrackClass(className)) continue;

                    int xLeftBottom = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
                    int yLeftBottom = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
                    int xRightTop = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
                    int yRightTop = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);

                    // Plotting bounding boxes
                    Rect box(xLeftBottom, yLeftBottom, xRightTop - xLeftBottom, yRightTop - yLeftBottom);
                    rectangle(frame, box, Scalar(0, 255, 0), 2);

                    string label = format("%s: %.2f%%", className.c_str(), confidence * 100);
                    int baseLine;
                    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
                    int top = max(yLeftBottom, labelSize.height);
                    rectangle(frame, Point(xLeftBottom, top - labelSize.height),
                              Point(xLeftBottom + labelSize.width, top + baseLine),
                              Scalar(0, 255, 0), FILLED);
                    putText(frame, label, Point(xLeftBottom, top), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 1);


                    Ptr<Tracker> tracker = TrackerCSRT::create();
                    tracker->init(frame, box);
                    trackers.push_back(tracker);
                    boxes.push_back(box);
                }
            }
        } else {
            // Update trackers
            for (size_t i = 0; i < trackers.size(); ++i) {
                bool ok = trackers[i]->update(frame, boxes[i]);
                if (ok) {
                    rectangle(frame, boxes[i], Scalar(255, 0, 0), 2);
                } else {
                    putText(frame, "Lost", Point(50, 50 + (int)i * 20), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));
                }
            }
        }

        // Displaying frames 
        imshow("Multi Object Tracking", frame);
        writer.write(frame);
        if (waitKey(1) == 27) break;
    }

    // Releasing resources in the end
    cap.release();
    writer.release();
    destroyAllWindows();
    return 0;
}
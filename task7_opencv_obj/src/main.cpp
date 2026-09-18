#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main()
{
    // ==============================
    // COCO CLASS NAMES
    // ==============================

    vector<string> classNames = {
        "person", "bicycle", "car", "motorcycle", "airplane",
        "bus", "train", "truck", "boat", "traffic light",
        "fire hydrant", "stop sign", "parking meter", "bench",
        "bird", "cat", "dog", "horse", "sheep", "cow",
        "elephant", "bear", "zebra", "giraffe", "backpack",
        "umbrella", "handbag", "tie", "suitcase", "frisbee",
        "skis", "snowboard", "sports ball", "kite", "baseball bat",
        "baseball glove", "skateboard", "surfboard", "tennis racket",
        "bottle", "wine glass", "cup", "fork", "knife", "spoon",
        "bowl", "banana", "apple", "sandwich", "orange", "broccoli",
        "carrot", "hot dog", "pizza", "donut", "cake", "chair",
        "couch", "potted plant", "bed", "dining table", "toilet",
        "TV", "laptop", "mouse", "remote", "keyboard", "cell phone",
        "microwave", "oven", "toaster", "sink", "refrigerator",
        "book", "clock", "vase", "scissors", "teddy bear",
        "hair drier", "toothbrush"
    };

    // ==============================
    // LOAD YOLO MODEL
    // ==============================

    dnn::Net net = dnn::readNetFromONNX("../models/yolo26n.onnx");

    if (net.empty())
    {
        cout << "Error: Could not load YOLO model." << endl;
        return -1;
    }

    // ==============================
    // OPEN WEBCAM
    // ==============================

    VideoCapture cap(0);

    if (!cap.isOpened())
    {
        cout << "Error: Could not open webcam." << endl;
        return -1;
    }

    Mat frame;

    while (true)
    {
        cap >> frame;

        if (frame.empty())
            break;

        // ==============================
        // PREPROCESS
        // ==============================

        Mat blob = dnn::blobFromImage(
            frame,
            1.0 / 255.0,
            Size(640, 640),
            Scalar(),
            true,
            false
        );

        net.setInput(blob);

        // ==============================
        // RUN YOLO
        // ==============================

        vector<Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());

        // ==============================
        // RESHAPE YOLO OUTPUT
        // ==============================

        Mat output = outputs[0];

        Mat detections = output.reshape(1, output.total() / 6);

        // ==============================
        // PROCESS DETECTIONS
        // ==============================

        for (int i = 0; i < detections.rows; i++)
        {
            float x1 = detections.at<float>(i, 0);
            float y1 = detections.at<float>(i, 1);
            float x2 = detections.at<float>(i, 2);
            float y2 = detections.at<float>(i, 3);

            float confidence = detections.at<float>(i, 4);

            int classID =
                static_cast<int>(detections.at<float>(i, 5));

            if (confidence < 0.5)
                continue;

            // Convert 640x640 coordinates to webcam coordinates
            int left =
                static_cast<int>(x1 * frame.cols / 640.0);

            int top =
                static_cast<int>(y1 * frame.rows / 640.0);

            int right =
                static_cast<int>(x2 * frame.cols / 640.0);

            int bottom =
                static_cast<int>(y2 * frame.rows / 640.0);

            // Keep coordinates inside image
            left = max(0, left);
            top = max(0, top);
            right = min(frame.cols - 1, right);
            bottom = min(frame.rows - 1, bottom);

            // Draw bounding box
            rectangle(
                frame,
                Point(left, top),
                Point(right, bottom),
                Scalar(0, 255, 0),
                2
            );

            // Get class name
            string label;

            if (classID >= 0 && classID < classNames.size())
                label = classNames[classID];
            else
                label = "Unknown";

            label += " " +
                     to_string(confidence).substr(0, 4);

            // Draw label
            putText(
                frame,
                label,
                Point(left, max(top - 10, 20)),
                FONT_HERSHEY_SIMPLEX,
                0.6,
                Scalar(0, 255, 0),
                2
            );
        }

        // ==============================
        // DISPLAY
        // ==============================

        imshow("Rudra Object Detection", frame);

        if (waitKey(1) == 'q')
            break;
    }

    cap.release();
    destroyAllWindows();

    return 0;
}

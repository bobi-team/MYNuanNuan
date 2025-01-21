
// SGHTool.cpp
#include "SGHTool.h"

SGHTool::SGHTool(const std::string& imagePath) : scale(1.0), center(0, 0), windowName("Image Zoom Tool") {
    image = cv::imread(imagePath);
    if (image.empty()) {
        throw std::runtime_error("Could not open or find the image!");
    }
}

SGHTool::~SGHTool() {
    cv::destroyWindow(windowName);
}

void SGHTool::onMouse(int event, int x, int y, int flags, void* userdata) {
    SGHTool* tool = static_cast<SGHTool*>(userdata);
    if (event == cv::EVENT_MOUSEWHEEL) {
        int delta = cv::getMouseWheelDelta(flags);
        if (delta > 0) {
            tool->scale *= 1.1; // �Ŵ�
        }
        else {
            tool->scale /= 1.1; // ��С
        }
        tool->scale = std::max(0.1, tool->scale); // ������С���ű���

        // ������������
        tool->center = cv::Point2f(x, y);
    }
}

void SGHTool::displayImage() {
    cv::Mat resized;
    cv::resize(image, resized, cv::Size(), scale, scale, cv::INTER_LINEAR);

    // ����ƫ�����Ա����������Ĳ���
    cv::Point2f offset = center * (1 - scale);
    cv::Mat translationMatrix = (cv::Mat_<float>(2, 3) << 1, 0, offset.x, 0, 1, offset.y);
    cv::warpAffine(resized, resized, translationMatrix, resized.size());

    cv::imshow(windowName, resized);
}

void SGHTool::run() {
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(windowName, onMouse, this);

    while (true) {
        displayImage();

        char c = (char)cv::waitKey(10);
        if (c == 27) { // ����ESC���˳�
            break;
        }
    }
}


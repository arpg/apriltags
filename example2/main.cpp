#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/tracking.hpp>
#include <HAL/Camera/CameraDevice.h>

int main() {
#if 0
  hal::Camera camera = hal::Camera( hal::Uri("opencv:[id=0]") );
  std::shared_ptr<hal::ImageArray> images = hal::ImageArray::Create();
  std::shared_ptr<hal::Image> im;
  std::cout << "OK so far." << std::endl;
#endif

#if 1
  cv::VideoCapture cap;
  cap.open(0);

  if (!cap.isOpened()) {
    std::cerr << "***Could not initialize capturing...***\n";
    std::cerr << "Current parameter's value: \n";
    return -1;
  }

  cv::Mat frame;
  while (1) {
    cap >> frame;
    if (frame.empty()) {
      std::cerr << "frame is empty" << std::endl;
      break;
    }

    cv::imshow("", frame);
    cv::waitKey(10);
  }
#endif
  return 1;
}

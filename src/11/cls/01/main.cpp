#include <iostream>
#include <opencv2/opencv.hpp>

#define IMG_W 180
#define IMG_H 100
#define HUE_MAX 180
#define SAT_MAX 255
#define VAL_MAX 255

uchar to_uchar(const int value)
{
  // ref: http://opencv.jp/opencv-2.2/cpp/saturation_arithmetics.html
  return cv::saturate_cast<uchar>(value);
}

int main(int argc, const char *argv[])
{
  auto output_hsv = cv::Mat(cv::Size(IMG_W, IMG_H), CV_8UC3);
  output_hsv.forEach<cv::Vec3b>([](cv::Vec3b &pixel, const int *position) -> void {
    auto [y, x] = std::make_pair(position[0], position[1]);

    auto h = to_uchar(x * HUE_MAX / IMG_W);
    auto s = to_uchar(SAT_MAX);
    auto v = to_uchar(VAL_MAX);

    pixel = cv::Vec3b(h, s, v);
  });

  auto output_bgr = cv::Mat(cv::Size(IMG_W, IMG_H), CV_8UC3);
  cv::cvtColor(output_hsv, output_bgr, cv::COLOR_HSV2BGR);

  cv::imshow("Hue Bar", output_bgr);
  cv::waitKey();

  return 0;
}

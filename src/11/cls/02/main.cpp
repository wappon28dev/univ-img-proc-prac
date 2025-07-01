#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/fruit_image.jpg"
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
  auto input = cv::imread(FILE_NAME_INPUT);
  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }
  auto input_hsv = cv::Mat(input.size(), CV_8UC3);
  cv::cvtColor(input, input_hsv, cv::COLOR_BGR2HSV);

  auto output_hsv = input_hsv.clone();
  output_hsv.forEach<cv::Vec3b>([](cv::Vec3b &pixel, const int *position) -> void {
    auto [h, s, v] = std::make_tuple(pixel[0], pixel[1], pixel[2]);

    h = (h + HUE_MAX - 90) % HUE_MAX;
    // v = std::max(0, v - 50);

    pixel = cv::Vec3b(to_uchar(h), to_uchar(s), to_uchar(v));
  });

  auto output_bgr = cv::Mat(input.size(), CV_8UC3);
  cv::cvtColor(output_hsv, output_bgr, cv::COLOR_HSV2BGR);

  cv::imshow("output", output_bgr);
  cv::waitKey();

  return 0;
}

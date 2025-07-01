#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/handinwhite.jpg"
#define HUE_MAX 180
#define SAT_MAX 255
#define VAL_MAX 255

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
  auto black = cv::Vec3b(0, 0, 0);
  output_hsv.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *position) -> void {
    auto [h, s, v] = std::make_tuple(pixel[0], pixel[1], pixel[2]);
    auto is_foreground = ((h >= 0 && h <= 20) || (h >= 160 && h <= 180)) && (s >= 30) && (v >= 50);

    pixel = is_foreground ? pixel : black;
  });

  auto output_bgr = cv::Mat(input.size(), CV_8UC3);
  cv::cvtColor(output_hsv, output_bgr, cv::COLOR_HSV2BGR);

  cv::imshow("output_bgr", output_bgr);
  cv::waitKey();

  return 0;
}

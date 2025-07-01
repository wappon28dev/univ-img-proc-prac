#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/fruit_image.jpg"
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
  auto white = cv::Vec3b(HUE_MAX, SAT_MAX, VAL_MAX);
  output_hsv.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *position) -> void {
    auto [h, s, v] = std::make_tuple(pixel[0], pixel[1], pixel[2]);
    auto cond = h >= 50 && v <= 200;

    pixel = cond ? white : black;
  });

  auto output_bgr = cv::Mat(input.size(), CV_8UC3);
  cv::cvtColor(output_hsv, output_bgr, cv::COLOR_HSV2BGR);

  auto output_bin = cv::Mat(input.size(), CV_8UC1);
  cv::cvtColor(output_bgr, output_bin, cv::COLOR_BGR2GRAY);
  cv::threshold(output_bin, output_bin, 0, 255, cv::THRESH_BINARY);

  cv::imshow("output_bin", output_bin);
  cv::waitKey();

  return 0;
}

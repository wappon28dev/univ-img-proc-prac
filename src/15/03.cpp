#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "calendar.jpg"
#define FILE_NAME_TPL "cake.jpg"

int main(int argc, const char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT);
  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto is_saturday = [&](cv::Vec3b &pixel) -> bool {
    auto [b, g, r] = std::make_tuple(pixel[0], pixel[1], pixel[2]);
    return (b >= 130 && r <= 50 && g <= 100);
  };

  auto color_white = cv::Vec3b(255, 255, 255);
  auto output = input.clone();

  output.forEach<cv::Vec3b>(
      [&](cv::Vec3b &pixel, const int *position) -> void { pixel = is_saturday(pixel) ? pixel : color_white; });

  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

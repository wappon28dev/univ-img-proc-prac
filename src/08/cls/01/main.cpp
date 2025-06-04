#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/red_rectangle.jpg"
#define THRESHOLD 100

uchar to_uchar(const int value)
{
  // ref: http://opencv.jp/opencv-2.2/cpp/saturation_arithmetics.html
  return cv::saturate_cast<uchar>(value);
}

int to_binarized(int value)
{
  auto is_foreground = (value <= THRESHOLD);
  return is_foreground ? 255 : 0;
}

int main(int argc, char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT, cv::IMREAD_GRAYSCALE);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto output = input.clone();

  output.forEach<uchar>([&](uchar &pixel, auto _) -> void {
    auto binarized = to_binarized(static_cast<int>(pixel));
    pixel = to_uchar(binarized);
  });

  cv::imshow("input", input);
  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

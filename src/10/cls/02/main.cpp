#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/a_img.jpg"
#define THRESHOLD 100

int main(int argc, const char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT, cv::IMREAD_GRAYSCALE);
  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto input_bin = cv::Mat(input.size(), input.type());
  cv::threshold(input, input_bin, THRESHOLD, 255, cv::THRESH_BINARY);

  for (int i = 0; i < 10; ++i)
  {
    auto input_dilated = cv::Mat(input_bin.size(), input_bin.type());
    auto input_eroded = cv::Mat(input_bin.size(), input_bin.type());

    cv::dilate(input_bin, input_dilated, cv::Mat(), cv::Point(-1, -1), i);
    cv::erode(input_dilated, input_eroded, cv::Mat(), cv::Point(-1, -1), i);

    cv::imshow("input_dilated x" + std::to_string(i), input_dilated);
    cv::imshow("input_eroded x" + std::to_string(i), input_eroded);
    cv::waitKey();
  }

  return 0;
}

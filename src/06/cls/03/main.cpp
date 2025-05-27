#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#define FILE_NAME_INPUT "../assets/apple_grayscale.jpg"

int main(int argc, char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT, cv::IMREAD_GRAYSCALE);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto output = cv::Mat(input.size(), input.type());

  cv::Sobel(input, output, CV_64F, 1, 0);
  cv::convertScaleAbs(output, output, 1, 0);

  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

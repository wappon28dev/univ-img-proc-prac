#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/AIT-logo2.jpg"
#define THRESHOLD 110

int main(int argc, const char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT);
  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto input_bin = cv::Mat(input.size(), CV_8UC1);
  cv::cvtColor(input, input_bin, cv::COLOR_BGR2GRAY);
  // NOTE: JPEG 圧縮によるノイズを除去するためにブラーを適用
  cv::GaussianBlur(input_bin, input_bin, cv::Size(1, 1), 0);
  cv::threshold(input_bin, input_bin, THRESHOLD, 255, cv::THRESH_BINARY_INV);

  // NOTE: `findContours` は入力画像を破壊的に変更するため一旦クローン
  auto input_bin_cloned = input_bin.clone();
  auto contours = std::vector<std::vector<cv::Point>>();
  cv::findContours(input_bin_cloned, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

  auto output = cv::Mat(input.size(), CV_8UC1, cv::Scalar(0));
  cv::drawContours(output, contours, -1, 255, 1);

  cv::imshow("input", input);
  cv::imshow("input_bin", input_bin);
  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

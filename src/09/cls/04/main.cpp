#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/Japan.png"
#define THRESHOLD 110

void print_contours_stat(const int idx, const std::vector<cv::Point> &contour)
{
  auto arc_len = cv::arcLength(contour, true);
  auto area = cv::contourArea(contour);
  auto circularity = (4 * CV_PI * area) / (arc_len * arc_len);

  printf("%2d: L=%10.2f, S=%10.2f, R=%.2f\n", idx, arc_len, area, circularity);
}

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
  // NOTE: 画像全体の矩形を検出させるために外枠を追加
  auto rect = cv::Rect(0, 0, input_bin.cols, input_bin.rows);
  auto black = cv::Scalar(0);
  cv::rectangle(input_bin, rect, black, 3);
  cv::threshold(input_bin, input_bin, THRESHOLD, 255, cv::THRESH_BINARY);

  // NOTE: `findContours` は入力画像を破壊的に変更するため一旦クローン
  auto input_bin_cloned = input_bin.clone();
  auto contours = std::vector<std::vector<cv::Point>>();
  cv::findContours(input_bin_cloned, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

  auto output = input.clone();

  auto idx = 0;
  for (const auto &contour : contours)
  {
    auto rect = cv::boundingRect(contour);
    auto magenta = cv::Vec3b(255, 0, 255);
    cv::rectangle(output, rect, magenta, 2);

    print_contours_stat(idx, contour);

    idx++;
  }

  cv::imshow("input", input);
  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/sample.jpg"
#define THRESHOLD 100

void set_output(cv::Mat &output, const cv::Mat &input, const std::vector<cv::Point> &contour)
{
  auto contour_arc_len = cv::arcLength(contour, true);
  auto contour_area = cv::contourArea(contour);

  std::cout << "周囲長: " << contour_arc_len << " 面積: " << contour_area << std::endl;

  auto magenta = cv::Vec3b(255, 0, 255);
  auto rect = cv::boundingRect(contour);
  cv::rectangle(output, rect, magenta, 3);
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
  cv::threshold(input_bin, input_bin, THRESHOLD, 255, cv::THRESH_BINARY);

  // NOTE: `findContours` は入力画像を破壊的に変更するため一旦クローン
  auto input_bin_cloned = input_bin.clone();
  auto contours = std::vector<std::vector<cv::Point>>();
  cv::findContours(input_bin_cloned, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

  auto contours_len = contours.size();
  std::cout << "輪郭の数=" << contours_len << std::endl;

  int target_contour_idx;
  std::cout << "輪郭番号？ (0〜" << contours_len - 1 << "): ";
  std::cin >> target_contour_idx;
  auto is_valid_idx = target_contour_idx >= 0 && target_contour_idx < contours_len;
  if (!is_valid_idx)
  {
    std::cerr << "無効な輪郭番号です。" << std::endl;
    return -1;
  }

  auto target_contour = contours[target_contour_idx];
  auto output = input.clone();
  set_output(output, input, target_contour);

  cv::imshow("input", input);
  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

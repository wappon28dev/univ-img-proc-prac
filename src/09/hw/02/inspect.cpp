#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT_1 "./assets/sample.jpg"
#define FILE_NAME_INPUT_2 "./assets/sample_rotate.jpg"
#define THRESHOLD 120

typedef struct
{
  int idx;
  double height;
  double width;
  double arc_len;
  double area;
  double circularity;
  bool is_rect;
} ContourStat;

ContourStat calc_contour_stat(const int idx, const std::vector<cv::Point> &contour)
{
  auto arc_len = cv::arcLength(contour, true);
  auto area = cv::contourArea(contour);
  auto circularity = (4 * M_PI * area) / (arc_len * arc_len);
  auto rect = cv::boundingRect(contour);
  auto height = static_cast<double>(rect.height);
  auto width = static_cast<double>(rect.width);
  // NOTE: 実面積と矩形面積の比率で矩形かどうかを判定.  誤差 10% なら矩形とみなす
  auto is_rect = 1 - (area / (height * width)) < 0.1;

  return {idx, height, width, arc_len, area, circularity, is_rect};
}

void print_contours_stat(const ContourStat &stat)
{
  printf("%2d: L=%10.2f, S=%10.2f, R=%.2f, H=%10.2f, W=%10.2f, IS_RECT=%s\n", stat.idx, stat.arc_len, stat.area,
         stat.circularity, stat.height, stat.width, stat.is_rect ? "true" : "false");
}

int main(int argc, const char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT_1);
  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto input_bin = cv::Mat(input.size(), CV_8UC1);
  cv::cvtColor(input, input_bin, cv::COLOR_BGR2GRAY);
  cv::threshold(input_bin, input_bin, THRESHOLD, 255, cv::THRESH_BINARY_INV);

  // NOTE: `findContours` は入力画像を破壊的に変更するため一旦クローン
  auto input_bin_cloned = input_bin.clone();
  auto contours = std::vector<std::vector<cv::Point>>();
  cv::findContours(input_bin_cloned, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

  auto output = input.clone();
  auto white = cv::Vec3b(255, 255, 255);
  cv::drawContours(output, contours, -1, white, 2, cv::LINE_8);

  auto idx = 0;
  std::for_each(contours.begin(), contours.end(), [&](auto &contour) {
    auto center = cv::Point(cv::boundingRect(contour).x + 5, cv::boundingRect(contour).y + 5);
    cv::putText(output, std::to_string(idx), center, cv::FONT_HERSHEY_SIMPLEX, 0.5, white, 1);
    auto stat = calc_contour_stat(idx, contour);
    print_contours_stat(stat);
    idx++;
  });

  cv::imshow("input", input);
  cv::imshow("input_bin", input_bin);
  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

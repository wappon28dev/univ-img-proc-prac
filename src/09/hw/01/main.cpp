#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/sample.jpg"
#define THRESHOLD 120

typedef struct
{
  int idx;
  double arc_len;
  double area;
  double circularity;
} ContourStat;

ContourStat calc_contour_stat(const int idx, const std::vector<cv::Point> &contour)
{
  auto arc_len = cv::arcLength(contour, true);
  auto area = cv::contourArea(contour);
  auto circularity = (4 * M_PI * area) / (arc_len * arc_len);

  return {idx, arc_len, area, circularity};
}

void print_contours_stat(const ContourStat &stat)
{
  printf("%2d: L=%10.2f, S=%10.2f, R=%.2f\n", stat.idx, stat.arc_len, stat.area, stat.circularity);
}

int get_index_inferred_star_shape(const std::vector<std::vector<cv::Point>> &contours)
{
  auto stats = std::vector<ContourStat>();
  {
    auto idx = 0;
    auto mapper = [&idx](const std::vector<cv::Point> &contour) { return calc_contour_stat(idx++, contour); };
    std::transform(contours.begin(), contours.end(), std::back_inserter(stats), mapper);
    std::for_each(stats.begin(), stats.end(), print_contours_stat);
  }

  // NOTE: 星型の輪郭 ... 最小円形度をもつ輪郭
  auto comparer = [](const ContourStat &a, const ContourStat &b) { return a.circularity < b.circularity; };
  auto stats_min_iter = std::min_element(stats.begin(), stats.end(), comparer);

  return stats_min_iter->idx;
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
  cv::threshold(input_bin, input_bin, THRESHOLD, 255, cv::THRESH_BINARY_INV);

  // NOTE: `findContours` は入力画像を破壊的に変更するため一旦クローン
  auto input_bin_cloned = input_bin.clone();
  auto contours = std::vector<std::vector<cv::Point>>();
  cv::findContours(input_bin_cloned, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

  auto target_idx = get_index_inferred_star_shape(contours);

  auto output = input.clone();
  auto white = cv::Vec3b(255, 255, 255);
  cv::drawContours(output, contours, target_idx, white, 2, cv::LINE_8);

  cv::imshow("input", input);
  cv::imshow("input_bin", input_bin);
  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

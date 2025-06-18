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
  auto is_rect = (1 - (area / (height * width))) < 0.1;

  return {idx, height, width, arc_len, area, circularity, is_rect};
}

void print_contours_stat(const ContourStat &stat)
{
  printf("%2d: L=%10.2f, S=%10.2f, R=%.2f, H=%10.2f, W=%10.2f, IS_RECT=%s\n", stat.idx, stat.arc_len, stat.area,
         stat.circularity, stat.height, stat.width, stat.is_rect ? "true" : "false");
}

/**
 * 問題にて指定された図形領域らのインデックスを返す.
 *
 * +----------------------------------------+
 * | +-------------------------+    S_0     |
 * | |                     S_1 |  (画像全体) |
 * | | +--------------------+  |            |
 * | | |                S_2 |  |            |
 * | | |         +------+   |  |            |
 * | | |         |  S_3 |   |  |            |
 * | | |         +------+   |  |            |
 * | | +--------------------+  |            |
 * | +-------------------------+            |
 * +----------------------------------------+
 *
 * @return 図形領域のインデックス (<S_2>, <S_3>) のペア.
 */
std::pair<int, int> infer_rect_index(const std::vector<std::vector<cv::Point>> &contours)
{
  auto stats = std::vector<ContourStat>();
  {
    auto idx = 0;
    auto mapper = [&idx](const std::vector<cv::Point> &contour) { return calc_contour_stat(idx++, contour); };
    std::transform(contours.begin(), contours.end(), std::back_inserter(stats), mapper);
    std::for_each(stats.begin(), stats.end(), print_contours_stat);
  }

  auto stats_rect = std::vector<ContourStat>();
  {
    auto cond = [](const ContourStat &stat) { return stat.is_rect; };
    std::copy_if(stats.begin(), stats.end(), std::back_inserter(stats_rect), cond);
  }

  auto stats_rect_sorted = stats_rect;
  {
    auto comparer = [](const ContourStat &a, const ContourStat &b) { return a.height > b.height; };
    std::sort(stats_rect_sorted.begin(), stats_rect_sorted.end(), comparer);
  }

  return std::make_pair(stats_rect_sorted[2].idx, stats_rect_sorted[3].idx);
}

int main(int argc, const char *argv[])
{
  auto input1 = cv::imread(FILE_NAME_INPUT_1);
  auto input2 = cv::imread(FILE_NAME_INPUT_2);
  if (input1.empty() || input2.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  for (auto &input : {input1, input2})
  {
    auto input_bin = cv::Mat(input.size(), CV_8UC1);
    cv::cvtColor(input, input_bin, cv::COLOR_BGR2GRAY);
    cv::threshold(input_bin, input_bin, THRESHOLD, 255, cv::THRESH_BINARY_INV);

    // NOTE: `findContours` は入力画像を破壊的に変更するため一旦クローン
    auto input_bin_cloned = input_bin.clone();
    auto contours = std::vector<std::vector<cv::Point>>();
    cv::findContours(input_bin_cloned, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

    auto output = input.clone();
    auto white = cv::Vec3b(255, 255, 255);
    auto magenta = cv::Vec3b(255, 0, 255);
    auto [idx_parent, idx_child] = infer_rect_index(contours);
    std::cout << "親図形: " << idx_parent << ", 子図形: " << idx_child << std::endl;
    cv::drawContours(output, contours, idx_parent, white, -1);
    cv::drawContours(output, contours, idx_child, magenta, -1);

    cv::imshow("output", output);
    cv::waitKey();
  }

  return 0;
}

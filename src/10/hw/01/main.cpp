#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/issue1.jpg"
#define THRESHOLD 100

// ref: src/09/hw/02/main.cpp
typedef struct ContourStat
{
  int idx;
  cv::Rect rect;
  double arc_len;
  double area;
  double circularity;
} ContourStat;

ContourStat calc_contour_stat(const int idx, const std::vector<cv::Point> &contour)
{
  auto rect = cv::boundingRect(contour);
  auto arc_len = cv::arcLength(contour, true);
  auto area = cv::contourArea(contour);
  auto circularity = (4 * M_PI * area) / (arc_len * arc_len);

  return {idx, rect, arc_len, area, circularity};
}

void make_contour_stat(const std::vector<std::vector<cv::Point>> &contours, std::vector<ContourStat> &stats)
{
  auto idx = 0;
  auto mapper = [&idx](const std::vector<cv::Point> &contour) { return calc_contour_stat(idx++, contour); };
  std::transform(contours.begin(), contours.end(), std::back_inserter(stats), mapper);
}

void print_contours_stat(const ContourStat &stat)
{
  auto height = static_cast<double>(stat.rect.height);
  auto width = static_cast<double>(stat.rect.width);

  printf("%2d: L=%10.2f, S=%10.2f, R=%.2f, H=%10.2f, W=%10.2f\n", //
         stat.idx, stat.arc_len, stat.area, stat.circularity, height, width);
}

int main(int argc, const char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT, cv::IMREAD_GRAYSCALE);
  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto input_bin = cv::Mat(input.size(), CV_8UC1);
  cv::threshold(input, input_bin, THRESHOLD, 255, cv::THRESH_BINARY);

  auto output_without_circle = input_bin.clone();

  // 輪郭から円を検出 → 塗りつぶし
  auto contours = std::vector<std::vector<cv::Point>>();
  cv::findContours(output_without_circle, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

  auto output_for_stat = output_without_circle.clone();

  // 18 回膨張 → 18 回収縮
  for (int i = 0; i < 18; i++)
  {
    cv::dilate(output_for_stat, output_for_stat, cv::Mat());
  }
  for (int i = 0; i < 18; i++)
  {
    cv::erode(output_for_stat, output_for_stat, cv::Mat());
  }

  auto stats = std::vector<ContourStat>();
  make_contour_stat(contours, stats);

  auto stats_circle = std::vector<ContourStat>();
  {
    auto pred = [](const ContourStat &stat) { return stat.circularity > 0.8; };
    std::copy_if(stats.begin(), stats.end(), std::back_inserter(stats_circle), pred);
  }

  // 円と検出されたものを黒で塗りつぶす
  auto black = cv::Scalar(0, 0, 0);
  std::for_each(stats_circle.begin(), stats_circle.end(), [&](const ContourStat &stat) {
    cv::drawContours(output_without_circle, contours, stat.idx, black, -1);
  });

  auto contours_without_circle = std::vector<std::vector<cv::Point>>();
  cv::findContours(output_without_circle, contours_without_circle, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

  auto stats_without_circle = std::vector<ContourStat>();
  make_contour_stat(contours_without_circle, stats_without_circle);

  // 面積の大きい順にソート
  auto stats_merged_sorted = stats_without_circle;
  {
    auto cmp = [](const ContourStat &a, const ContourStat &b) { return a.area > b.area; };
    std::sort(stats_merged_sorted.begin(), stats_merged_sorted.end(), cmp);
    std::for_each(stats_merged_sorted.begin(), stats_merged_sorted.end(), print_contours_stat);
  }

  // マゼンダ ... 最も面積が大きい, 赤色 ... 次に面積が大きい
  auto stat0 = stats_merged_sorted.at(0);
  auto magenta = cv::Vec3b(255, 0, 255);
  auto stat1 = stats_merged_sorted.at(1);
  auto red = cv::Vec3b(0, 0, 255);

  auto output_colored = cv::Mat(output_without_circle.size(), CV_8UC3);
  cv::cvtColor(output_without_circle, output_colored, cv::COLOR_GRAY2BGR);

  auto white = cv::Vec3b(255, 255, 255);
  output_colored.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *position) -> void {
    auto [x, y] = std::make_pair(position[1], position[0]);
    auto point = cv::Point2f(x, y);

    if (stat0.rect.contains(point))
    {
      pixel = pixel == white ? magenta : pixel;
    }

    if (stat1.rect.contains(point))
    {
      pixel = pixel == white ? red : pixel;
    }
  });

  // ↓ チート
  // auto roi0_color = output_colored(stat0.rect);
  // auto roi0_mask = output_without_circle(stat0.rect);
  // roi0_color.setTo(magenta, roi0_mask);

  // auto roi1_color = output_colored(stat1.rect);
  // auto roi1_mask = output_without_circle(stat1.rect);
  // roi1_color.setTo(red, roi1_mask);

  cv::imshow("output_colored", output_colored);
  cv::waitKey();

  return 0;
}

#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/issue1.jpg"
#define THRESHOLD 100

// ref: https://qiita.com/IgnorantCoder/items/3101d6276e9bdddf872c
namespace vec_util
{

template <typename A, typename F> auto map(const A &v, F &&f)
{
  using R = std::vector<decltype(f(*v.begin()))>;
  R y;
  y.reserve(v.size());
  std::transform(std::cbegin(v), std::cend(v), std::back_inserter(y), f);
  return y;
}

template <typename A, typename F> auto filter(const A &v, F &&pred)
{
  A y;
  std::copy_if(std::cbegin(v), std::cend(v), std::back_inserter(y), pred);
  return y;
}

template <typename A, typename F> void for_each(const A &v, F &&f)
{
  std::for_each(std::cbegin(v), std::cend(v), f);
}
} // namespace vec_util

using Contour = std::vector<cv::Point>;
struct ContourStat
{
  int idx;
  cv::Rect rect;
  double arc_len;
  double area;
  double circularity;

  static ContourStat from(const int idx, const Contour &contour)
  {
    auto rect = cv::boundingRect(contour);
    auto arc_len = cv::arcLength(contour, true);
    auto area = cv::contourArea(contour);
    auto circularity = (4 * M_PI * area) / (arc_len * arc_len);

    return {idx, rect, arc_len, area, circularity};
  }

  void print() const
  {
    auto height = static_cast<double>(rect.height);
    auto width = static_cast<double>(rect.width);

    printf("%2d: L=%10.2f, S=%10.2f, R=%.2f, H=%10.2f, W=%10.2f\n", //
           idx, arc_len, area, circularity, height, width);
  }
};

void make_contour_stats(std::vector<ContourStat> &stats, const std::vector<Contour> &contours)
{
  auto idx = 0;
  auto mapper = [&idx](const Contour &contour) { return ContourStat::from(idx++, contour); };
  stats = vec_util::map(contours, mapper);
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
  make_contour_stats(stats, contours);

  auto stats_circle = vec_util::filter(stats, [](const auto &stat) { return stat.circularity > 0.8; });

  // 円と検出されたものを黒で塗りつぶす
  auto black = cv::Scalar(0, 0, 0);
  vec_util::for_each(stats_circle,
                     [&](const auto &stat) { cv::drawContours(output_without_circle, contours, stat.idx, black, -1); });

  auto contours_without_circle = std::vector<std::vector<cv::Point>>();
  cv::findContours(output_without_circle, contours_without_circle, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

  auto stats_without_circle = std::vector<ContourStat>();
  make_contour_stats(stats_without_circle, contours_without_circle);

  // 面積の大きい順にソート
  auto stats_merged_sorted = stats_without_circle;
  {
    auto cmp = [](const ContourStat &a, const ContourStat &b) { return a.area > b.area; };
    std::sort(stats_merged_sorted.begin(), stats_merged_sorted.end(), cmp);
    vec_util::for_each(stats_merged_sorted, [](const auto &stat) { stat.print(); });
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

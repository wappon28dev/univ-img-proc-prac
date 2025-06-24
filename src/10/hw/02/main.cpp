#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/challenge.jpg"
#define THRESHOLD 200

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

template <typename A, typename F> auto filter(const A &v, F &&f)
{
  A y;
  std::copy_if(std::cbegin(v), std::cend(v), std::back_inserter(y), f);
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
  stats = vec_util::map(contours, [&](const auto &contour) { return ContourStat::from(idx++, contour); });
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

  auto contours = std::vector<Contour>();
  cv::findContours(input_bin, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

  auto stats = std::vector<ContourStat>();
  make_contour_stats(stats, contours);

  auto stats_filtered = vec_util::filter(stats, [](const auto &stat) { return stat.area >= 50; });
  std::cout << stats_filtered.size() << std::endl;

  auto output = cv::Mat(input_bin.size(), input_bin.type(), cv::Scalar(0));
  auto white = cv::Scalar(255);
  vec_util::for_each(stats_filtered,
                     [&](const auto &stat) { cv::drawContours(output, contours, stat.idx, white, -1); });

  cv::imshow("input_bin", input_bin);
  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

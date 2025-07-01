#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/fruit_image.jpg"
#define HUE_MAX 180
#define SAT_MAX 255
#define VAL_MAX 255
#define HUE_APPLE 5

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
  auto input = cv::imread(FILE_NAME_INPUT);
  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }
  auto input_hsv = cv::Mat(input.size(), CV_8UC3);
  cv::cvtColor(input, input_hsv, cv::COLOR_BGR2HSV);

  auto temp_recolor_hsv = input_hsv.clone();
  auto black = cv::Vec3b(0, 0, 0);
  auto white = cv::Vec3b(0, 0, VAL_MAX);
  temp_recolor_hsv.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *position) -> void {
    auto [h, s, v] = std::make_tuple(pixel[0], pixel[1], pixel[2]);
    auto is_apple_red = (31 <= h && h <= 128) && (70 <= s && s <= 255) && (100 <= v && v <= 255);

    pixel = is_apple_red ? white : black;
  });

  auto temp_recolor_bgr = cv::Mat(input.size(), CV_8UC3);
  cv::cvtColor(temp_recolor_hsv, temp_recolor_bgr, cv::COLOR_HSV2BGR);

  auto temp_recolor_bin = cv::Mat(input.size(), CV_8UC1);
  cv::cvtColor(temp_recolor_bgr, temp_recolor_bin, cv::COLOR_BGR2GRAY);
  cv::threshold(temp_recolor_bin, temp_recolor_bin, 0, 255, cv::THRESH_BINARY);

  auto contours = std::vector<Contour>();
  cv::findContours(temp_recolor_bin, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

  auto contour_stats = std::vector<ContourStat>();
  make_contour_stats(contour_stats, contours);

  auto contour_stats_sorted = contour_stats;
  {
    auto comparer = [](const ContourStat &a, const ContourStat &b) { return a.area > b.area; };
    std::sort(contour_stats_sorted.begin(), contour_stats_sorted.end(), comparer);
  }

  auto output_hsv = input_hsv.clone();
  auto roi = contour_stats_sorted.at(0).rect;
  output_hsv.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *position) -> void {
    auto [y, x] = std::make_tuple(position[0], position[1]);
    auto point = cv::Point(x, y);
    auto temp_pixel = temp_recolor_hsv.at<cv::Vec3b>(y, x);

    if (roi.contains(point) && temp_pixel == white)
    {
      auto [h, s, v] = std::make_tuple(pixel[0], pixel[1], pixel[2]);
      pixel = cv::Vec3b(HUE_APPLE, s, v);
    }
  });

  auto output_bgr = cv::Mat(input.size(), CV_8UC3);
  cv::cvtColor(output_hsv, output_bgr, cv::COLOR_HSV2BGR);

  cv::imshow("output_bgr", output_bgr);
  cv::waitKey();

  return 0;
}

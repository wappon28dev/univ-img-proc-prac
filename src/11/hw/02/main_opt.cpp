#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

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
} // namespace vec_util

using Contour = std::vector<cv::Point>;
struct ContourStat
{
  int idx;
  double area;

  static ContourStat from(const int idx, const Contour &contour)
  {
    return {idx, cv::contourArea(contour)};
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
    std::cerr << "読み込み失敗" << std::endl;
    return -1;
  }
  auto input_hsv = cv::Mat(input.size(), CV_8UC3);
  cv::cvtColor(input, input_hsv, cv::COLOR_BGR2HSV);

  // 1. cv::inRangeでリンゴの色の範囲に合致する二値マスクを直接生成
  cv::Mat apple_mask;
  const cv::Scalar lower_bound(31, 70, 100);
  const cv::Scalar upper_bound(128, 255, 255);
  cv::inRange(input_hsv, lower_bound, upper_bound, apple_mask);

  // 2. マスクから輪郭を抽出
  // 最も外側の輪郭のみを検出する RETR_EXTERNAL がこのケースでは効率的
  auto contours = std::vector<Contour>();
  cv::findContours(apple_mask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

  if (contours.empty())
  {
    std::cerr << "対象の輪郭が見つかりませんでした。" << std::endl;
    return -1;
  }

  // 3. 面積が最大の輪郭を特定
  auto contour_stats = std::vector<ContourStat>();
  make_contour_stats(contour_stats, contours);
  std::sort(contour_stats.begin(), contour_stats.end(),
            [](const ContourStat &a, const ContourStat &b) { return a.area > b.area; });

  // 4. 最大の輪郭の内側だけを塗りつぶした、最終的なマスク画像を生成
  cv::Mat largest_contour_mask = cv::Mat::zeros(apple_mask.size(), CV_8UC1);
  const int largest_contour_idx = contour_stats.at(0).idx;
  cv::drawContours(largest_contour_mask, contours, largest_contour_idx, cv::Scalar(255), cv::FILLED);

  // 5. マスクを使い、指定した部分の色相(H)のみを変更
  // 元のHSV画像をH, S, Vチャンネルに分割
  std::vector<cv::Mat> hsv_channels;
  cv::split(input_hsv, hsv_channels);

  // マスクが白(255)の部分だけ、Hチャンネルの値を HUE_APPLE に設定
  hsv_channels[0].setTo(HUE_APPLE, largest_contour_mask);

  // 6. チャンネルを再結合し、BGR色空間に戻す
  auto output_hsv = cv::Mat();
  cv::merge(hsv_channels, output_hsv);

  auto output_bgr = cv::Mat(input.size(), CV_8UC3);
  cv::cvtColor(output_hsv, output_bgr, cv::COLOR_HSV2BGR);

  cv::imshow("Original", input);
  cv::imshow("Final Mask", largest_contour_mask);
  cv::imshow("Output", output_bgr);
  cv::waitKey();

  return 0;
}

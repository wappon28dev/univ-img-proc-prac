#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/challenge.jpg"
#define THRESHOLD 200

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

  auto contours = std::vector<std::vector<cv::Point>>();
  cv::findContours(input_bin, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

  auto stats = std::vector<ContourStat>();
  make_contour_stat(contours, stats);

  auto stats_filtered = std::vector<ContourStat>();
  {
    auto pred = [](const ContourStat &stat) { return stat.area >= 50; };
    std::copy_if(stats.begin(), stats.end(), std::back_inserter(stats_filtered), pred);
  }

  // 50画素以上の白斑点の個数
  std::cout << stats_filtered.size() << std::endl;

  auto output = cv::Mat(input_bin.size(), input_bin.type(), cv::Scalar(0));
  std::for_each(stats_filtered.begin(), stats_filtered.end(), [&](const ContourStat &stat) {
    auto white = cv::Scalar(255);
    cv::drawContours(output, contours, stat.idx, white, -1);
  });

  cv::imshow("input_bin", input_bin);
  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

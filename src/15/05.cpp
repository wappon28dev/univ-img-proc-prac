#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "calendar.jpg"
#define FILE_NAME_TPL "cake.jpg"

using Contour = std::vector<cv::Point>;

int infer_num_of_holiday(const cv::Mat img_input)
{
  auto color_white = cv::Vec3b(255, 255, 255);
  auto is_sunday = [&](cv::Vec3b &pixel) -> bool {
    auto [b, g, r] = std::make_tuple(pixel[0], pixel[1], pixel[2]);
    return (r >= 130 && b <= 50 && g <= 100);
  };

  auto temp_sunday_only = img_input.clone();
  temp_sunday_only.forEach<cv::Vec3b>(
      [&](cv::Vec3b &pixel, const int *position) -> void { pixel = is_sunday(pixel) ? pixel : color_white; });

  auto temp_sunday_only_bin = cv::Mat(img_input.size(), CV_8UC1);
  cv::cvtColor(temp_sunday_only, temp_sunday_only_bin, cv::COLOR_BGR2GRAY);
  cv::threshold(temp_sunday_only_bin, temp_sunday_only_bin, 100, 255, cv::THRESH_BINARY);

  auto contours = std::vector<Contour>();
  {
    auto temp_sunday_only_bin_ = temp_sunday_only_bin.clone();
    cv::findContours(temp_sunday_only_bin_, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
  }

  auto contours_filtered = std::vector<Contour>();
  {
    auto pred = [&](const Contour &contour) -> bool {
      auto rect = cv::boundingRect(contour);
      auto area = rect.area();
      return (7000 <= area && area <= 8000);
    };
    std::copy_if(contours.begin(), contours.end(), std::back_inserter(contours_filtered), pred);
  }

  return static_cast<int>(contours_filtered.size());
}

int main(int argc, const char *argv[])
{
  auto img_input = cv::imread(FILE_NAME_INPUT);
  auto img_tpl = cv::imread(FILE_NAME_TPL);
  if (img_input.empty() || img_tpl.empty())
  {
    fprintf(stderr, "画像の読み込みに失敗しました\n");
    return -1;
  }

  std::cout << "日曜日・祝日は" << infer_num_of_holiday(img_input) << "日間です" << std::endl;

  return 0;
}

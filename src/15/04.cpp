#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "calendar.jpg"
#define FILE_NAME_TPL "cake.jpg"

using Contour = std::vector<cv::Point>;

int main(int argc, const char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT);
  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto is_saturday = [&](cv::Vec3b &pixel) -> bool {
    auto [b, g, r] = std::make_tuple(pixel[0], pixel[1], pixel[2]);
    return (b >= 130 && r <= 50 && g <= 100);
  };

  auto color_white = cv::Vec3b(255, 255, 255);
  auto temp_saturday_only = input.clone();

  temp_saturday_only.forEach<cv::Vec3b>(
      [&](cv::Vec3b &pixel, const int *position) -> void { pixel = is_saturday(pixel) ? pixel : color_white; });

  auto temp_saturday_only_bin = cv::Mat(input.size(), CV_8UC1);
  cv::cvtColor(temp_saturday_only, temp_saturday_only_bin, cv::COLOR_BGR2GRAY);
  cv::threshold(temp_saturday_only_bin, temp_saturday_only_bin, 100, 255, cv::THRESH_BINARY);

  auto contours = std::vector<Contour>();
  {
    auto temp_saturday_only_bin_ = temp_saturday_only_bin.clone();
    cv::findContours(temp_saturday_only_bin_, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
  }

  auto contours_filtered = std::vector<Contour>();
  {
    auto pred = [&](const Contour &contour) -> bool {
      auto rect = cv::boundingRect(contour);
      auto area = rect.area();
      std::cout << area << std::endl;
      return (area <= 2000);
    };
    std::copy_if(contours.begin(), contours.end(), std::back_inserter(contours_filtered), pred);
  }

  auto output = cv::Mat(input.size(), CV_8UC1, cv::Scalar(0));
  cv::drawContours(output, contours_filtered, -1, CV_RGB(255, 255, 255), -1);

  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

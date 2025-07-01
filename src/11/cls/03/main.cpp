#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/fruit_image.jpg"
#define HUE_MAX 180
#define SAT_MAX 255
#define VAL_MAX 255

uchar to_uchar(const int value)
{
  // ref: http://opencv.jp/opencv-2.2/cpp/saturation_arithmetics.html
  return cv::saturate_cast<uchar>(value);
}

int main(int argc, const char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT);
  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto output = input.clone();
  auto black = cv::Vec3b(0, 0, 0);
  auto white = cv::Vec3b(255, 255, 255);
  output.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *position) -> void {
    auto [b, g, r] = std::make_tuple(pixel[0], pixel[1], pixel[2]);
    auto cond = (r >= 100 && (30 <= g && g <= 100) && (50 <= b && b <= 100));

    pixel = cond ? white : black;
  });

  auto output_bin = cv::Mat(input.size(), CV_8UC1);
  cv::cvtColor(output, output_bin, cv::COLOR_BGR2GRAY);
  cv::threshold(output_bin, output_bin, 0, 255, cv::THRESH_BINARY);

  cv::imshow("output_bin", output_bin);
  cv::waitKey();

  return 0;
}

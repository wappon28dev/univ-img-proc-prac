#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/sakura.jpg"
#define DIR_NAME_OUTPUT "./out/"

std::tuple<int, int, int> to_tuple(const cv::Vec3b &color)
{
  return {color[0], color[1], color[2]};
}

uchar to_uchar(const int value)
{
  // ref: http://opencv.jp/opencv-2.2/cpp/saturation_arithmetics.html
  return cv::saturate_cast<uchar>(value);
}

int to_grayscale(const cv::Vec3b &color)
{
  auto [b, g, r] = to_tuple(color);
  return (b * 0.114 + g * 0.587 + r * 0.299);
}

int to_inverted(const int value)
{
  return 255 - value;
}

int main(int argc, const char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto output = cv::Mat(input.size(), CV_8UC1);
  for (int y = 0; y < input.rows; y++)
  {
    for (int x = 0; x < input.cols; x++)
    {
      auto color = input.at<cv::Vec3b>(y, x);
      auto gray = to_grayscale(color);
      auto inverted = to_inverted(gray);

      output.at<uchar>(y, x) = to_uchar(inverted);
    }
  }

  cv::imwrite(DIR_NAME_OUTPUT "inverted.jpg", output);
  cv::imshow("inverted", output);
  cv::waitKey(0);

  return 0;
}

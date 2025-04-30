#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/apple_tree.jpg"
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

std::vector<int> get_lut(int level = 4)
{
  auto lut_size = 256;
  auto color_max = 255;

  if (level <= 1)
  {
    return std::vector<int>(lut_size, 0);
  }

  std::vector<int> lut(lut_size);
  auto step = static_cast<double>(lut_size) / level;

  for (int i = 0; i < lut_size; ++i)
  {
    auto idx = std::min(static_cast<int>(i / step), level - 1);
    auto is_last = (idx == level - 1);

    lut[i] = is_last ? 255 : static_cast<int>(step * idx + step / 2);
  }
  return lut;
}

int main(int argc, const char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  for (auto level : {2, 3, 4})
  {
    auto output = cv::Mat(input.size(), CV_8UC1);

    auto lut = get_lut(level);
    auto to_posterized = [&](const int value) { return lut[value]; };

    for (int y = 0; y < input.rows; y++)
    {
      for (int x = 0; x < input.cols; x++)
      {
        auto color = input.at<cv::Vec3b>(y, x);
        auto gray = to_grayscale(color);
        auto posterized = to_posterized(gray);

        output.at<uchar>(y, x) = to_uchar(posterized);
      }
    }

    cv::imwrite(DIR_NAME_OUTPUT "posterized-" + std::to_string(level) + ".jpg", output);
  }

  return 0;
}

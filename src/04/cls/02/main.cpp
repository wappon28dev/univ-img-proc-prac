#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/kagoshima.jpg"
#define DIR_NAME_OUTPUT "./out/"

auto processor(int v_min, int v_max, int d_min = 0, int d_max = 255)
{
  auto c = static_cast<double>(d_max - d_min) / (v_max - v_min);

  return [=](int f) {
    auto result = c * (f - v_min) + d_min;
    return static_cast<int>(result);
  };
}

uchar to_uchar(const int value)
{
  // ref: http://opencv.jp/opencv-2.2/cpp/saturation_arithmetics.html
  return cv::saturate_cast<uchar>(value);
}

int main(int argc, char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT, cv::IMREAD_GRAYSCALE);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto input_flat = input.reshape(1);
  auto [v_min_iter, v_max_iter] = std::minmax_element(input_flat.begin<uchar>(), input_flat.end<uchar>());
  auto v_min = *v_min_iter;
  auto v_max = *v_max_iter;

  std::cout << "v_min: " << static_cast<int>(v_min) << std::endl;
  std::cout << "v_max: " << static_cast<int>(v_max) << std::endl;

  auto process = processor(v_min, v_max);
  auto output = input.clone();

  output.forEach<uchar>([&](uchar &color, auto _) -> void {
    auto f = static_cast<int>(color);
    auto result = process(f);
    color = to_uchar(result);
  });

  cv::imwrite(DIR_NAME_OUTPUT "kagoshima.jpg", output);

  return 0;
}

#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#define FILE_NAME_INPUT "../assets/apple_grayscale.jpg"

int main(int argc, char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT, cv::IMREAD_GRAYSCALE);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto output = cv::Mat(input.size(), input.type());

  for (auto &&filter_size : {3, 5})
  {
    auto init_val = 1.0 / (filter_size * filter_size);
    auto filter_h = std::vector<double>(filter_size * filter_size, init_val);

    auto kernel = cv::Mat(filter_size, filter_size, CV_32F, filter_h.data());
    cv::normalize(kernel, kernel, 1, 0, cv::NORM_L1);
    cv::filter2D(input, output, -1, kernel);

    auto window_name = std::to_string(filter_size) + "x" + std::to_string(filter_size);
    cv::imshow(window_name, output);
  }

  cv::waitKey();
  cv::destroyAllWindows();

  return 0;
}

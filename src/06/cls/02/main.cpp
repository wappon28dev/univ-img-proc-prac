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

  auto filter_size = 3;
  auto filter_h = std::vector<double>{
      -1., 0., 0., //
      -2., 0., 2., //
      -1., 0., 1., //
  };

  auto kernel = cv::Mat(filter_size, filter_size, CV_64F, filter_h.data());
  // cv::normalize(kernel, kernel, 1, 0, cv::NORM_L1);
  cv::filter2D(input, output, -1, kernel);
  cv::convertScaleAbs(output, output, 1, 0);

  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

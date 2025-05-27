#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#define FILE_NAME_INPUT "./assets/apple_tree.jpg"

cv::Mat to_gray(const cv::Mat &src)
{
  auto dst = cv::Mat(src.size(), CV_8UC1);
  cv::cvtColor(src, dst, cv::COLOR_BGR2GRAY);
  return dst;
}

cv::Mat apply_sobel(const cv::Mat &src)
{
  double filter_h[] = {-1., 0., 1., -2., 0., 2., -1., 0., 1.};
  double filter_v[] = {-1., -2., -1., 0., 0., 0., 1., 2., 1.};

  cv::Mat kernel_h(3, 3, CV_64F, filter_h);
  cv::Mat kernel_v(3, 3, CV_64F, filter_v);

  cv::Mat gx, gy;
  cv::filter2D(src, gx, CV_32F, kernel_h); // 横方向微分
  cv::filter2D(src, gy, CV_32F, kernel_v); // 縦方向微分

  cv::Mat magnitude;
  cv::magnitude(gx, gy, magnitude);

  cv::Mat dst;
  cv::convertScaleAbs(magnitude, dst);

  return dst;
}

int main(int argc, char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto input_gray = to_gray(input);
  auto output_sobel = apply_sobel(input_gray);

  cv::imshow("input", input);
  cv::imshow("output_sobel", output_sobel);
  cv::waitKey();

  return 0;
}

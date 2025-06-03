#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#define FILE_NAME_INPUT_1 "./assets/input1.jpg"
#define FILE_NAME_INPUT_2 "./assets/input2.jpg"
#define ALPHA 0.5

uchar to_uchar(const int value)
{
  // ref: http://opencv.jp/opencv-2.2/cpp/saturation_arithmetics.html
  return cv::saturate_cast<uchar>(value);
}

int main(int argc, char *argv[])
{
  auto input1 = cv::imread(FILE_NAME_INPUT_1, cv::IMREAD_GRAYSCALE);
  auto input2 = cv::imread(FILE_NAME_INPUT_2, cv::IMREAD_GRAYSCALE);

  if (input1.empty() || input2.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto output = cv::Mat(input1.size(), CV_8UC1);

  output.forEach<uchar>([&](uchar &pixel, const int *position) -> void {
    auto y = position[0];
    auto x = position[1];

    // 画素値の取得
    auto s1 = static_cast<double>(input1.at<uchar>(y, x));
    auto s2 = static_cast<double>(input2.at<uchar>(y, x));
    int s_result = ALPHA * s1 + (1.0 - ALPHA) * s2;

    pixel = to_uchar(s_result);
  });

  cv::imshow("input1", input1);
  cv::imshow("input2", input2);
  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

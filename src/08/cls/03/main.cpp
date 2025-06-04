#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/canvas.jpg"
#define THRESHOLD 127

int main(int argc, char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto output = input.clone();

  cv::threshold(input, output, THRESHOLD, 255,
                // THRESH_BINARY: (val > THRESHOLD) ? maxval : 0
                // THRESH_BINARY_INV: (val > THRESHOLD) ? 0 : maxval
                // ref: http://opencv.jp/opencv-2svn/cpp/miscellaneous_image_transformations.html#cv-threshold
                cv::THRESH_BINARY_INV);

  cv::imshow("input", input);
  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

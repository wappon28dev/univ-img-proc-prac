#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/red_rectangle.jpg"

int main(int argc, char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto output = cv::Mat(input.size(), CV_8UC1);
  cv::cvtColor(input, output, cv::COLOR_BGR2GRAY);

  cv::threshold(output, output, 0, 255,
                // 大津 展之 さんの判別分析法
                // https://search.ieice.org/bin/summary.php?id=j63-d_4_349
                cv::THRESH_BINARY | cv::THRESH_OTSU);

  cv::imshow("input", input);
  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

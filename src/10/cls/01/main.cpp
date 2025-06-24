#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "../../hw/01/assets/issue1.jpg"
#define THRESHOLD 100

int main(int argc, const char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT, cv::IMREAD_GRAYSCALE);
  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto input_bin = cv::Mat(input.size(), input.type());
  cv::threshold(input, input_bin, THRESHOLD, 255, cv::THRESH_BINARY);

  auto result = input_bin.clone();
  auto count = 0;

  while (true)
  {
    cv::imshow("input_bin", input_bin);

    auto window_name = "result (c=" + std::to_string(count) + ")";
    cv::imshow(window_name, result);

    int key = cv::waitKey();

    if (key == 'd')
    {
      auto input_dilated = cv::Mat(input_bin.size(), input_bin.type());
      cv::dilate(result, input_dilated, cv::Mat(), cv::Point(-1, -1), 1);
      result = input_dilated;
      count++;
    }
    else if (key == 'e')
    {
      auto input_eroded = cv::Mat(input_bin.size(), input_bin.type());
      cv::erode(result, input_eroded, cv::Mat(), cv::Point(-1, -1), 1);
      result = input_eroded;
      count--;
    }
    else if (key == 'r')
    {
      result = input_bin.clone();
      count = 0;
    }
    else if (key == 'q')
    {
      break;
    }

    cv::destroyWindow(window_name);
  }

  return 0;
}

#include <iostream>
#include <numeric>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/papersample.jpg"

uchar to_uchar(const int value)
{
  // ref: http://opencv.jp/opencv-2.2/cpp/saturation_arithmetics.html
  return cv::saturate_cast<uchar>(value);
}

void print_hist(const std::vector<int> &hist)
{
  for (int i = 0; i < hist.size(); i += 10)
  {
    printf(" %3d | %d", i, hist[i]);
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

void set_hist(std::vector<int> &hist, const cv::Mat &img)
{
  hist = std::vector<int>(256, 0);
  for (int y = 0; y < img.rows; ++y)
  {
    for (int x = 0; x < img.cols; ++x)
    {
      auto pixel = static_cast<int>(img.at<uchar>(y, x));
      hist[pixel]++;
    }
  }
}

int calc_threshold_with_ptile(const std::vector<int> &hist, const double p)
{
  auto total = std::accumulate(hist.begin(), hist.end(), 0LL);
  auto target = static_cast<long long>(total * p);
  auto sum = 0;
  for (int i = 0; i < static_cast<int>(hist.size()); ++i)
  {
    sum += hist[i];
    if (sum >= target)
    {
      return i;
    }
  }

  return -1;
}

int main(int argc, char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT, cv::IMREAD_GRAYSCALE);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  std::vector<int> hist;
  set_hist(hist, input);
  print_hist(hist);

  auto threshold = calc_threshold_with_ptile(hist, 0.1);

  std::cout << "ヒストグラムの 10% にあたる値: " << threshold << std::endl;

  auto output = cv::Mat(input.size(), CV_8UC1);
  cv::threshold(input, output, threshold, 255, cv::THRESH_BINARY_INV);

  cv::imshow("input", input);
  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

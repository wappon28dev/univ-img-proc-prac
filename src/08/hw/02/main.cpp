#include <iostream>
#include <numeric>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/red_green_rectangle.jpg"
#define VAL_HIGH 255
#define VAL_LOW 0

std::tuple<int, int, int> to_tuple(const cv::Vec3b &color)
{
  return {color[0], color[1], color[2]};
}

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
  auto input = cv::imread(FILE_NAME_INPUT);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  std::vector<int> hist_b(256, 0);
  std::vector<int> hist_g(256, 0);
  std::vector<int> hist_r(256, 0);
  input.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, auto __) -> void {
    auto [b, g, r] = to_tuple(pixel);
    hist_b[b]++;
    hist_g[g]++;
    hist_r[r]++;
  });

  std::cout << "--- HISTOGRAM ---" << std::endl;
  std::cout << "B" << std::endl;
  print_hist(hist_b);
  std::cout << "G" << std::endl;
  print_hist(hist_g);
  std::cout << "R" << std::endl;
  print_hist(hist_r);

  auto calc = [](const std::vector<int> &hist) { return calc_threshold_with_ptile(hist, 0.9); };
  auto b_threshold = calc(hist_b);
  auto g_threshold = calc(hist_g);
  auto r_threshold = calc(hist_r);

  std::cout << "ヒストグラムの 90% にあたる値: " << std::endl;
  std::cout << "B: " << b_threshold << ", G: " << g_threshold << ", R: " << r_threshold << std::endl;

  auto output_only_red = cv::Mat(input.size(), CV_8UC1);
  input.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *position) -> void {
    auto [b, g, r] = to_tuple(pixel);
    auto [y, x] = std::make_tuple(position[0], position[1]);
    output_only_red.at<uchar>(y, x) = (r >= r_threshold) ? VAL_HIGH : VAL_LOW;
  });

  auto output_only_green = cv::Mat(input.size(), CV_8UC1);
  input.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *position) -> void {
    auto [b, g, r] = to_tuple(pixel);
    auto [y, x] = std::make_tuple(position[0], position[1]);
    output_only_green.at<uchar>(y, x) = (g >= g_threshold) ? VAL_HIGH : VAL_LOW;
  });

  cv::imshow("Input", input);
  cv::imshow("Output Only Red", output_only_red);
  cv::imshow("Output Only Green", output_only_green);
  cv::waitKey();

  return 0;
}

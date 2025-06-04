#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/red_rectangle.jpg"

void print_hist(const std::vector<int> &hist)
{
  for (int i = 0; i < hist.size(); i += 10)
  {
    printf(" %3d |", i);
    for (int j = 0; j < hist[i]; j += 2000)
    {
      std::cout << "▆";
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;
}

int main(int argc, char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT, cv::IMREAD_GRAYSCALE);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  std::vector<int> hist(256, 0);
  input.forEach<uchar>([&](uchar &pixel, auto _) -> void {
    auto idx = static_cast<int>(pixel);
    hist[idx]++;
  });

  print_hist(hist);

  return 0;
}

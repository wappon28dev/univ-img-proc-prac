#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/red_rectangle.jpg"

void write_csv(const std::string &filename, const std::vector<int> &data)
{
  std::ofstream ofs(filename);
  if (!ofs.is_open())
  {
    fprintf(stderr, "ファイル読み込み失敗\n");
    return;
  }

  for (size_t i = 0; i < data.size(); ++i)
  {
    auto line = std::to_string(i) + "," + std::to_string(data[i]);
    ofs << line << std::endl;
  }
  ofs.close();
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

  for (int idx = 0; idx < hist.size(); idx++)
  {
    printf("%d\t%d\n", idx, hist[idx]);
  }

  write_csv("out/hist.csv", hist);

  return 0;
}

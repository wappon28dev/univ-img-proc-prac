#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/image2.jpg"
#define DIR_NAME_OUTPUT "./out/"
#define THRESHOLD 121

void write_count(const std::string &filename, const int count)
{
  std::ofstream ofs(filename);
  if (!ofs.is_open())
  {
    fprintf(stderr, "ファイル読み込み失敗\n");
    return;
  }

  ofs << "image2=" << count << std::endl;

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

  auto is_foreground = [](int value) { return (value <= THRESHOLD); };
  auto foreground_count = std::count_if(input.begin<uchar>(), input.end<uchar>(), is_foreground);

  write_count(DIR_NAME_OUTPUT "count.txt", foreground_count);

  return 0;
}

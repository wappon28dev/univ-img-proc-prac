#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/aquarium.jpg"
#define DIR_NAME_OUTPUT "./out/"

int main(int argc, const char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto output = input.clone();
  output.forEach<cv::Vec3b>([](cv::Vec3b &color, const int *position) {
    // color:
    //  +---+---+---+
    //  | B | G | R |
    //  +---+---+---+
    //  | 0 | 1 | 2 |
    //  +---+---+---+
    //
    // R→B, B→G, G→R, therefore...
    // color[0] ← color[2];
    // color[1] ← color[0];
    // color[2] ← color[1];
    color = cv::Vec3b(color[2], color[0], color[1]);
  });

  cv::imwrite(DIR_NAME_OUTPUT "swapped.png", output);

  return 0;
}

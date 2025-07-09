#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/bg.jpg"
#define FILE_NAME_TPL "./assets/face.jpg"

int main(int argc, const char *argv[])
{
  auto img_input = cv::imread(FILE_NAME_INPUT);
  auto img_tpl = cv::imread(FILE_NAME_TPL);
  if (img_input.empty() || img_tpl.empty())
  {
    fprintf(stderr, "画像の読み込みに失敗しました\n");
    return -1;
  }

  auto mat_compare = cv::Mat(img_input.size(), CV_32FC1);
  cv::matchTemplate(img_input, img_tpl, mat_compare, cv::TM_SQDIFF_NORMED);

  // double min_val;
  // double max_val;
  // cv::Point min_loc;
  // cv::Point max_loc;
  // cv::minMaxLoc(mat_compare, &min_val, &max_val, &min_loc, &max_loc);

  auto output = img_input.clone();
  mat_compare.forEach<float>([&](auto &s, const int *position) -> void {
    auto [y, x] = std::make_tuple(position[0], position[1]);
    if (s >= 0.1)
    {
      return;
    }

    auto pt1 = cv::Point(x, y);
    auto pt2 = cv::Point(x + img_tpl.cols, y + img_tpl.rows);
    auto color_blue = CV_RGB(255, 0, 0);

    cv::rectangle(output, pt1, pt2, color_blue, 3);

    std::cout << x << ", " << y << std::endl;
  });

  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

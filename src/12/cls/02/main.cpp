#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/bg.jpg"
#define FILE_NAME_TPL "./assets/face.jpg"

template <typename Iter = const float *>
std::pair<cv::Point, cv::Point> minmax_loc(const cv::Mat &mat, Iter min_iter, Iter max_iter)
{
  auto begin = mat.begin<float>();
  auto [min_idx, max_idx] = std::make_pair(min_iter - begin, max_iter - begin);

  auto calc_row = [&](int idx) { return idx / mat.cols; };
  auto calc_col = [&](int idx) { return idx % mat.cols; };
  auto calc_point = [&](int idx) { return cv::Point(calc_col(idx), calc_row(idx)); };

  return std::make_pair(calc_point(min_idx), calc_point(max_idx));
}

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

  auto [min_val_iter, max_val_iter] = std::minmax_element(mat_compare.begin<float>(), mat_compare.end<float>());
  auto [min_val, max_val] = std::make_pair(*min_val_iter, *max_val_iter);
  auto [min_loc, max_loc] = minmax_loc(mat_compare, min_val_iter, max_val_iter);

  std::cout << "min_val: " << min_val << ", max_val: " << max_val << std::endl;
  std::cout << "min_loc: (" << min_loc.x << ", " << min_loc.y << "), "
            << "max_loc: (" << max_loc.x << ", " << max_loc.y << ")" << std::endl;

  auto output = img_input.clone();
  auto pt1 = cv::Point(min_loc.x, min_loc.y);
  auto pt2 = cv::Point(min_loc.x + img_tpl.cols, min_loc.y + img_tpl.rows);
  auto color_red = CV_RGB(255, 0, 0);
  cv::rectangle(output, pt1, pt2, color_red, 3);

  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

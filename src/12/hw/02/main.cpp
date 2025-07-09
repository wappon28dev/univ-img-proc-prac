#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/gazoDora.jpg"
#define FILE_NAME_TPL_RED "./assets/red.jpg"
#define THRESHOLD 0.05

// ref: https://qiita.com/IgnorantCoder/items/3101d6276e9bdddf872c
namespace vec_util
{

template <typename A, typename F> auto map(const A &v, F &&f)
{
  using R = std::vector<decltype(f(*v.begin()))>;
  R y;
  y.reserve(v.size());
  std::transform(std::cbegin(v), std::cend(v), std::back_inserter(y), f);
  return y;
}

template <typename A, typename F> auto filter(const A &v, F &&pred)
{
  A y;
  std::copy_if(std::cbegin(v), std::cend(v), std::back_inserter(y), pred);
  return y;
}

template <typename A, typename F> void for_each(const A &v, F &&f)
{
  std::for_each(std::cbegin(v), std::cend(v), f);
}
} // namespace vec_util

struct Template
{
  std::string name;
  cv::Mat img;
  cv::Scalar color;
};

struct MatchResult
{
  Template tpl;
  cv::Mat mat_compare;

  static MatchResult calc(const Template &tpl, const cv::Mat &img_input)
  {
    auto mat_compare = cv::Mat(img_input.size(), CV_32FC1);
    cv::matchTemplate(img_input, tpl.img, mat_compare, cv::TM_SQDIFF_NORMED);

    // auto mat_compare_filtered = vec_util::filter(mat_compare, [](const float &s) { return s < THRESHOLD; });

    return MatchResult{tpl, mat_compare};
  }

  std::pair<cv::Point, cv::Point> calc_pt(const cv::Point &pt) const
  {
    auto pt1 = cv::Point(pt.x, pt.y);
    auto pt2 = cv::Point(pt.x + tpl.img.cols, pt.y + tpl.img.rows);
    return std::make_pair(pt1, pt2);
  }

  void print() const
  {
    mat_compare.forEach<float>([&](const float &s, const int *position) -> void {
      auto [y, x] = std::make_tuple(position[0], position[1]);
      auto [pt1, pt2] = calc_pt(cv::Point(x, y));

      if (s >= THRESHOLD)
      {
        return;
      }

      std::cout << "Template: " << tpl.name << ", Position: (" << x << ", " << y << "), "
                << "Rectangle: (" << pt1.x << ", " << pt1.y << ") to (" << pt2.x << ", " << pt2.y << ")" << std::endl;
    });
  }

  void draw(cv::Mat &output) const
  {
    mat_compare.forEach<float>([&](const float &s, const int *position) -> void {
      auto [y, x] = std::make_tuple(position[0], position[1]);
      auto [pt1, pt2] = calc_pt(cv::Point(x, y));

      if (s >= THRESHOLD)
      {
        return;
      }

      cv::rectangle(output, pt1, pt2, tpl.color, 3);
    });
  }
};

int main(int argc, const char *argv[])
{
  auto img_input = cv::imread(FILE_NAME_INPUT, cv::IMREAD_GRAYSCALE);
  auto img_tpl_red = cv::imread(FILE_NAME_TPL_RED, cv::IMREAD_GRAYSCALE);
  if (img_input.empty() || img_tpl_red.empty())
  {
    fprintf(stderr, "画像の読み込みに失敗しました\n");
    return -1;
  }

  auto tpl_red = Template{"red", img_tpl_red, CV_RGB(255, 0, 0)};
  auto match_results = MatchResult::calc(tpl_red, img_input);
  match_results.print();

  // 類似度マップの描画
  auto output = match_results.mat_compare.clone();
  cv::normalize(match_results.mat_compare, output, 0, 255, cv::NORM_MINMAX, CV_8UC1);

  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

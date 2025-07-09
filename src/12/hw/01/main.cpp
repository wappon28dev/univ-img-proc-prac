#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/gazoDora.jpg"
#define FILE_NAME_TPL_GREEN "./assets/green.jpg"
#define FILE_NAME_TPL_RED "./assets/red.jpg"
#define FILE_NAME_TPL_STAR "./assets/star.jpg"
#define FILE_NAME_TPL_YELLOW "./assets/yellow.jpg"
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
  auto img_input = cv::imread(FILE_NAME_INPUT);
  auto img_tpl_green = cv::imread(FILE_NAME_TPL_GREEN);
  auto img_tpl_red = cv::imread(FILE_NAME_TPL_RED);
  auto img_tpl_star = cv::imread(FILE_NAME_TPL_STAR);
  auto img_tpl_yellow = cv::imread(FILE_NAME_TPL_YELLOW);
  if (img_input.empty() || img_tpl_green.empty() || img_tpl_red.empty() || img_tpl_star.empty() ||
      img_tpl_yellow.empty())
  {
    fprintf(stderr, "画像の読み込みに失敗しました\n");
    return -1;
  }

  auto tpl_list = std::vector<Template>{//
                                        {"green", img_tpl_green, CV_RGB(0, 128, 0)},
                                        {"red", img_tpl_red, CV_RGB(255, 0, 0)},
                                        {"star", img_tpl_star, CV_RGB(255, 0, 255)},
                                        {"yellow", img_tpl_yellow, CV_RGB(255, 255, 0)}};

  auto mapper = [&](const Template &tpl) { return MatchResult::calc(tpl, img_input); };
  auto match_results = vec_util::map(tpl_list, mapper);

  auto output = img_input.clone();
  vec_util::for_each(match_results, [&](const MatchResult &result) {
    result.print();
    result.draw(output);
  });

  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

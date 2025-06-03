#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#define FILE_NAME_INPUT_1 "./assets/issue_input1.jpg"
#define FILE_NAME_INPUT_2 "./assets/issue_input2.jpg"
#define THRESHOLD 30

uchar to_uchar(const int value)
{
  // ref: http://opencv.jp/opencv-2.2/cpp/saturation_arithmetics.html
  return cv::saturate_cast<uchar>(value);
}

int count_diff(const cv::Mat &img1, const cv::Mat &img2)
{
  int count = 0;
  for (int y = 0; y < img1.rows; ++y)
  {
    for (int x = 0; x < img1.cols; ++x)
    {
      auto a = static_cast<int>(img1.at<uchar>(y, x));
      auto b = static_cast<int>(img2.at<uchar>(y, x));

      if (std::abs(a - b) >= THRESHOLD)
      {
        count++;
      }
    }
  }
  return count;
}

cv::Size infer_opt_filter_size(const cv::Mat &img_input, const cv::Mat &img_processed)
{
  /*
    適用順:
      フィルターサイズ (`int`)
        -> フィルターサイズ (`cv::Size`)
        -> ぼかし適用の画像 (`cv::Mat`)
        -> 2 画像の差分カウント (`int`)

    +---+    +----------------+
    | 3 | -> | cv::Size(3, 3) | -> 差分カウント
    | 5 | -> | cv::Size(5, 5) | -> 差分カウント
    | 7 | -> | cv::Size(7, 7) | -> 差分カウント
    | 9 | -> | cv::Size(9, 9) | -> 差分カウント
    +---+    +----------------+

    もっとも差分が少ない差分カウントを持つフィルターサイズを選ぶ
   */

  auto filter_sizes = std::vector<int>{3, 5, 7, 9};
  std::vector<int> diff_counts;
  std::transform(filter_sizes.begin(), filter_sizes.end(), std::back_inserter(diff_counts), [&](const int &size) {
    cv::Mat blur_img;
    cv::GaussianBlur(img_input, blur_img, cv::Size(size, size), 0);
    auto diff_count = count_diff(blur_img, img_processed);
    std::cout << "フィルターサイズ: " << size << "x" << size << " | 差分カウント: " << diff_count << std::endl;

    return diff_count;
  });

  // ref: https://qiita.com/dsy_i07/items/9f8184bc46684d0eaca0#%E7%B5%90%E8%AB%96
  auto min_iter = std::min_element(diff_counts.begin(), diff_counts.end());
  int best_index = std::distance(diff_counts.begin(), min_iter);
  int best_size = filter_sizes[best_index];

  return cv::Size(best_size, best_size);
}

int main(int argc, char *argv[])
{
  auto img_processed = cv::imread(FILE_NAME_INPUT_1, cv::IMREAD_GRAYSCALE);
  auto img_input = cv::imread(FILE_NAME_INPUT_2, cv::IMREAD_GRAYSCALE);

  if (img_processed.empty() || img_input.empty())
  {
    fprintf(stderr, "画像読み込み失敗\n");
    return -1;
  }

  // 1. 最も差分の少ないフィルターサイズを推定
  auto filter_size = infer_opt_filter_size(img_input, img_processed);

  // 2. 推定したフィルターサイズで画像をぼかす
  cv::Mat blur_img_input;
  cv::Mat blur_img_processed;
  cv::GaussianBlur(img_input, blur_img_input, filter_size, 0);
  cv::GaussianBlur(img_processed, blur_img_processed, filter_size, 0);

  auto output = cv::Mat(img_input.size(), CV_8UC3);

  for (int y = 0; y < img_input.rows; ++y)
  {
    for (int x = 0; x < img_input.cols; ++x)
    {
      auto a = static_cast<int>(blur_img_input.at<uchar>(y, x));
      auto b = static_cast<int>(blur_img_processed.at<uchar>(y, x));

      auto diff = std::abs(a - b);

      auto black = cv::Vec3b(0, 0, 0);
      auto red = cv::Vec3b(0, 0, 255);

      // 3. 差分が閾値以上なら赤, そうでなければ黒
      output.at<cv::Vec3b>(y, x) = (diff >= THRESHOLD) ? red : black;
    }
  }

  cv::imshow("input", img_input);
  cv::imshow("processed", img_processed);
  cv::imshow("output", output);
  cv::waitKey();

  return 0;
}

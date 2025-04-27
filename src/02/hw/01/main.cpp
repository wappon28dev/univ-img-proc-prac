#include <iostream>
#include <opencv2/opencv.hpp>

#define FILE_NAME_INPUT "./assets/dog.png"
#define DIR_NAME_OUTPUT "./out/"

#define SAMPLING_SIZE_PX 10
#define COLOR_LEVEL 8

cv::Mat sample(const cv::Mat &input)
{
  auto cell_size = SAMPLING_SIZE_PX;
  auto output = input.clone();

  for (int y = 0; y < input.rows; y += cell_size)
  {
    for (int x = 0; x < input.cols; x += cell_size)
    {
      // NOTE: `cell_size` で増分し続けると `cv::Rect` が画像の端を超えるので頭打ちにする.
      auto w = std::min(cell_size, input.cols - x);
      auto h = std::min(cell_size, input.rows - y);

      auto rect = cv::Rect(x, y, w, h);
      auto borderColor = cv::Vec3b(0, 0, 0);
      cv::rectangle(output, rect, borderColor, 1);
    }
  }

  return output;
}

/// @brief 色を量子化する.
/// @note:
///   `color` は `cv::Vec3b` (= `uint8_t[3]`) 型.
///   カラー画像 (RGB) で #FF6600 であれば, BGR であることに注意すると,
///   ```c++
///   uint8_t color[3] = { 0x00, 0x66, 0xFF };
///   ```
///   と表すことができる.
/// @see: https://docs.opencv.org/2.4/modules/core/doc/basic_structures.html#vec
cv::Vec3b quantize_color(const cv::Vec3b &color)
{
  auto calc = [](u_int8_t value) {
    uint8_t level = value * (COLOR_LEVEL - 1) / 255;
    return level * 255 / (COLOR_LEVEL - 1);
  };

  auto b = calc(color[0]);
  auto g = calc(color[1]);
  auto r = calc(color[2]);

  return cv::Vec3b(b, g, r);
}

// ref: https://minus9d.hatenablog.com/entry/20130126/1359194404
cv::Mat quantize(const cv::Mat &input)
{
  auto cell_size = SAMPLING_SIZE_PX;
  auto output = cv::Mat(input.size(), input.type());

  for (int y = 0; y < input.rows; y += cell_size)
  {
    for (int x = 0; x < input.cols; x += cell_size)
    {
      // NOTE: `cell_size` で増分し続けると `cv::Rect` が画像の端を超えるので頭打ちにする.
      auto w = std::min(cell_size, input.cols - x);
      auto h = std::min(cell_size, input.rows - y);

      auto rect = cv::Rect(x, y, w, h);
      auto cell = input(rect);

      auto color_mean = cv::mean(cell);
      auto color = cv::Vec3b(color_mean[0], color_mean[1], color_mean[2]);
      auto color_quantized = quantize_color(color);

      cv::rectangle(output, rect, color_quantized, -1);
    }
  }

  return output;
}

int main(int argc, const char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto sampled = sample(input);
  auto quantized = quantize(input);

  cv::imwrite(DIR_NAME_OUTPUT "sampled.png", sampled);
  cv::imwrite(DIR_NAME_OUTPUT "quantized.png", quantized);

  return 0;
}

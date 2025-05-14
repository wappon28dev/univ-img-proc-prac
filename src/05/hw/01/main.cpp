#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#define FILE_NAME_INPUT "./assets/apple_grayscale.jpg"
#define DIR_NAME_OUTPUT "./out/"

int main(int argc, char *argv[])
{
  auto input = cv::imread(FILE_NAME_INPUT, cv::IMREAD_GRAYSCALE);

  if (input.empty())
  {
    fprintf(stderr, "読み込み失敗\n");
    return -1;
  }

  auto line = std::string{};
  std::cout << "平均化フィルターの回数 (カンマ区切り): ";
  std::cin >> line;

  auto count_list = std::vector<int>{};
  std::stringstream ss(line);
  while (ss.good())
  {
    std::string substr;
    std::getline(ss, substr, ',');
    count_list.push_back(std::stoi(substr));
  }

  for (auto &&loop_count : count_list)
  {
    auto filter_size = 3;
    auto init_val = 1.0 / (filter_size * filter_size);
    auto filter_h = std::vector<double>(filter_size * filter_size, init_val);

    auto kernel = cv::Mat(filter_size, filter_size, CV_32F, filter_h.data());
    cv::normalize(kernel, kernel, 1, 0, cv::NORM_L1);

    auto output = input.clone();
    for (int i = 0; i < loop_count; i++)
    {
      cv::filter2D(output, output, -1, kernel);
    }

    auto window_name = "ave_x" + std::to_string(loop_count);
    cv::imshow(window_name, output);
  }

  cv::waitKey();
  cv::destroyAllWindows();

  return 0;
}

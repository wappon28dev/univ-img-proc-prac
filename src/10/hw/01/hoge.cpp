#include <iostream>
#include <vector>

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

template <typename A, typename F> auto filter(const A &v, F &&f)
{
  A y;
  std::copy_if(std::cbegin(v), std::cend(v), std::back_inserter(y), f);
  return y;
}

template <typename A, typename F> void for_each(const A &v, F &&f)
{
  std::for_each(std::cbegin(v), std::cend(v), f);
}
} // namespace vec_util

int main(int argc, char *argv[], char *envp[])
{
  auto vec = std::vector<int>{1, 2, 3, 4, 5};

  auto mapped = vec_util::map(vec, [](auto x) { return x * 2; });
  auto filtered = vec_util::filter(mapped, [](auto x) { return x > 5; });

  vec_util::for_each(filtered, [](auto x) { std::cout << x << " "; });
  std::cout << std::endl;

  return 0;
}

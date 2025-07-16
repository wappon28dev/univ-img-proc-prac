#include <vector>
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

int main()
{
  auto arr = std::vector<int>{1, 2, 3, 4, 5};
  auto arr2x = vec_util::map(arr, [](int x) { return x * 2; });
  auto arr_odd = vec_util::filter(arr, [](int x) { return x % 2 != 0; });
}

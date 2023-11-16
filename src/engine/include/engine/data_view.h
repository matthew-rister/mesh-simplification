#ifndef SRC_ENGINE_INCLUDE_ENGINE_DATA_VIEW_H_
#define SRC_ENGINE_INCLUDE_ENGINE_DATA_VIEW_H_

#include <ranges>

namespace gfx {

template <typename R, typename T>
concept DataRange =
    std::ranges::contiguous_range<R> && std::same_as<std::ranges::range_value_t<R>, std::remove_cvref_t<T>>;

template <typename T>
class DataView {
public:
  constexpr DataView(T& data) noexcept : data_{&data}, size_{1} {}
  constexpr DataView(T* data, const std::size_t size) noexcept : data_{data}, size_{size} {}
  constexpr DataView(DataRange<T> auto&& range) noexcept : data_{range.data()}, size_{range.size()} {}

  [[nodiscard]] constexpr T* data() const noexcept { return data_; }
  [[nodiscard]] constexpr std::size_t size() const noexcept { return size_; }
  [[nodiscard]] constexpr std::size_t size_bytes() const noexcept { return sizeof(T) * size_; }

private:
  T* data_;
  std::size_t size_;
};

}  // namespace gfx

#endif  // SRC_ENGINE_INCLUDE_ENGINE_DATA_VIEW_H_

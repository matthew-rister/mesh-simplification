#ifndef SRC_GRAPHICS_DATA_VIEW_H_
#define SRC_GRAPHICS_DATA_VIEW_H_

#include <ranges>

namespace gfx {

/** \brief A sized range of contiguous data. */
template <typename R, typename T>
concept DataRange = std::ranges::contiguous_range<R> && std::ranges::sized_range<R>
                    && std::same_as<std::ranges::range_value_t<R>, std::remove_cvref_t<T>>;

/**
 * \brief A view of a single item or a contiguous range of homogeneous data.
 * \tparam T The data view type. If this represents a contiguous range, it is the data type for each item in the range.
 */
template <typename T>
class DataView {
public:
  /**
   * \brief Initializes a data view from a single item.
   * \param data The single item to create a view of.
   */
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr DataView(T& data) noexcept : data_{&data}, size_{1} {}

  /**
   * \brief Initializes a data view from a pointer and size.
   * \param data A pointer to the data to view.
   * \param size The number of items in the data view.
   */
  constexpr DataView(T* data, const std::size_t size) noexcept : data_{data}, size_{size} {}

  /**
   * \brief Initializes a data view from a range of data.
   * \param range The range of data to create a view of.
   */
  // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
  constexpr DataView(DataRange<T> auto&& range) noexcept : data_{range.data()}, size_{range.size()} {}

  /** \brief Gets a pointer to the underlying data. */
  [[nodiscard]] constexpr T* data() const noexcept { return data_; }

  /** \brief Gets the number of items in the data view. */
  [[nodiscard]] constexpr std::size_t size() const noexcept { return size_; }

  /** \brief Gets the total number of bytes the underlying data occupies. */
  [[nodiscard]] constexpr std::size_t size_bytes() const noexcept { return size_ * sizeof(T); }

private:
  T* data_;
  std::size_t size_;
};

}  // namespace gfx

#endif  // SRC_GRAPHICS_DATA_VIEW_H_

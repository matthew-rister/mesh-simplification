#include "graphics/data_view.h"

#include <array>
#include <cstdint>

#include <gtest/gtest.h>

namespace {

using DataType = std::uint32_t;
constexpr std::array<DataType, 3> kData{};

TEST(DataViewTest, SingleValueInitializationHasTheCorrectData) {
  constexpr gfx::DataView kDataView{kData[0]};
  static_assert(kDataView.data() == kData.data());
}

TEST(DataViewTest, SingleValueInitializationHasTheCorrectSize) {
  constexpr gfx::DataView kDataView{kData[0]};
  static_assert(kDataView.size() == 1);
}

TEST(DataViewTest, SingleValueInitializationHasTheCorrectSizeInBytes) {
  constexpr gfx::DataView kDataView{kData[0]};
  static_assert(kDataView.size_bytes() == sizeof(DataType));
}

TEST(DataViewTest, PointerInitializationHasTheCorrectData) {
  constexpr auto* kDataPtr = kData.data();
  constexpr auto kDataSize = 2;
  constexpr gfx::DataView kDataView{kDataPtr, kDataSize};
  static_assert(kDataView.data() == kDataPtr);
}

TEST(DataViewTest, PointerInitializationHasTheCorrectSize) {
  constexpr auto* kDataPtr = kData.data();
  constexpr auto kDataViewSize = 2;
  constexpr gfx::DataView kDataView{kDataPtr, kDataViewSize};
  static_assert(kDataView.size() == kDataViewSize);
}

TEST(DataViewTest, PointerInitializationHasTheCorrectSizeinBytes) {
  constexpr auto* kDataPtr = kData.data();
  constexpr auto kDataViewSize = 2;
  constexpr gfx::DataView kDataView{kDataPtr, kDataViewSize};
  static_assert(kDataView.size_bytes() == sizeof(DataType) * kDataViewSize);
}

TEST(DataViewTest, DataRangeInitializationHasTheCorrectData) {
  constexpr gfx::DataView<const DataType> kDataView{kData};
  static_assert(kDataView.data() == kData.data());
}

TEST(DataViewTest, DataRangeInitializationHasTheCorrectSize) {
  constexpr gfx::DataView<const DataType> kDataView{kData};
  static_assert(kDataView.size() == kData.size());
}

TEST(DataViewTest, DataRangeInitializationHasTheCorrectSizeInBytes) {
  constexpr gfx::DataView<const DataType> kDataView{kData};
  static_assert(kDataView.size_bytes() == sizeof(DataType) * kData.size());
}

}  // namespace

#include "engine/data_view.h"

#include <array>
#include <string_view>

#include <gtest/gtest.h>

class DataViewTest : public ::testing::Test {
protected:
  struct Quote {
    std::string_view quote;
    std::string_view author;
  };

  static constexpr std::array kQuotes{
      Quote{.quote = "What lies behind us and what lies before us are tiny matters compared to what lies within us",
            .author = "Ralph Waldo Emerson"},
      Quote{.quote = "Simplicity is prerequisite for reliability", .author = "Edsger Dijkstra"},
      Quote{.quote = "The true sign of intelligence is not knowledge but imagination", .author = "Albert Einstein"}};
};

TEST_F(DataViewTest, SingleValueInitializationHasTheCorrectData) {
  constexpr auto& kData = kQuotes[0];
  constexpr gfx::DataView kDataView{kData};
  static_assert(kDataView.data() == &kData);
}

TEST_F(DataViewTest, SingleValueInitializationHasTheCorrectSize) {
  constexpr auto& kData = kQuotes[0];
  constexpr gfx::DataView kDataView{kData};
  static_assert(kDataView.size() == 1);
}

TEST_F(DataViewTest, SingleValueInitializationHasTheCorrectSizeInBytes) {
  constexpr auto& kData = kQuotes[0];
  constexpr gfx::DataView kDataView{kData};
  static_assert(kDataView.size_bytes() == sizeof(Quote));
}

TEST_F(DataViewTest, PointerInitializationHasTheCorrectData) {
  constexpr auto* kData = kQuotes.data();
  constexpr auto kDataSize = 2;
  constexpr gfx::DataView kDataView{kData, kDataSize};
  static_assert(kDataView.data() == kData);
}

TEST_F(DataViewTest, PointerInitializationHasTheCorrectSize) {
  constexpr auto* kData = kQuotes.data();
  constexpr auto kDataSize = 2;
  constexpr gfx::DataView kDataView{kData, kDataSize};
  static_assert(kDataView.size() == kDataSize);
}

TEST_F(DataViewTest, PointerInitializationHasTheCorrectSizeinBytes) {
  constexpr auto* kData = kQuotes.data();
  constexpr auto kDataSize = 2;
  constexpr gfx::DataView kDataView{kData, kDataSize};
  static_assert(kDataView.size_bytes() == sizeof(Quote) * kDataSize);
}

TEST_F(DataViewTest, DataRangeInitializationHasTheCorrectData) {
  constexpr gfx::DataView<const Quote> kDataView{kQuotes};
  static_assert(kDataView.data() == kQuotes.data());
}

TEST_F(DataViewTest, DataRangeInitializationHasTheCorrectSize) {
  constexpr gfx::DataView<const Quote> kDataView{kQuotes};
  static_assert(kDataView.size() == kQuotes.size());
}

TEST_F(DataViewTest, DataRangeInitializationHasTheCorrectSizeInBytes) {
  constexpr gfx::DataView<const Quote> kDataView{kQuotes};
  static_assert(kDataView.size_bytes() == sizeof(Quote) * kQuotes.size());
}

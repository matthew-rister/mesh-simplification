#include "graphics/arcball.cpp"  // NOLINT(build/include)

#include <limits>

#include <gtest/gtest.h>

#include "graphics/window.h"

namespace {

constexpr auto kWindowWidth = 1920;
constexpr auto kWindowHeight = 1080;
constexpr gfx::Window::Size kWindowSize{.width = kWindowWidth, .height = kWindowHeight};

TEST(ArcballTest, GetNormalizedViewPositionConvertsScreenCoordiantesToNormalizedXyViewCoordiantes) {
  static constexpr glm::vec2 kTopLeftCorner{0.0f, 0.0f};
  static_assert(GetNormalizedViewPosition(kTopLeftCorner, kWindowSize) == glm::vec2{-1.0f, 1.0f});

  static constexpr glm::vec2 kBottomLeftCorner{0.0f, kWindowHeight};
  static_assert(GetNormalizedViewPosition(kBottomLeftCorner, kWindowSize) == glm::vec2{-1.0f, -1.0f});

  static constexpr glm::vec2 kBottomRightCorner{kWindowWidth, kWindowHeight};
  static_assert(GetNormalizedViewPosition(kBottomRightCorner, kWindowSize) == glm::vec2{1.0f, -1.0f});

  static constexpr glm::vec2 kTopRightCorner{kWindowWidth, 0.0f};
  static_assert(GetNormalizedViewPosition(kTopRightCorner, kWindowSize) == glm::vec2{1.0f, 1.0f});

  static constexpr glm::vec2 kCenter{kWindowWidth / 2.0f, kWindowHeight / 2.0f};
  static_assert(GetNormalizedViewPosition(kCenter, kWindowSize) == glm::vec2{0.0f, 0.0f});
}

TEST(ArcballTest, GetNormalizedViewPositionClampsInTheRangeMinusOneToOne) {
  static constexpr auto kHalfFloatMin = 0.5f * std::numeric_limits<float>::min();
  static constexpr auto kHalfFloatMax = 0.5f * std::numeric_limits<float>::max();

  static constexpr glm::vec2 kTopLeftCorner{kHalfFloatMin, kHalfFloatMin};
  static_assert(GetNormalizedViewPosition(kTopLeftCorner, kWindowSize) == glm::vec2{-1.0f, 1.0f});

  static constexpr glm::vec2 kBottomLeftCorner{kHalfFloatMin, kHalfFloatMax};
  static_assert(GetNormalizedViewPosition(kBottomLeftCorner, kWindowSize) == glm::vec2{-1.0f, -1.0f});

  static constexpr glm::vec2 kBottomRightCorner{kHalfFloatMax, kHalfFloatMax};
  static_assert(GetNormalizedViewPosition(kBottomRightCorner, kWindowSize) == glm::vec2{1.0f, -1.0f});

  static constexpr glm::vec2 kTopRightCorner{kHalfFloatMax, kHalfFloatMin};
  static_assert(GetNormalizedViewPosition(kTopRightCorner, kWindowSize) == glm::vec2{1.0f, 1.0f});
}

TEST(ArcballTest, GetArcballPositionForViewPositionInsideTheUnitSphere) {
  static constexpr glm::vec2 kViewPosition{0.5f, 0.25f};
  const auto arcball_position = GetArcballPosition(kViewPosition);
  const auto x = arcball_position.x;
  const auto y = arcball_position.y;
  const auto z = arcball_position.z;
  EXPECT_FLOAT_EQ(x, kViewPosition.x);
  EXPECT_FLOAT_EQ(y, kViewPosition.y);
  EXPECT_FLOAT_EQ(z, std::sqrt(1.0f - x * x - y * y));
}

TEST(ArcballTest, GetArcballPositionForViewPositionOutsideTheUnitSphere) {
  constexpr glm::vec2 kViewPosition{0.75f, 0.85f};
  const auto normalized_view_position = glm::normalize(kViewPosition);
  const auto arcball_position = GetArcballPosition(kViewPosition);
  EXPECT_FLOAT_EQ(arcball_position.x, normalized_view_position.x);
  EXPECT_FLOAT_EQ(arcball_position.y, normalized_view_position.y);
  EXPECT_FLOAT_EQ(arcball_position.z, 0.0f);
}

}  // namespace

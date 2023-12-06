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

}  // namespace

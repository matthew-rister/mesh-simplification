#include "graphics/obj_loader.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <cstdint>
#include <format>
#include <fstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include "graphics/mesh.h"

namespace {

constexpr auto kInvalidIndex = -1;

constexpr std::string_view Trim(std::string_view line, const std::string_view delimiter = " ") {
  line.remove_prefix(std::min(line.find_first_not_of(delimiter), line.size()));
  line.remove_suffix(line.size() - line.find_last_not_of(delimiter) - 1);
  return line;
}

constexpr std::vector<std::string_view> Split(const std::string_view line, const std::string_view delimiter = " ") {
  std::vector<std::string_view> tokens;
  for (auto i = line.find_first_not_of(delimiter); i < line.size();) {
    const auto j = std::min(line.find_first_of(delimiter, i), line.size());
    tokens.push_back(line.substr(i, j - i));
    i = line.find_first_not_of(delimiter, j);
  }
  return tokens;
}

template <typename T>
constexpr T ParseToken(const std::string_view token) {
  T value;
  if (const auto [_, error_code] = std::from_chars(token.data(), token.data() + token.size(), value);
      error_code == std::errc{}) {
    return value;
  }
  throw std::invalid_argument{std::format("Unable to convert {} to type {}", token, typeid(T).name())};
}

template <typename T, glm::length_t N>
constexpr glm::vec<N, T> ParseLine(const std::string_view line) {
  if (const auto tokens = Split(line); tokens.size() == N + 1) {
    glm::vec<N, T> vec{};
    for (glm::length_t i = 0; i < N; ++i) {
      const auto j = static_cast<std::size_t>(i) + 1;
      vec[i] = ParseToken<T>(tokens[j]);
    }
    return vec;
  }
  throw std::invalid_argument{std::format("Unsupported format {}", line)};
}

glm::ivec3 ParseIndexGroup(const std::string_view token) {
  static constexpr auto kDelimiter = "/";
  const auto delimiter_count = std::ranges::count(token, *kDelimiter);

  switch (const auto tokens = Split(token, kDelimiter); tokens.size()) {
    case 1: {
      if (delimiter_count == 0) {
        const auto v = ParseToken<int>(tokens[0]);
        return glm::ivec3{v - 1, kInvalidIndex, kInvalidIndex};
      }
      break;
    }
    case 2: {
      if (delimiter_count == 1) {
        const auto v = ParseToken<int>(tokens[0]);
        const auto vt = ParseToken<int>(tokens[1]);
        return glm::ivec3{v - 1, vt - 1, kInvalidIndex};
      }
      if (delimiter_count == 2 && !token.starts_with(kDelimiter) && !token.ends_with(kDelimiter)) {
        const auto v = ParseToken<int>(tokens[0]);
        const auto vn = ParseToken<int>(tokens[1]);
        return glm::ivec3{v - 1, kInvalidIndex, vn - 1};
      }
      break;
    }
    case 3: {
      if (delimiter_count == 2) {
        const auto v = ParseToken<int>(tokens[0]);
        const auto vt = ParseToken<int>(tokens[1]);
        const auto vn = ParseToken<int>(tokens[2]);
        return glm::ivec3{v - 1, vt - 1, vn - 1};
      }
      break;
    }
  }

  throw std::invalid_argument{std::format("Unsupported format {}", token)};
}

std::array<glm::ivec3, 3> ParseFace(const std::string_view line) {
  if (const auto tokens = Split(line); tokens.size() == 4) {
    return std::array{ParseIndexGroup(tokens[1]), ParseIndexGroup(tokens[2]), ParseIndexGroup(tokens[3])};
  }
  throw std::invalid_argument{std::format("Unsupported format {}", line)};
}

template <typename T, glm::length_t N>
constexpr glm::vec<N, T> TryGet(const std::vector<glm::vec<N, T>>& data, const int index) {
  if (index != kInvalidIndex) {
    // use bounds checking here in case index position in .obj file is invalid
    return data.at(static_cast<std::size_t>(index));
  }
  return glm::vec<N, T>{};
}

gfx::Mesh LoadMesh(const gfx::Device& device, std::istream& is) {
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texture_coordinates;
  std::vector<std::array<glm::ivec3, 3>> faces;

  for (std::string line; getline(is, line);) {
    if (const auto line_view = Trim(line); !line_view.empty() && !line_view.starts_with('#')) {
      if (line_view.starts_with("v ")) {
        positions.push_back(ParseLine<float, 3>(line_view));
      } else if (line_view.starts_with("vt ")) {
        texture_coordinates.push_back(ParseLine<float, 2>(line_view));
      } else if (line_view.starts_with("vn ")) {
        normals.push_back(ParseLine<float, 3>(line_view));
      } else if (line_view.starts_with("f ")) {
        faces.push_back(ParseFace(line_view));
      }
    }
  }

  std::vector<gfx::Mesh::Vertex> vertices;
  vertices.reserve(positions.size());

  std::vector<std::uint32_t> indices;
  indices.reserve(faces.size() * 3);

  for (std::unordered_map<glm::ivec3, std::uint32_t> index_groups; const auto& face : faces) {
    for (const auto& index_group : face) {
      auto iterator = index_groups.find(index_group);
      if (iterator == index_groups.cend()) {
        vertices.push_back(gfx::Mesh::Vertex{.position = TryGet(positions, index_group[0]),
                                             .texture_coordinates = TryGet(texture_coordinates, index_group[1]),
                                             .normal = TryGet(normals, index_group[2])});
        iterator = index_groups.emplace(index_group, static_cast<std::uint32_t>(vertices.size()) - 1).first;
      }
      indices.push_back(iterator->second);
    }
  }

  return gfx::Mesh{device, std::move(vertices), std::move(indices)};
}

}  // namespace

gfx::Mesh gfx::obj_loader::LoadMesh(const Device& device, const std::filesystem::path& filepath) {
  if (std::ifstream ifs{filepath}) {
    return ::LoadMesh(device, ifs);
  }
  throw std::runtime_error{std::format("Unable to open {}", filepath.string())};
}

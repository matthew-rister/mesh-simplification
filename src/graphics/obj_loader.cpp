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

/** \brief Indicates an unspecified index position in the .obj file */
constexpr auto kInvalidIndex = -1;

/**
 * \brief Removes a set of characters from the beginning and end of the string.
 * \param line The string to evaluate.
 * \param delimiter The set of characters to remove from the beginning and end of the string.
 * \return A view of the characters in \p delimiter removed from the beginning and end of \p line.
 */
constexpr std::string_view Trim(std::string_view line, const std::string_view delimiter = " ") {
  line.remove_prefix(std::min(line.find_first_not_of(delimiter), line.size()));
  line.remove_suffix(line.size() - line.find_last_not_of(delimiter) - 1);
  return line;
}

/**
 * \brief Gets tokens delimited by a set of characters.
 * \param line The string to evaluate.
 * \param delimiter The set of characters to split the string on.
 * \return A vector of tokens in \p line split on the characters in \p delimiter.
 */
constexpr std::vector<std::string_view> Split(const std::string_view line, const std::string_view delimiter = " ") {
  std::vector<std::string_view> tokens;
  for (auto i = line.find_first_not_of(delimiter); i < line.size();) {
    const auto j = std::min(line.find_first_of(delimiter, i), line.size());
    tokens.push_back(line.substr(i, j - i));
    i = line.find_first_not_of(delimiter, j);
  }
  return tokens;
}

/**
 * \brief Parses a string token.
 * \tparam T The type to convert to.
 * \param token The token to parse.
 * \return The value of \p token converted to type \p T.
 */
template <typename T>
constexpr T ParseToken(const std::string_view token) {
  T value;
  if (std::from_chars(token.data(), token.data() + token.size(), value).ec != std::errc{}) {
    throw std::invalid_argument{std::format("Unable to convert {} to type {}", token, typeid(T).name())};
  }
  return value;
}

/**
 * \brief Parses a line in an .obj file.
 * \tparam T The type to convert to.
 * \tparam N The number of items to convert (does not include the first token identifying the line type).
 * \param line The line to parse.
 * \return A vector of size \p N containing each item in \p line converted to type \p T.
 */
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

/**
 * \brief Parses a token representing a face element index group.
 * \param token The token to parse. May optionally contain texture coordinate and normal indices.
 * \return A vector containing vertex position, texture coordinate, and normal indices. Unspecified texture
 *         coordinate and normal values are indicated by the value \c kInvalidIndex.
 */
glm::ivec3 ParseIndexGroup(const std::string_view token) {
  static constexpr auto kDelimiter = "/";
  const auto delimiter_count = std::ranges::count(token, *kDelimiter);

  switch (const auto tokens = Split(token, kDelimiter); tokens.size()) {
    case 1:
      // case: f v0 v1 v2
      if (delimiter_count == 0) {
        const auto v = ParseToken<int>(tokens[0]);
        return glm::ivec3{v - 1, kInvalidIndex, kInvalidIndex};
      }
      break;
    case 2:
      // case: f v0/vt0 v1/vt1 v2/vt2
      if (delimiter_count == 1) {
        const auto v = ParseToken<int>(tokens[0]);
        const auto vt = ParseToken<int>(tokens[1]);
        return glm::ivec3{v - 1, vt - 1, kInvalidIndex};
      }
      // case: f v0//vn0 v1//vn1 v2//vn2
      if (delimiter_count == 2 && !token.starts_with(kDelimiter) && !token.ends_with(kDelimiter)) {
        const auto v = ParseToken<int>(tokens[0]);
        const auto vn = ParseToken<int>(tokens[1]);
        return glm::ivec3{v - 1, kInvalidIndex, vn - 1};
      }
      break;
    case 3:
      // case: v0/vt0/vn0 v1/vt1/vn1 v2/vt2/vn2
      if (delimiter_count == 2) {
        const auto v = ParseToken<int>(tokens[0]);
        const auto vt = ParseToken<int>(tokens[1]);
        const auto vn = ParseToken<int>(tokens[2]);
        return glm::ivec3{v - 1, vt - 1, vn - 1};
      }
      break;
    default:
      break;
  }

  throw std::invalid_argument{std::format("Unsupported format {}", token)};
}

/**
 * \brief Parses a line representing a triangular face element.
 * \param line The line to parse.
 * \return An array containing three parsed index groups for the face.
 */
std::array<glm::ivec3, 3> ParseFace(const std::string_view line) {
  if (const auto tokens = Split(line); tokens.size() == 4) {
    return std::array{ParseIndexGroup(tokens[1]), ParseIndexGroup(tokens[2]), ParseIndexGroup(tokens[3])};
  }
  throw std::invalid_argument{std::format("Unsupported format {}", line)};
}

/**
 * \brief Gets indexed vertex data (e.g., position, normal, texture coordinates).
 * \tparam T The vertex data vector type.
 * \tparam N The vertex data vector size.
 * \param data A list of homogeneous vertex data in the .obj file.
 * \param index The index position to get in \p data.
 * \return The vertex data at \p index or a default value if undefined in the .obj file.
 */
template <typename T, glm::length_t N>
constexpr glm::vec<N, T> TryGet(const std::vector<glm::vec<N, T>>& data, const int index) {
  if (index != kInvalidIndex) {
    // use bounds checking in case the index position in the .obj file is invalid
    return data.at(static_cast<std::size_t>(index));
  }
  return glm::vec<N, T>{};
}

/**
 * \brief Loads a triangle mesh from an input stream representing the contents of an .obj file.
 * \param device The graphics device used to the create and load mesh data into GPU memory.
 * \param istream The input stream to parse.
 * \return A mesh defined by the position, texture coordinates, normals, and indices specified in the input stream.
 */
gfx::Mesh LoadMesh(const gfx::Device& device, std::istream& istream) {
  std::vector<glm::vec3> positions;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texture_coordinates;
  std::vector<std::array<glm::ivec3, 3>> faces;

  for (std::string line; getline(istream, line);) {
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

  // create a vertex for each unique index group in the .obj file
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

  return gfx::Mesh{device, vertices, indices};
}

}  // namespace

gfx::Mesh gfx::obj_loader::LoadMesh(const Device& device, const std::filesystem::path& filepath) {
  if (std::ifstream ifstream{filepath}) {
    return ::LoadMesh(device, ifstream);
  }
  throw std::runtime_error{std::format("Unable to open {}", filepath.string())};
}

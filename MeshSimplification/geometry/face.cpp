#include "geometry/face.h"

#include <algorithm>
#include <cassert>
#include <tuple>

#include <glm/geometric.hpp>

using namespace glm;
using namespace qem;
using namespace std;

namespace {

	/**
	 * \brief Gets a canonical ordering of face vertices such that the vertex with the lowest ID.
	 * \param v0,v1,v2 The face vertices.
	 * \return A tuple consisting of \p v0, \p v1, \p v2 ordered by the lowest ID.
	 * \note Preserves winding order. The is necessary to disambiguate equivalent face elements queries.
	 */
	auto GetMinVertexOrder(
		const shared_ptr<const Vertex>& v0, const shared_ptr<const Vertex>& v1, const shared_ptr<const Vertex>& v2) {

		if (const auto min_id = min<>({v0->id(), v1->id(), v2->id()}); min_id == v0->id()) {
			return make_tuple(v0, v1, v2);
		} else if (min_id == v1->id()) {
			return make_tuple(v1, v2, v0);
		} else {
			return make_tuple(v2, v0, v1);
		}
	}
}

Face::Face(const shared_ptr<const Vertex>& v0, const shared_ptr<const Vertex>& v1, const shared_ptr<const Vertex>& v2) {

	tie(v0_, v1_, v2_) = GetMinVertexOrder(v0, v1, v2);

	const auto edge01 = v1_.lock()->position() - v0_.lock()->position();
	const auto edge02 = v2_.lock()->position() - v0_.lock()->position();
	const auto normal = cross(edge01, edge02);
	const auto normal_magnitude = length(normal);
	assert(normal_magnitude != 0.f); // ensure face vertices are not collinear

	normal_ = normal / normal_magnitude;
	area_ = .5f * normal_magnitude;
}

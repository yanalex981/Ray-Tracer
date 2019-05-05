#include <vector>

#include <glm/glm.hpp>

#include "plane.hpp"
#include "vector.hpp"

std::optional<hit> plane::intersect(const glm::dvec4 &ray_start, const glm::dvec4 &ray_end)
{
	auto inv = glm::inverse(transforms);

	// ray warped into model space
	auto start4 = inv * ray_start;
	auto start = glm::dvec3(start4 / start4.w);
	auto end4 = inv * ray_end;
	auto end = glm::dvec3(end4 / end4.w);
	auto dir = end - start;

	// no z difference, ray is either parallel to the plane or in the plane. just return nothing
	if (dir.z == 0)
		return {};

	// solution to the parametric eq
	double t = -start.z / dir.z;

	// intersecting behind the camera
	if (t < 0)
		return {};

	auto intersection = start + dir * t;
	auto u = intersection.x;
	auto v = intersection.y;

	// check if solution is in bounds of the 2x2 sq
	if (-1.0 <= u && u <= 1.0 && -1.0 <= v && v <= 1.0)
	{
		glm::dvec4 normal{ 0.0, 0.0, 1.0, 1.0 };
		auto world_normal = dir_to_world(transforms, normal);
		auto world_pt = transforms * glm::dvec4(intersection, 1.0);

		return {{
			glm::normalize(glm::dvec3(world_normal / world_normal.w)),
			glm::dvec3(world_pt / world_pt.w),
			this
		}};
	}

	return {};
}

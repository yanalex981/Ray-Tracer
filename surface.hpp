#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <vector>
#include <optional>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

// #include "matrix.hpp"
// #include "matrix_utils.hpp"
// #include "vector.hpp"
#include "material.hpp"

struct hit;

// extend to define surfaces
struct surface
{
	glm::dmat4 transforms = glm::identity<glm::dmat4>(); // transforms on this surface, to be able to create variations of this shape

	material m_material; // parameters for Phong lighting

	// returns a potential intersection given a ray
	virtual std::optional<hit> intersect(const glm::dvec4 &ray_start, const glm::dvec4 &ray_end) = 0;
};

// holds results from intersection checks
struct hit
{
	glm::dvec3 normal;
	glm::dvec3 world_pt;
	surface *obj;
};

#endif

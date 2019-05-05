#ifndef A4_PLANE_HPP
#define A4_PLANE_HPP

#include "surface.hpp"

struct plane : public surface
{
	virtual std::optional<hit> intersect(const glm::dvec4 &ray_start, const glm::dvec4 &ray_end);
};

#endif //A4_PLANE_HPP

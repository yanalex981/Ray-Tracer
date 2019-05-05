#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "surface.hpp"

struct sphere : surface
{
	virtual std::optional<hit> intersect(const glm::dvec4 &ray_start, const glm::dvec4 &ray_end);
};

#endif

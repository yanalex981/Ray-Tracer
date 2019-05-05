#ifndef A4_CONE_HPP
#define A4_CONE_HPP

#include "surface.hpp"

struct cone : public surface
{
	virtual std::optional<hit> intersect(const glm::dvec4 &ray_start, const glm::dvec4 &ray_end);
};


#endif //A4_CONE_HPP

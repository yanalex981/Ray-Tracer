#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>

struct light
{
	glm::dvec4 position;
	double intensity;
};

#endif

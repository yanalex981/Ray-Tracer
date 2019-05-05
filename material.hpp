#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm/glm.hpp>

#include "light.hpp"

struct material
{
	glm::dvec3 color;
	double k_ambient, k_diffuse, k_specular, k_reflect, fallout;
};

#endif

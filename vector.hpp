#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>
#include <vector>

#include <glm/glm.hpp>

glm::dvec4 dir_to_world(const glm::dmat4 &transform, const glm::dvec4 &dir);

#endif

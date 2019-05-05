#include "vector.hpp"

glm::dvec4 dir_to_world(const glm::dmat4 &transform, const glm::dvec4 &dir)
{
    auto start4 = transform * glm::dvec4{ 0.0, 0.0, 0.0, 1.0 };
    auto start = start4 / start4.w;
    auto end4 = transform * dir;
    auto end = end4 / end4.w;

    return glm::dvec4(glm::dvec3(end - start), 1.0);
}

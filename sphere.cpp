#include <iostream>
#include <cmath>

#include "sphere.hpp"
// #include "vector.hpp"
// #include "matrix_utils.hpp"

glm::dvec4 dir_to_world(const glm::dmat4 &transform, const glm::dvec4 &dir)
{
	auto start4 = transform * glm::dvec4{ 0.0, 0.0, 0.0, 1.0 };
	auto start = start4 / start4.w;
	auto end4 = transform * dir;
	auto end = end4 / end4.w;

	return glm::dvec4(glm::dvec3(end - start), 1.0);
}

std::optional<hit> sphere::intersect(const glm::dvec4 &ray_start, const glm::dvec4 &ray_end)
{
	auto inv = glm::inverse(transforms);
	auto start4 = inv * ray_start;
	auto start = glm::dvec3(start4 / start4.w);
	auto end4 = inv * ray_end;
	auto end = glm::dvec3(end4 / end4.w);
	auto dir = glm::normalize(end - start); // warp the ray into model space

	// solution to intersection
	double a = glm::dot(dir, dir);
	double b = 2 * glm::dot(dir, start);
	double c = glm::dot(start, start) - 1;

	double discrim = b*b - 4*a*c;

	// discriminant has to be >= 0
	// no hits
	if (discrim < 0)
		return {};

	double t1 = (-b + std::sqrt(discrim)) / (2 * a);
	double t2 = (-b - std::sqrt(discrim)) / (2 * a);

	// "intersects" behind camera, false alarm
	if (t1 < 0 && t2 < 0)
		return {};

	// values for one intersection
	auto i1 = start + dir * t1;
	auto w14 = transforms * glm::dvec4(i1, 1.0);
	auto w1 = w14 / w14.w;
	auto n1 = glm::normalize(glm::dvec3(dir_to_world(transforms, glm::dvec4(i1, 1.0))));
	hit hit1{ n1, w1, this };

	// values for the alternate intersection
	auto i2 = start + dir * t2;
	auto w24 = transforms * glm::dvec4(i2, 1.0);
	auto w2 = w24 / w24.w;
	auto n2 = glm::normalize(glm::dvec3(dir_to_world(transforms, glm::dvec4(i2, 1.0))));
	hit hit2{ n2, w2, this };

	// both in front of camera, pick the closest one
	if (t1 >= 0 && t2 >= 0)
	{
		if (t1 < t2)
			return hit1;

		return hit2;
	}

	// one or the other is in front of the camera, pick that one
	if (t1 >= 0)
		return hit1;

	return hit2;
}

#include <iostream>
#include <cmath>
#include <optional>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "cone.hpp"
#include "vector.hpp"

// returns the normal of the cone based on a point of intersection
glm::dvec4 normal(const glm::dvec3 &v)
{
	glm::dmat4 id = glm::identity<glm::dmat4>();
	double angle = glm::degrees(std::atan2(v.x, v.z));
	glm::dvec3 axis = glm::dvec3{ 0, 1, 0 };

	return glm::rotate(id, angle, axis) * glm::dvec4{ 0, 1, 1, 1 };
}

std::optional<hit> cone::intersect(const glm::dvec4 &ray_start, const glm::dvec4 &ray_end)
{
	// warp ray into model space
	auto inv = glm::inverse(transforms);
	auto start4 = inv * ray_start;
	auto start = glm::dvec3(start4 / start4.w);
	auto end4 = inv * ray_end;
	auto end = glm::dvec3(end4 / end4.w);
	auto dir = glm::normalize(end - start);

	auto tip = glm::dvec3{ 0.0, 1.0, 0.0 }; // tip of the cone
	auto origin = glm::dvec3{ 0.0, 0.0, 0.0 }; // origin

	auto v = glm::normalize(origin - tip); // vec from tip going towards origin
	auto co = start - tip; // vec from tip towards ray's starting point in model space

	// solutions to intersection
	double a = std::pow(glm::dot(dir, v), 2) - 0.5;
	double b = 2 * ( glm::dot(dir, v) * glm::dot(co, v) - glm::dot(dir, co) / 2.0 );
	double c = std::pow(glm::dot(co, v), 2) - glm::dot(co, co) / 2.0;

	double discrim = b * b - 4 * a * c;

	// not intersecting
	if (discrim < 0)
		return {};

	// 2 intersections
	double t1 = (-b - std::sqrt(discrim)) / 2 / a;
	double t2 = (-b + std::sqrt(discrim)) / 2 / a;

	// intersecting behind the camera
	if (t1 < 0 && t2 < 0)
		return {};

	// various vectors and dots for one intersection
	auto i2 = start + dir * t2; // intersection on cone surface in model coords
	auto w24 = transforms * glm::dvec4(i2, 1); // intersection in world coords
	auto w2 = w24 / w24.w;
	auto n24 = dir_to_world(transforms, normal(i2)); // normal dir in world coords
	auto n2 = n24 / n24.w;
	auto dot2 = glm::dot(i2 - tip, v); // shadow of the intersection on the axis
	bool d2_in_range = 0 <= dot2 && dot2 <= 1; // the shadow should be positive and less than 1
	// >1 means the point is below the base of the cone, which is not actually part of the cone we want
	// <0 means the point is on the shadow cone above the tip
	hit hit2{ n2, w2, this };
	if (t1 < 0) // t1 is behind the camera
	{
		// check if the other point is overextending
		if (d2_in_range)
			return hit2;
		else
			return {};
	}

	// various vectors and dots for the other intersection
	auto i1 = start + dir * t1;
	auto w14 = transforms * glm::dvec4(i1, 1);
	auto w1 = w14 / w14.w;
	auto n14 = dir_to_world(transforms, normal(i1));
	auto n1 = n14 / n14.w;
	auto dot1 = glm::dot(i1 - tip, v);
	bool d1_in_range = 0 <= dot1 && dot1 <= 1;
	hit hit1{ n1, w1, this };
	if (t2 < 0) // t2 is behind the camera
	{
		if (d1_in_range)
			return hit1;
		else
			return {};
	}

	if (!d1_in_range && !d2_in_range)
		return {}; // false alarm, intersecting the shadow cone, or below the base

	// both are in range, return the closest one 'cause the other one is actually behind it
	if (d1_in_range && d2_in_range)
	{
		if (t1 < t2)
			return hit1;

		return hit2;
	}

	// one is negative, return either
	if (d2_in_range)
		return hit2;

	return hit1;
}

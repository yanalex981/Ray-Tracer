//  module: CS 3388 Assignment 4
//  author: Zhen Yan
//    date: Fri, Apr 5, 2019
// purpose: ray trace a scene of generic objects

#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <utility>
#include <limits>

#include <SFML/Graphics.hpp>

#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/matrix.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "vector.hpp"
#include "light.hpp"
#include "plane.hpp"
#include "cone.hpp"
#include "sphere.hpp"

template<typename T>
glm::dmat4 camera(const glm::dvec3 &eye, const glm::dvec3 &gaze, const glm::dvec3 &up)
{
	auto dir = glm::normalize(eye - gaze);
	auto u = glm::normalize(glm::cross(up, dir));
	auto v = glm::normalize(glm::cross(dir, u));

	glm::dmat4 m{
		u.x, u.y, u.z, 0,
		v.x, v.y, v.z, 0,
		dir.x, dir.y, dir.z, 0,
		eye.x, eye.y, eye.z, 1
	};

	return glm::inverse(m);
}

template<typename T>
glm::dmat4 perspective(T far, T near, T left, T right, T top, T bottom)
{
	T a = -(far + near) / (far - near);
	T b = -2 * far * near / (far - near);

	glm::dmat4 mp{
		near, 0, 0, 0,
		0, near, 0, 0,
		0, 0, a, -1,
		0, 0, b, 0
	};

	glm::dmat4 s1{
		2 / (right - left), 0 ,0, 0,
		0, 2 / (top - bottom), 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	glm::dmat4 t1{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		-(right + left) / 2, -(top + bottom) / 2, 0, 1
	};

	return s1 * t1 * mp;
}

template<typename T>
glm::dmat4 screen(T width, T height)
{
	glm::dmat4 t2{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		1, 1, 0, 1
	};

	glm::dmat4 s2{
		width / 2, 0, 0, 0,
		0, height / 2, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	glm::dmat4 w{
		1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, 1, 0,
		0, height, 0, 1
	};

	return w * s2 * t2;
}

// find intersections in a scene given a ray
template<typename C>
std::optional<hit> find_intersection(const C &scene, const glm::dvec4 &ray_start, const glm::dvec4 &ray_end)
{
	std::vector<hit> hits;

	// get the intersections of this ray and every object in the scene
	for (auto &obj : scene)
	{
		auto intersection = obj->intersect(ray_start, ray_end);

		if (intersection)
			hits.push_back(intersection.value());
	}

	if (hits.size() < 1)
		return {};

	// return the shortest
	return *std::min_element(hits.begin(), hits.end(), [&](const hit &a, const hit &b)
	{
		auto ra = a.world_pt - glm::dvec3(ray_start / ray_start.w);
		auto rb = b.world_pt - glm::dvec3(ray_start / ray_start.w);

		return glm::dot(ra, ra) < glm::dot(rb, rb);
	});
}

int main()
{
	const size_t window_width = 1000, window_height = 600;
	sf::RenderWindow window(sf::VideoMode(window_width, window_height), "pew pew pew");

	sf::Image image; // colleciton of pixels. cannot be drawn directly by SFML

	sf::Texture texture; // need a texture to make a sprite
	sf::Sprite sprite; // SFML can draw sprites

	// camera params
	auto eye = glm::dvec3{ 0, 40, 80 };
	auto gaze = glm::dvec3{ 0, 0, 0 };
	auto up = glm::dvec3{ 0, 1, 0 };

	// window, camera, perspective matrices
	auto ms = screen( 1.0 * window_width, 1.0 * window_height );
	auto mp = perspective( 10.0, 1.0, -1.0, 1.0, 0.6, -0.6 );
	auto mc = camera<double>( eye, gaze, up );
	auto mvp = ms * mp * mc;
	auto inv = glm::inverse(mvp);

	light bulb{ glm::dvec4{ 40, 80, 0, 1 }, 1.0 };

	sphere ball;
	ball.transforms = glm::translate(glm::identity<glm::dmat4>(), glm::dvec3{ -20.0, 20.0, 0.0 }) * glm::scale(glm::identity<glm::dmat4>(), glm::dvec3{ 20.0, 20.0, 20.0 });
	ball.m_material.color = glm::dvec3{ 255, 150, 0 };
	// ball.m_material.k_ambient = 0.08;
	// ball.m_material.k_diffuse = 1;
	// ball.m_material.k_specular = 255;
	// ball.m_material.fallout = 256;
	ball.m_material.k_ambient = 0;
	ball.m_material.k_diffuse = 1;
	ball.m_material.k_specular = 0;
	ball.m_material.fallout = 0;

	plane ground;
	ground.transforms = glm::scale(glm::identity<glm::dmat4>(), glm::dvec3{ 100.0, 100.0, 100.0 }) * glm::rotate(glm::identity<glm::dmat4>(), -M_PI / 2, glm::dvec3{ 1, 0, 0 });
	ground.m_material.color = glm::dvec3{ 180, 180, 180 };
	ground.m_material.k_ambient = 0.1;
	ground.m_material.k_diffuse = 1;
	ground.m_material.k_specular = 255;
	ground.m_material.fallout = 25;

	cone dunce;
	dunce.transforms = glm::translate(glm::identity<glm::dmat4>(), glm::dvec3{ 40.0, 0.0, 0.0 }) * glm::scale(glm::identity<glm::dmat4>(), glm::dvec3{ 20.0, 20.0, 20.0 }) * glm::scale(glm::identity<glm::dmat4>(), glm::dvec3{ 1.0, 2.0, 1.0 });
 	dunce.m_material.color = glm::dvec3{ 0, 180, 180 };
	// dunce.m_material.k_ambient = 0.1;
	// dunce.m_material.k_diffuse = 1;
	// dunce.m_material.k_specular = 255;
	// dunce.m_material.fallout = 256;
	dunce.m_material.k_ambient = 0.1;
	dunce.m_material.k_diffuse = 1;
	dunce.m_material.k_specular = 255;
	dunce.m_material.fallout = 256;

	std::array<surface *, 3> scene{{ &dunce, &ground, &ball }};

	image.create(window_width, window_height, sf::Color(0, 0, 0, 0)); // init to 100% transparent
	window.clear(sf::Color::White);

	// trace those rays!
	for (size_t x = 0; x < window_width; ++x)
	{
		for (size_t y = 0; y < window_height; ++y)
		{
			// screen space
			glm::dvec4 ray_end{ 1.0 * x, 1.0 * y, 1, 1 };

			// world space
			auto ray_start_w = glm::dvec4{ eye, 1 };
			auto ray_end_w = inv * ray_end;

			// find intersection
			auto intersection = find_intersection(scene, ray_start_w, ray_end_w);
			if (!intersection)
				continue;

			auto hit = intersection.value();

			// lighting computation
			double ambient = hit.obj->m_material.k_ambient;

			glm::dvec3 s = glm::normalize(glm::dvec3(bulb.position / bulb.position.w) - hit.world_pt);
			double diffuse = bulb.intensity * hit.obj->m_material.k_diffuse * std::max(0.0, glm::dot(s, hit.normal));

			glm::dvec3 ref = -s + hit.normal * 2.0 * glm::dot(s, hit.normal);
			glm::dvec3 v = glm::normalize(eye - hit.world_pt);

			double specular = bulb.intensity * hit.obj->m_material.k_specular * std::pow(std::max(0.0, glm::dot(ref, v)), hit.obj->m_material.fallout);

			auto r = static_cast<uint8_t>(glm::clamp(std::round(diffuse * hit.obj->m_material.color.x + ambient + specular), 0.0, 255.0));
			auto g = static_cast<uint8_t>(glm::clamp(std::round(diffuse * hit.obj->m_material.color.y + ambient + specular), 0.0, 255.0));
			auto b = static_cast<uint8_t>(glm::clamp(std::round(diffuse * hit.obj->m_material.color.z + ambient + specular), 0.0, 255.0));

			// trace those shadows!
			auto obstruction = find_intersection(scene, bulb.position, glm::dvec4{hit.world_pt, 1});
			if (obstruction)
			{
				auto pt = obstruction.value();
				auto d = hit.world_pt - pt.world_pt;
				if (glm::dot(d, d) > std::numeric_limits<double>::epsilon())
				{
					// obstructed
					r = ambient * hit.obj->m_material.color.x;
					g = ambient * hit.obj->m_material.color.y;
					b = ambient * hit.obj->m_material.color.z;
				}
			}

			// for debugging
		    // uint8_t r = static_cast<uint8_t>(glm::clamp(hit.normal.x * 255.0, 0.0, 255.0));
		    // uint8_t g = static_cast<uint8_t>(glm::clamp(hit.normal.y * 255.0, 0.0, 255.0));
		    // uint8_t b = static_cast<uint8_t>(glm::clamp(hit.normal.z * 255.0, 0.0, 255.0));

			image.setPixel(x, y, sf::Color{ r, g, b, 255 });
		}
	}

	texture.loadFromImage(image); // convert to texture
	sprite.setTexture(texture); // convert to sprite
	window.draw(sprite);
	window.display();

	while (window.isOpen()) // poll for input while window is open
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			// if close button, or Q button pressed, close window gracefully
			if (event.type == sf::Event::Closed ||
			   (event.type == sf::Event::KeyPressed &&
				event.key.code == sf::Keyboard::Q))
				window.close();
		}
	}
	
	return 0;
}

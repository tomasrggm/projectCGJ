#pragma once
#include <glm/glm.hpp>

struct AABB {
	// v1 v2
	// v3 v4
	/*
	glm::vec4 vert1 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 vert2 = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	glm::vec4 vert3 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 vert4 = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec4 tl = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // top left = v1
	glm::vec4 br = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // bottom right = v4
	*/

	/*
	glm::vec4 vert1 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 vert2 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 vert3 = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec4 vert4 = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	glm::vec4 tl = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // top left = v1
	glm::vec4 br = glm::vec4(-1.0f, 0.0f, 1.0f, 1.0f); // bottom right = v4
	*/
	glm::vec4 v1 = glm::vec4(3.95f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v2 = glm::vec4(3.95f, 0.0f, 1.0f, 1.0f);
	glm::vec4 v3 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v4 = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

	glm::vec4 vert1 = glm::vec4(3.95f, 0.0f, 0.0f, 1.0f);
	glm::vec4 vert2 = glm::vec4(3.95f, 0.0f, 1.0f, 1.0f);
	glm::vec4 vert3 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 vert4 = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	glm::vec4 tl = glm::vec4(3.95f, 0.0f, 0.0f, 1.0f); // top left = v1
	glm::vec4 br = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // bottom right = v4
};
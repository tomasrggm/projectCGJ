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
	//Colliding box po cabeso
	
	glm::vec4 v1 = glm::vec4(2.0f, 0.0f, -0.25f, 1.0f);
	glm::vec4 v2 = glm::vec4(2.0f, 0.0f, 0.25f, 1.0f);
	glm::vec4 v3 = glm::vec4(-2.0f, 0.0f, -0.25f, 1.0f);
	glm::vec4 v4 = glm::vec4(-2.0f, 0.0f, 0.25f, 1.0f);

	glm::vec4 vert1 = glm::vec4(2.0f, 0.0f, -0.25f, 1.0f);
	glm::vec4 vert2 = glm::vec4(2.0f, 0.0f, 0.25f, 1.0f);
	glm::vec4 vert3 = glm::vec4(-2.0f, 0.0f, -0.25f, 1.0f);
	glm::vec4 vert4 = glm::vec4(-2.0f, 0.0f, 0.25f, 1.0f);
	glm::vec4 tl = glm::vec4(2.0f, 0.0f, -0.25f, 1.0f); // top left = v1
	glm::vec4 br = glm::vec4(-2.0f, 0.0f, 0.25f, 1.0f); // bottom right = v4
	
	//Colliding box pas asas
	
	glm::vec4 v1A = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v2A = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v3A = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v4A = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	glm::vec4 vert1A = glm::vec4(0.5f, 0.0f, -2.5f, 1.0f);
	glm::vec4 vert2A = glm::vec4(0.5f, 0.0f, 2.5f, 1.0f);
	glm::vec4 vert3A = glm::vec4(-0.5f, 0.0f, -2.5f, 1.0f);
	glm::vec4 vert4A = glm::vec4(-0.5f, 0.0f, 2.5f, 1.0f);
	glm::vec4 tlA = glm::vec4(0.5f, 0.0f, -2.5f, 1.0f); // top left = v1
	glm::vec4 brA = glm::vec4(-0.5f, 0.0f, 2.5f, 1.0f); // bottom right = v4
};
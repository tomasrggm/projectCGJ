#pragma once
#include <glm/glm.hpp>

struct AABB {
	//Colliding box po cabeso
	
	glm::vec4 v1 = glm::vec4(2.0f, 0.0f, -0.25f, 1.0f);
	glm::vec4 v2 = glm::vec4(2.0f, 0.0f, 0.25f, 1.0f);
	glm::vec4 v3 = glm::vec4(-2.0f, 0.0f, -0.25f, 1.0f);
	glm::vec4 v4 = glm::vec4(-2.0f, 0.0f, 0.25f, 1.0f);

	glm::vec4 vert1 = glm::vec4(2.0f, 0.0f, -0.25f, 1.0f);
	glm::vec4 vert2 = glm::vec4(2.0f, 0.0f, 0.25f, 1.0f);
	glm::vec4 vert3 = glm::vec4(-2.0f, 0.0f, -0.25f, 1.0f);
	glm::vec4 vert4 = glm::vec4(-2.0f, 0.0f, 0.25f, 1.0f);
	
	//Colliding box pas asas
	
	glm::vec4 v1A = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v2A = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v3A = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v4A = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	glm::vec4 vert1A = glm::vec4(0.5f, 0.0f, -2.5f, 1.0f);
	glm::vec4 vert2A = glm::vec4(0.5f, 0.0f, 2.5f, 1.0f);
	glm::vec4 vert3A = glm::vec4(-0.5f, 0.0f, -2.5f, 1.0f);
	glm::vec4 vert4A = glm::vec4(-0.5f, 0.0f, 2.5f, 1.0f);
	
	//2 Boxes para tratar das colisões dos misseis e cenas
	glm::vec4 v1C = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v2C = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v3C = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v4C = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v5C = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v6C = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v7C = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v8C = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	glm::vec4 vert1C = glm::vec4(2.0f, 0.25f, -0.25f, 1.0f);
	glm::vec4 vert2C = glm::vec4(2.0f, 0.25f, 0.25f, 1.0f);
	glm::vec4 vert3C = glm::vec4(2.0f, -0.25f, -0.25f, 1.0f);
	glm::vec4 vert4C = glm::vec4(2.0f, -0.25f, 0.25f, 1.0f);
	glm::vec4 vert5C = glm::vec4(-2.0f, 0.25f, -0.25f, 1.0f);
	glm::vec4 vert6C = glm::vec4(-2.0f, 0.25f, 0.25f, 1.0f);
	glm::vec4 vert7C = glm::vec4(-2.0f, -0.25f, -0.25f, 1.0f);
	glm::vec4 vert8C = glm::vec4(-2.0f, -0.25f, 0.25f, 1.0f);
	
	glm::vec4 tlC = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 brC = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	glm::vec4 v1CA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v2CA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v3CA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v4CA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v5CA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v6CA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v7CA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 v8CA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	glm::vec4 vert1CA = glm::vec4(0.5f, 0.1f, -2.5f, 1.0f);
	glm::vec4 vert2CA = glm::vec4(0.5f, 0.1f, 2.5f, 1.0f);
	glm::vec4 vert3CA = glm::vec4(0.5f, -0.1f, -2.5f, 1.0f);
	glm::vec4 vert4CA = glm::vec4(0.5f, -0.1f, 2.5f, 1.0f);
	glm::vec4 vert5CA = glm::vec4(-0.5f, 0.1f, -2.5f, 1.0f);
	glm::vec4 vert6CA = glm::vec4(-0.5f, 0.1f, 2.5f, 1.0f);
	glm::vec4 vert7CA = glm::vec4(-0.5f, -0.1f, -2.5f, 1.0f);
	glm::vec4 vert8CA = glm::vec4(-0.5f, -0.1f, 2.5f, 1.0f);
	
	glm::vec4 tlCA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 brCA = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	};
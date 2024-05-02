#pragma once
#include <gl2d/gl2d.h>

constexpr float enemyShipSize = 250.f;

struct Enemy
{

	// Index into texture atlas
	glm::uvec2 type = {};
	glm::vec2 position = {};
	glm::vec2 viewDirection = { 1, 0 };

	// Enemy Attributes
	float speed = 1500.f;
	float turnSpeed = 3.f;
	float firedTime = 1.f;
	float fireTimeReset = 0.3;
	float fireRange = 1.5;
	float bulletSpeed = 2000.f;

	void render(gl2d::Renderer2D& renderer, gl2d::Texture& sprites,
		gl2d::TextureAtlasPadding& atlas);

	bool update(float deltaTime, glm::vec2 playerPos);

};
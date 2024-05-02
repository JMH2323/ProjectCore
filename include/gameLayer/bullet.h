
#pragma once

#include <gl2d/gl2d.h>

struct Bullet
{
	glm::vec2 position = {};
	glm::vec2 fireDirection = {};

	float bulletSpeed = 3000.f;
	bool isEnemy = false;
	float bulletSize = 50.f;;

	void render(gl2d::Renderer2D& renderer,
		gl2d::Texture bulletsTexture, gl2d::TextureAtlasPadding bulletsAtlas);

	void update(float deltaTime);
};
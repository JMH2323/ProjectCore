#pragma once
#include <gl2d/gl2d.h>

constexpr float enemyShipSize = 250.f;

struct Enemy
{

	// Index into texture atlas
	glm::uvec2 type = {};
	glm::vec2 position = {};
	glm::vec2 viewDirection = { 1, 0 };

	void render(gl2d::Renderer2D& renderer, gl2d::Texture& sprites,
		gl2d::TextureAtlasPadding& atlas);

};
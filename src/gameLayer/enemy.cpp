#include <enemy.h>
#include <tileRenderer.h>


void Enemy::render(gl2d::Renderer2D& renderer,
	gl2d::Texture& sprites, gl2d::TextureAtlasPadding& atlas)
{

	renderSpaceShip(renderer, position, enemyShipSize,
		sprites, atlas.get(type.x, type.y), viewDirection);


}


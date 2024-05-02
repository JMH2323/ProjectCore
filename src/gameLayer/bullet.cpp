#include <bullet.h>


void Bullet::render(gl2d::Renderer2D& renderer,
	gl2d::Texture bulletsTexture, gl2d::TextureAtlasPadding bulletsAtlas)
{
	float spaceShipAngle = atan2(fireDirection.y, -fireDirection.x);
	spaceShipAngle = glm::degrees(spaceShipAngle) + 90.f;

	if (isEnemy)
	{
		renderer.renderRectangle({ position - glm::vec2(25,25), bulletSize,bulletSize },
		bulletsTexture, Colors_White, {}, spaceShipAngle, bulletsAtlas.get(1, 0));
	}
	else
	{
		renderer.renderRectangle({ position - glm::vec2(25,25), bulletSize,bulletSize },
		bulletsTexture, Colors_White, {}, spaceShipAngle, bulletsAtlas.get(0, 0));
	}
}

void Bullet::update(float deltaTime)
{

	position += fireDirection * deltaTime * bulletSpeed;

}
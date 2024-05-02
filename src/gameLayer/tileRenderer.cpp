#include <tileRenderer.h>

void TiledRenderer::render(gl2d::Renderer2D& renderer)
{
	// Get the size of view of the player
	auto viewRect = renderer.getViewRect();
	
	// Get the paralaxDistance and multiply it by strength. 
	// Added to rendering call to make the background move slower
	glm::vec2 paralaxDistance = { viewRect.x, viewRect.y };
	paralaxDistance *= -paralaxStrength;

	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			int posX = x + int((viewRect.x - paralaxDistance.x) / backgroundSize);
			int posY = y + int((viewRect.y - paralaxDistance.y) / backgroundSize);

			renderer.renderRectangle(
				glm::vec4{ posX, posY, 1, 1 } *backgroundSize
				+glm::vec4(paralaxDistance, 0, 0), texture);

		}
	}




}
#include <tileRenderer.h>

void TiledRenderer::render(gl2d::Renderer2D& renderer)
{
	// Get the size of view of the player
	auto viewRect = renderer.getViewRect();
	

	for (int y = -1; y <= 1; y++)
	{
		for (int x = -1; x <= 1; x++)
		{
			int posX = x + int(viewRect.x / backgroundSize);
			int posY = y + int(viewRect.y / backgroundSize);

			renderer.renderRectangle(
				glm::vec4{ posX, posY, 1, 1 } *backgroundSize, texture);

		}
	}


	renderer.renderRectangle({ 0, 0, backgroundSize, backgroundSize }, texture);


}
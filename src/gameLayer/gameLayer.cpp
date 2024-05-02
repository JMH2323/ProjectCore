#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>

#include <tileRenderer.h>

struct GameplayData
{
	glm::vec2 playerPos = { 100,100 };
};

GameplayData data;

#pragma region textureLoading

gl2d::Renderer2D renderer;
constexpr int BACKGROUNDS = 3;

gl2d::Texture playerTexture;
gl2d::Texture BK_spaceTexture;
gl2d::Texture BK_starTexture;
gl2d::Texture BK_planetTexture;


#pragma endregion

gl2d::Texture backgroundTexture[BACKGROUNDS];
TiledRenderer tiledRenderer[BACKGROUNDS];


bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();

	playerTexture.loadFromFile(RESOURCES_PATH "spaceShip/ships/green.png", true);
	backgroundTexture[0].loadFromFile(RESOURCES_PATH "background/Space.png", true);
	backgroundTexture[1].loadFromFile(RESOURCES_PATH "background/Stars.png", true);
	backgroundTexture[2].loadFromFile(RESOURCES_PATH "background/Planets.png", true);

	tiledRenderer[0].texture = backgroundTexture[0];
	tiledRenderer[1].texture = backgroundTexture[1];
	tiledRenderer[2].texture = backgroundTexture[2];

	
	return true;
}



bool gameLogic(float deltaTime)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);
#pragma endregion


#pragma region movement

	glm::vec2 move = {};


	// Movement through Vectors
	// Move Up
	if (
		platform::isButtonHeld(platform::Button::W) ||
		platform::isButtonHeld(platform::Button::Up)
		)
	{
		move.y = -1;
	}
	// Move Down
	if (
		platform::isButtonHeld(platform::Button::S) ||
		platform::isButtonHeld(platform::Button::Down)
		)
	{
		move.y = 1;
	}
	// Move Left
	if (
		platform::isButtonHeld(platform::Button::A) ||
		platform::isButtonHeld(platform::Button::Left)
		)
	{
		move.x = -1;
	}
	// Move Right
	if (
		platform::isButtonHeld(platform::Button::D) ||
		platform::isButtonHeld(platform::Button::Right)
		)
	{
		move.x = 1;
	}

	// Normalize Movement
	if (move.x != 0 || move.y != 0)
	{
		move = glm::normalize(move);
		move *= deltaTime * 1000; // 1000 pixels per second
		data.playerPos += move;
	}


#pragma endregion

#pragma region renderBackground


	// Camera Zoom, test for background
	renderer.currentCamera.zoom = 0.5;

	// loop through backgrounds and render each
	for (int i = 0; i < BACKGROUNDS; i++)
	{
		tiledRenderer[i].render(renderer);
	}


#pragma endregion


	// Camera Follow
	renderer.currentCamera.follow(data.playerPos, deltaTime * 450, 10, 50, w, h);

#pragma region renderShips

	// Render Player
	renderer.renderRectangle({ data.playerPos, 200, 200 }, playerTexture);

#pragma endregion
	


	renderer.flush();


	//ImGui::ShowDemoWindow();


	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{



}

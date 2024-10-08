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

// My Includes
#include <tileRenderer.h>
#include <bullet.h>
#include <vector>
#include <enemy.h>
#include <cstdio>
#include <glui/glui.h>
#include <raudio.h>


struct GameplayData
{
	// Player Position
	glm::vec2 playerPos = { 100,100 };

	// Keep track of Bullets, Enemies
	std::vector<Bullet> bullets;
	std::vector<Enemy> enemies;

	float playerHealth = 1.f;
	float fireRate = 0.2;
	float fireReset = 0.2;
	
	// Player sprite size for determining rendering
	float shipSize = 250.f;

	// Enemy Spawn
	float enemySpawnRate = 3.f;
	int enemySpawnMod = 3;

	float score = 0;

};

GameplayData data;

#pragma region AssetLoading

gl2d::Renderer2D renderer;
constexpr int BACKGROUNDS = 3;

gl2d::Texture playerTexture;
gl2d::TextureAtlasPadding playersAtlas;

gl2d::Texture bulletsTexture;
gl2d::TextureAtlasPadding bulletsAtlas;

gl2d::Texture spaceShipsTexture;
gl2d::TextureAtlasPadding spaceShipsAtlas;

gl2d::Texture backgroundTexture[BACKGROUNDS];
TiledRenderer tiledRenderer[BACKGROUNDS];

gl2d::Texture healthBar;
gl2d::Texture health;

gl2d::Texture button;

gl2d::Font font;

glui::RendererUi uiRenderer;

Sound shootSound1;
Sound shootSound2;
Sound shootSound3;
Sound gameplaySound;

#pragma endregion

// Not a good long term solution. TODO
bool gameIsRunning = 0;

void restartGame()
{
	data = {};
	renderer.currentCamera.follow(data.playerPos
		, 550, 0, 0, renderer.windowW, renderer.windowH);

	if (gameIsRunning) { PlaySound(gameplaySound); }
}

bool bulletCollision(glm::vec2 bulletPos, glm::vec2 shipPos, float shipSize)
{
	return glm::distance(bulletPos, shipPos) <= shipSize;
}

void spawnEnemy()
{
	// List of ship sprites. Can expand into enemy classes TODO?
	glm::uvec2 shipTypes[] = { {0,0}, {0,1}, {2,0}, {3, 1} };

	Enemy e;
	e.position = data.playerPos;

	glm::vec2 offset(2000, 0);
	offset = glm::vec2(glm::vec4(offset, 0, 1) * glm::rotate(glm::mat4(1.f), glm::radians((float)(rand() % 360)), glm::vec3(0, 0, 1)));

	e.position += offset;

	e.speed = 800.f + rand() % 1000;
	e.turnSpeed = 2.2 + (rand() & 1000) / 500.f;
	e.type = shipTypes[rand() % 4];
	e.fireRange = 1.5 + (rand() % 1000) / 2000.f;
	e.fireTimeReset = 0.1 + (rand() % 1000) / 500;
	e.bulletSpeed = rand() % 3000 + 1000;

	// TODO bullet speed

	data.enemies.push_back(e);
}

bool initGame()
{

	std::srand(std::time(0));


	// Initializing for the renderer
	gl2d::init();
	renderer.create();

	// Player Text/Pad
	playerTexture.loadFromFileWithPixelPadding
	(RESOURCES_PATH "excessAssets/Player/PlayerShip.png", 128, true);
	playersAtlas = gl2d::TextureAtlasPadding(12, 5, playerTexture.GetSize().x, playerTexture.GetSize().y);

	// Bullets Text/Pad
	bulletsTexture.loadFromFileWithPixelPadding
	(RESOURCES_PATH "spaceShip/stitchedFiles/projectiles.png", 500, true);
	bulletsAtlas = gl2d::TextureAtlasPadding(3, 2, bulletsTexture.GetSize().x, bulletsTexture.GetSize().y);

	// Enemy Text/Pad
	spaceShipsTexture.loadFromFileWithPixelPadding
	(RESOURCES_PATH "spaceShip/stitchedFiles/spaceships.png", 128, true);
	spaceShipsAtlas = gl2d::TextureAtlasPadding(5, 2, spaceShipsTexture.GetSize().x, spaceShipsTexture.GetSize().y);

	// HealthBar Text
	healthBar.loadFromFile(RESOURCES_PATH "excessAssets/HealthBar/Bar/noHP.png", true);
	health.loadFromFile(RESOURCES_PATH "excessAssets/HealthBar/Health/health.png", true);

	// Background 1
	backgroundTexture[0].loadFromFile(RESOURCES_PATH "background/Space.png", true);
	backgroundTexture[1].loadFromFile(RESOURCES_PATH "background/Stars.png", true);
	backgroundTexture[2].loadFromFile(RESOURCES_PATH "background/Planets.png", true);

	// Sounds
	shootSound1 = LoadSound(RESOURCES_PATH "arcade_shot1.wav");
	SetSoundVolume(shootSound1, 0.8);
	shootSound2 = LoadSound(RESOURCES_PATH "arcade_shot2.wav");
	SetSoundVolume(shootSound2, 0.8);
	shootSound3 = LoadSound(RESOURCES_PATH "arcade_shot3.wav");
	SetSoundVolume(shootSound3, 0.5);
	gameplaySound = LoadSound(RESOURCES_PATH "Battle in the Stars.ogg");
	SetSoundVolume(gameplaySound, 0.10);
	
	//Font
	font.createFromFile(RESOURCES_PATH "roboto_black.ttf");
	button.loadFromFile(RESOURCES_PATH "space/Black Hole2.png", true);


	// Background 2
	tiledRenderer[0].texture = backgroundTexture[0];
	tiledRenderer[1].texture = backgroundTexture[1];
	tiledRenderer[2].texture = backgroundTexture[2];

	tiledRenderer[0].paralaxStrength = 0;
	tiledRenderer[1].paralaxStrength = 0.4;
	tiledRenderer[2].paralaxStrength = 0.7;


	restartGame();
	
	return true;
}

void gameplay(float deltaTime, int w, int h) 
{
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
		move *= deltaTime * 1500; // 1500 pixels per second
		data.playerPos += move;
	}


#pragma endregion

#pragma region Camerafollow

	// Camera Follow
	renderer.currentCamera.follow(data.playerPos, deltaTime * 1450, 1, 50, w, h);

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

#pragma region mousePos


	glm::vec2 mousePos = platform::getRelMousePosition();
	glm::vec2 screenCenter(w / 2.f, h / 2.f);

	// Updated: Player is now rendered in center so the mouse is also centered.
	// Center offset for determining rotation
	//glm::vec2 centerOffset(playerSpriteSize/4, playerSpriteSize/4); // Quarter of the player sprite size

	glm::vec2 mouseDirection = mousePos - screenCenter;

	if (glm::length(mouseDirection) == 0.f)
	{
		mouseDirection = { 1,0 };
	}
	else
	{
		mouseDirection = normalize(mouseDirection);
	}

	float spaceShipAngle = atan2(mouseDirection.y, -mouseDirection.x);

#pragma endregion

#pragma region handlePlayerBullets



	data.fireRate -= deltaTime;
	if (data.fireRate < 0) { data.fireRate = 0.f; }

	if (platform::isLMouseHeld() && data.fireRate == 0.f)
	{


		data.fireRate = data.fireReset;

		Bullet b1;
		Bullet b2;
		// Coordinates of the gun barrel's exit point, relative to the player's center (a quarter from center)
		glm::vec2 gunOffset = glm::vec2(0, data.shipSize / 4);

		// Rotate the offset according to the sprite's rotation
		float s = sin(spaceShipAngle);
		float c = cos(spaceShipAngle);
		glm::vec2 rotatedGunOffset;
		rotatedGunOffset.x = gunOffset.x * c + gunOffset.y * s;
		rotatedGunOffset.y = -gunOffset.x * s + gunOffset.y * c;

		// Create bullets at position, with offset and direction.
		b1.position = data.playerPos + rotatedGunOffset;
		b1.bulletSpeed = 3000.f;
		b1.fireDirection = mouseDirection;
		// Second bullet has negative offset
		b2.position = data.playerPos - rotatedGunOffset;
		b2.bulletSpeed = 3000.f;
		b2.fireDirection = mouseDirection;

		data.bullets.push_back(b1);
		data.bullets.push_back(b2);

		if (rand() % 2 == 0) PlaySound(shootSound1);
		else PlaySound(shootSound2);


	}

#pragma endregion

#pragma region handleEnemies


	// Enemy spawn code
	if (data.enemies.size() < 25)
	{
		data.enemySpawnRate -= deltaTime;

		if (data.enemySpawnRate < 0)
		{
			data.enemySpawnRate = rand() % data.enemySpawnMod + 1;

			spawnEnemy();
			if (rand() % 3 == 0)
			{
				spawnEnemy();
				spawnEnemy();
			}

		}

	}

	// Enemy update code
	for (int i = 0; i < data.enemies.size(); i++)
	{
		// If the enemy is too far away, despawn them
		if (glm::distance(data.playerPos, data.enemies[i].position) > 4000.f)
		{
			//despawn enemy
			data.enemies.erase(data.enemies.begin() + i);
			i--;
			continue;
		}

		if (data.enemies[i].update(deltaTime, data.playerPos))
		{
			Bullet b;
			b.position = data.enemies[i].position;
			b.fireDirection = data.enemies[i].viewDirection;
			b.isEnemy = true;
			b.bulletSize = 50;
			//todo speed
			data.bullets.push_back(b);

			if (!IsSoundPlaying(shootSound3)) PlaySound(shootSound3);
		}
	}

#pragma endregion

#pragma region handleBullets


	// For all bullets
	for (int i = 0; i < data.bullets.size(); i++)
	{
		// Despawn if too far
		if (glm::distance(data.bullets[i].position, data.playerPos) > 2000)
		{
			data.bullets.erase(data.bullets.begin() + i);
			i--;
			continue;
		}

		// If it is the player's bullet
		if (!data.bullets[i].isEnemy)
		{

			bool breakBothLoops = false;
			for (int e = 0; e < data.enemies.size(); e++)
			{
				// Check for collision with enemy. If hits, deal damage.
				if (bulletCollision(data.bullets[i].position, data.enemies[e].position, enemyShipSize))
				{
					data.enemies[e].life -= 0.1;

					// Check enemy health.
					if (data.enemies[e].life <= 0)
					{
						// Kill enemy and reward player
						data.enemies.erase(data.enemies.begin() + e);
						data.score += 10;
						data.playerHealth += .1f;
					}

					// Consume bullet
					data.bullets.erase(data.bullets.begin() + i);
					i--;
					breakBothLoops = true;
					continue;

				}
			}
			if (breakBothLoops)
			{
				continue;
			}
		}

		// If it is an enemy bullet
		else
		{
			// Check for collision with player. If hits, deal damage.
			if (bulletCollision(data.bullets[i].position, data.playerPos, data.shipSize))
			{

				data.playerHealth -= 0.1;

				// Consume bullet

				data.bullets.erase(data.bullets.begin() + i);
				i--;
				continue;
			}
		}

		// If no collision, update
		data.bullets[i].update(deltaTime);
	}

	// Check player health
	if (data.playerHealth <= 0)
	{
		// Send player back to menu
		gameIsRunning = false;
	}
	else
	{
		// Regenerate health
		data.playerHealth += deltaTime * 0.01;
		data.playerHealth = glm::clamp(data.playerHealth, 0.f, 1.f);
	}



#pragma endregion

#pragma region renderEnemies

	for (auto& e : data.enemies)
	{
		e.render(renderer, spaceShipsTexture, spaceShipsAtlas);
	}

#pragma endregion

#pragma region renderShips





	// Render Player 
	renderSpaceShip(renderer, data.playerPos, data.shipSize,
		playerTexture, playersAtlas.get(0, 0), mouseDirection);


#pragma endregion

#pragma region render bullets

	for (auto& b : data.bullets)
	{
		b.render(renderer, bulletsTexture, bulletsAtlas);
	}

#pragma endregion

#pragma region UI

	renderer.pushCamera();
	{

		glui::Frame f({ 0,0, w, h });

		glui::Box healthBox = glui::Box().xLeftPerc(0.f).yTopPerc(0.9).
			xDimensionPercentage(0.3).yAspectRatio(1.f / 8.f);

		renderer.renderRectangle(healthBox, healthBar);

		glm::vec4 newRect = healthBox();
		newRect.z *= data.playerHealth;

		glm::vec4 textCoords = { 0,1,1,0 };
		textCoords.z *= data.playerHealth;

		renderer.renderRectangle(newRect, health, Colors_White, {}, {},
			textCoords);

	}
	renderer.popCamera();

#pragma endregion

}

bool gameLogic(float deltaTime)
{
#pragma region initView
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);
#pragma endregion

	if (gameIsRunning)
	{
		gameplay(deltaTime, w, h);

	}
	else
	{
		StopSound(gameplaySound);

		uiRenderer.Begin(1);

		renderer.currentCamera.zoom = 0.3;
		tiledRenderer[0].render(renderer);

		if (uiRenderer.Button("Play", Colors_White, button))
		{
			gameIsRunning = true;
			restartGame();
		}

		uiRenderer.BeginMenu("Settings (WIP)", Colors_White, button);


		static float sound = 0;
		uiRenderer.sliderFloat("Example Slider: Sound", &sound, 0, 100);

		/*static bool toggle = 1;
		uiRenderer.newColum(1);
		uiRenderer.Toggle("##noName", Colors_White, &toggle, button);*/


		uiRenderer.EndMenu();

		uiRenderer.End();

		uiRenderer.renderFrame(renderer, font, platform::getRelMousePosition(),
			platform::isLMousePressed,platform::isLMouseHeld, platform::isLMouseReleased(),
			platform::isButtonReleased(platform::Button::Escape), platform::getTypedInput(), deltaTime);

		

	}

	
	// flush the renderer clean
	renderer.flush();


	//ImGui::ShowDemoWindow();

	ImGui::Begin("debug");

	ImGui::Text("Bullet count: %d", (int)data.bullets.size());
	ImGui::Text("Enemies count: %d", (int)data.enemies.size());

	if (ImGui::Button("Spawn enemy"))
	{
		spawnEnemy();
	}

	if (ImGui::Button("Reset game"))
	{
		restartGame();
	}


	ImGui::SliderInt("Spawn Rate (Lower means faster)", &data.enemySpawnMod, 1, 10);

	ImGui::SliderFloat("Player Health", &data.playerHealth, 0, 1);

	ImGui::End();

	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{



}

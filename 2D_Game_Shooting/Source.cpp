
#include <iostream>
#include <string>
#include <algorithm>
 //using namespace std;

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Header.h"


OneLoneCoder_Asteroids::OneLoneCoder_Asteroids()
{
	sAppName = "Asteroids";
}

	// Override from Game Engine
	bool OneLoneCoder_Asteroids::OnUserCreate()
	{
		vecModelShip =
		{
			{ 0.0f, -5.0f},
			{-2.5f, +2.5f},
			{+2.5f, +2.5f}
		}; // Isoceles Triangle

		// Create a "jagged" circle for the asteroid. It's important it remains
		// mostly circular, as we do a simple collision check against a perfect
		// circle.
		int verts = 20;
		for (int i = 0; i < verts; i++)
		{
			float noise = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;
			vecModelAsteroid.push_back(std::make_pair(noise * sinf(((float)i / (float)verts) * 6.28318f),
				noise * cosf(((float)i / (float)verts) * 6.28318f)));
		}

		for (int i = 0; i < 360; i += 15) // 15-degree separation between lines
		{
			for (int j = -90; j <= 90; j += 10) // Latitude points for each line
			{
				float x = 50 * cosf(i * 3.14159f / 180.0f);
				float y = 50 * sinf(i * 3.14159f / 180.0f) * cosf(j * 3.14159f / 180.0f);
				vecEarthModel.push_back({ x, y });
			}
		}

		ResetGame();

		return true;
	}

	void OneLoneCoder_Asteroids::ResetGame()
	{
		// Initialize Player Position
		player.x = ScreenWidth() / 2.0f;
		player.y = ScreenHeight() / 2.0f;
		player.dx = 0.0f;
		player.dy = 0.0f;
		player.angle = 0.0f;

		// Initialize Enemy Position
		enemy.x = ScreenWidth() / 3.0f;
		enemy.y = ScreenHeight() / 3.0f;
		enemy.dx = 0.0f;
		enemy.dy = 0.0f;
		enemy.angle = 0.0f;

		vecBullets.clear();
		vecAsteroids.clear();

		// Put in two asteroids
		vecAsteroids.push_back({ (int)16, 20.0f, 20.0f, 8.0f, -6.0f, 0.0f });
		vecAsteroids.push_back({ (int)16, 100.0f, 20.0f, -5.0f, 3.0f, 0.0f });

		// Reset game
		bDead = false;
		nScore = false;
	}

	// Implements "wrap around" for various in-game sytems
	void OneLoneCoder_Asteroids::WrapCoordinates(float ix, float iy, float& ox, float& oy)
	{
		ox = ix;
		oy = iy;
		if (ix < 0.0f)	ox = ix + (float)ScreenWidth();
		if (ix >= (float)ScreenWidth())	ox = ix - (float)ScreenWidth();
		if (iy < 0.0f)	oy = iy + (float)ScreenHeight();
		if (iy >= (float)ScreenHeight()) oy = iy - (float)ScreenHeight();
	}

	// Overriden to handle toroidal drawing routines
	bool OneLoneCoder_Asteroids::Draw(int x, int y, olc::Pixel col)
	{
		float fx, fy;
		WrapCoordinates(x, y, fx, fy);
		return olc::PixelGameEngine::Draw(fx, fy, col);
	}

	// Checking if point is inside circle
	bool OneLoneCoder_Asteroids::IsPointInsideCircle(float cx, float cy, float radius, float x, float y)
	{
		return (x - cx) * (x - cx) + (y - cy) * (y - cy) < radius * radius;
	}

	// Called by olcConsoleGameEngine
	bool OneLoneCoder_Asteroids::OnUserUpdate(float fElapsedTime)
	{
		if (bDead)
			ResetGame();

		// Clear Screen
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

		// Steer Ship
		if (GetKey(olc::Key::A).bHeld)
			player.angle -= 5.0f * fElapsedTime;
		if (GetKey(olc::Key::D).bHeld)
			player.angle += 5.0f * fElapsedTime;

		// Thrust? Apply ACCELERATION
		if (GetKey(olc::Key::W).bHeld)
		{
			// ACCELERATION changes VELOCITY (with respect to time)
			player.dx += sin(player.angle) * 20.0f * fElapsedTime;
			player.dy += -cos(player.angle) * 20.0f * fElapsedTime;
		}

		// VELOCITY changes POSITION (with respect to time)
		player.x += player.dx * fElapsedTime;
		player.y += player.dy * fElapsedTime;

		// Keep ship in gamespace
		WrapCoordinates(player.x, player.y, player.x, player.y);

		// Check ship collision with asteroids
		for (auto& a : vecAsteroids)
			if (IsPointInsideCircle(a.x, a.y, a.nSize, player.x, player.y))
				bDead = true; 

		// Fire Bullet in direction of player
		if (GetMouse(0).bPressed)
			vecBullets.push_back({ 0, player.x, player.y, 50.0f * sinf(player.angle), -50.0f * cosf(player.angle), 100.0f });

		// Update and draw asteroids
		for (auto& a : vecAsteroids)
		{
			// VELOCITY changes POSITION (with respect to time)
			a.x += a.dx * fElapsedTime;
			a.y += a.dy * fElapsedTime;
			a.angle += 0.5f * fElapsedTime; // Add swanky rotation :)

			// Asteroid coordinates are kept in game space (toroidal mapping)
			WrapCoordinates(a.x, a.y, a.x, a.y);

			// Draw Asteroids
			DrawWireFrameModel(vecModelAsteroid, a.x, a.y, a.angle, (float)a.nSize, olc::YELLOW);
		}

		// Any new asteroids created after collision detection are stored
		// in a temporary vector, so we don't interfere with the asteroids
		// vector iterator in the for(auto)
		std::vector<sSpaceObject> newAsteroids;

		// Update Bullets
		for (auto& b : vecBullets)
		{
			b.x += b.dx * fElapsedTime;
			b.y += b.dy * fElapsedTime;
			WrapCoordinates(b.x, b.y, b.x, b.y);
			b.angle -= 1.0f * fElapsedTime;

			// Check collision with asteroids
			for (auto& a : vecAsteroids)
			{
				//if (IsPointInsideRectangle(a.x, a.y, a.x + a.nSize, a.y + a.nSize, b.x, b.y))
				if (IsPointInsideCircle(a.x, a.y, a.nSize, b.x, b.y))
				{
					// Asteroid Hit - Remove bullet
					// We've already updated the bullets, so force bullet to be offscreen
					// so it is cleaned up by the removal algorithm.
					b.x = -100;

					// Create child asteroids
					if (a.nSize > 4)
					{
						float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						newAsteroids.push_back({ (int)a.nSize >> 1 ,a.x, a.y, 10.0f * sinf(angle1), 10.0f * cosf(angle1), 0.0f });
						newAsteroids.push_back({ (int)a.nSize >> 1 ,a.x, a.y, 10.0f * sinf(angle2), 10.0f * cosf(angle2), 0.0f });
					}

					// Remove asteroid - Same approach as bullets
					a.x = -100;
					nScore += 100; // Small score increase for hitting asteroid
				}
			}
		}

		// Append new asteroids to existing vector
		for (auto a : newAsteroids)
			vecAsteroids.push_back(a);

		// Clear up dead objects - They are out of game space

		// Remove asteroids that have been blown up
		if (vecAsteroids.size() > 0)
		{
			auto i = remove_if(vecAsteroids.begin(), vecAsteroids.end(), [&](sSpaceObject o) { return (o.x < 0); });
			if (i != vecAsteroids.end())
				vecAsteroids.erase(i);
		}

		if (vecAsteroids.empty()) // If no asteroids, level complete! :) - you win MORE asteroids!
		{
			// Level Clear
			nScore += 1000; // Large score for level progression
			vecAsteroids.clear();
			vecBullets.clear();

			// Add two new asteroids, but in a place where the player is not, we'll simply
			// add them 90 degrees left and right to the player, their coordinates will
			// be wrapped by th enext asteroid update
			vecAsteroids.push_back({ (int)16, 30.0f * sinf(player.angle - 3.14159f / 2.0f) + player.x,
											  30.0f * cosf(player.angle - 3.14159f / 2.0f) + player.y,
											  10.0f * sinf(player.angle), 10.0f * cosf(player.angle), 0.0f });

			vecAsteroids.push_back({ (int)16, 30.0f * sinf(player.angle + 3.14159f / 2.0f) + player.x,
											  30.0f * cosf(player.angle + 3.14159f / 2.0f) + player.y,
											  10.0f * sinf(-player.angle), 10.0f * cosf(-player.angle), 0.0f });
		}

		// Remove bullets that have gone off screen
		if (vecBullets.size() > 0)
		{
			auto i = remove_if(vecBullets.begin(), vecBullets.end(), [&](sSpaceObject o) { return (o.x < 1 || o.y < 1 || o.x >= ScreenWidth() - 1 || o.y >= ScreenHeight() - 1); });
			if (i != vecBullets.end())
				vecBullets.erase(i);
		}

		// Draw Bullets
		for (auto b : vecBullets)
			Draw(b.x, b.y);

		// Draw Ship
		DrawWireFrameModel(vecModelShip, player.x, player.y, player.angle, 1.0f);

		// Draw Enemy
		DrawWireFrameModel(vecModelShip, enemy.x, enemy.y, enemy.angle + 1.5708f, 1.0f, olc::RED); // Draw enemy in red for distinction

		// Draw Score
		DrawString(2, 2, "SCORE: " + std::to_string(nScore));

		// Gravity
		for (auto& a : vecAsteroids)
		{
			// Calculate distance between player and asteroid
			float dx = a.x - player.x;
			float dy = a.y - player.y;
			float dist = sqrt(dx * dx + dy * dy);
			float distSquared = dist * dist + 1.0f;  // Adding 1 to avoid division by zero

			if (dist < a.nSize * 20) // Gravity only has an effect within 10 times the asteroid's size
			{
				// Calculate gravitational force (simplified)
				float gravityForce = (a.nSize / distSquared) * 1000.0f;  // We multiply by 50 just as a scaling factor to make the force noticeable

				// Calculate acceleration components
				float ax = gravityForce * (dx / dist);
				float ay = gravityForce * (dy / dist);

				// Apply acceleration to player's velocity
				player.dx += ax * fElapsedTime;
				player.dy += ay * fElapsedTime;
			}
		}

		player.x += player.dx * fElapsedTime;
		player.y += player.dy * fElapsedTime;

		// Enemy

		float nDiffX = player.x - enemy.x;
		float nDiffY = player.y - enemy.y;
		enemy.angle = atan2f(nDiffY, nDiffX);

		const float enemySpeed = 20.0f; // Enemy's speed
		enemy.dx = enemySpeed * cosf(enemy.angle);
		enemy.dy = enemySpeed * sinf(enemy.angle);

		enemy.x += enemy.dx * fElapsedTime;
		enemy.y += enemy.dy * fElapsedTime;

		WrapCoordinates(enemy.x, enemy.y, enemy.x, enemy.y);

		//if (IsPointInsideCircle(enemy.x, enemy.y, 5.0f, player.x, player.y))
		//	bDead = true;

		// Draw Earth as a circle
		FillCircle(ScreenWidth() / 2, ScreenHeight() / 2, 50, olc::BLUE);

		// Draw countries based on current rotation
		for (const auto& country : mapCountries) {
			float x, y;
			if (LongitudeToX(country.second.first, country.second.second, fRotation, x, y)) {
				olc::Pixel color = countryColors[country.first];

				// If there are more than 2 vertices, we can form triangles
				if (countryShapes[country.first].size() > 2) {
					for (size_t i = 0; i < countryShapes[country.first].size() - 2; i++) {
						float x0, y0, x1, y1, x2, y2;

						// Fetch the vertices for a triangle
						bool bV0 = LongitudeToX(country.second.first + countryShapes[country.first][0].first * fCountryScale,
							country.second.second + countryShapes[country.first][0].second * fCountryScale,
							fRotation, x0, y0);

						bool bV1 = LongitudeToX(country.second.first + countryShapes[country.first][i + 1].first * fCountryScale,
							country.second.second + countryShapes[country.first][i + 1].second * fCountryScale,
							fRotation, x1, y1);

						bool bV2 = LongitudeToX(country.second.first + countryShapes[country.first][i + 2].first * fCountryScale,
							country.second.second + countryShapes[country.first][i + 2].second * fCountryScale,
							fRotation, x2, y2);

						if (bV0 && bV1 && bV2 &&
							IsPointInsideCircle(ScreenWidth() / 2, ScreenHeight() / 2, 50, x0, y0) &&
							IsPointInsideCircle(ScreenWidth() / 2, ScreenHeight() / 2, 50, x1, y1) &&
							IsPointInsideCircle(ScreenWidth() / 2, ScreenHeight() / 2, 50, x2, y2)) {

							// Fill the country with a triangle
							FillTriangle(x0, y0, x1, y1, x2, y2, color);
						}
					}
				}
			}
		}

		// Keep fRotation between 0 and 2*PI
		fRotation += fSpeed;
		if (fRotation > 2.0f * 3.14159f)
			fRotation -= 2.0f * 3.14159f;


		return true;
	}

	void OneLoneCoder_Asteroids::DrawWireFrameModel(const std::vector<std::pair<float, float>>& vecModelCoordinates, float x, float y, float r, float s, olc::Pixel col)
	{
		// pair.first = x coordinate
		// pair.second = y coordinate

		// Create translated model vector of coordinate pairs
		std::vector<std::pair<float, float>> vecTransformedCoordinates;
		int verts = vecModelCoordinates.size();
		vecTransformedCoordinates.resize(verts);

		// Rotate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecModelCoordinates[i].first * cosf(r) - vecModelCoordinates[i].second * sinf(r);
			vecTransformedCoordinates[i].second = vecModelCoordinates[i].first * sinf(r) + vecModelCoordinates[i].second * cosf(r);
		}

		// Scale
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first * s;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second * s;
		}

		// Translate
		for (int i = 0; i < verts; i++)
		{
			vecTransformedCoordinates[i].first = vecTransformedCoordinates[i].first + x;
			vecTransformedCoordinates[i].second = vecTransformedCoordinates[i].second + y;
		}

		// Draw Closed Polygon
		
		for (int i = 0; i < verts; i++)
		{
			int j = (i + 1) % verts;
			DrawLine(vecTransformedCoordinates[i].first, vecTransformedCoordinates[i].second,
				vecTransformedCoordinates[j].first, vecTransformedCoordinates[j].second, col);
		}

		/* WASN'T WORKING PROPERLY
		for (int i = 0; i < verts + 1; i++)
		{
			int j = (i + 1);
			DrawLine(vecTransformedCoordinates[i % verts].first, vecTransformedCoordinates[i % verts].second,
				vecTransformedCoordinates[j % verts].first, vecTransformedCoordinates[j % verts].second, col);
		}
		*/ 
		
	}



int main()
{
	// Use olcPixelGameEngine derived app
	OneLoneCoder_Asteroids game;
	if (game.Construct(320, 200, 4, 4))
		game.Start();
	return 0;
}
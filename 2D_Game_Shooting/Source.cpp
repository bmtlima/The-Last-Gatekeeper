
#include <iostream>
#include <string>
#include <algorithm>

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
			{ 0.0f, -7.0f },
			{ -3.0f, +4.0f },
			{ 0.0f, +2.5f },
			{ +3.0f, +4.0f }
		};

		// Creating a "jagged" circle for the asteroid. 
		int verts = 30;
		for (int i = 0; i < verts; i++)
		{
			float noise = (float)rand() / (float)RAND_MAX * 0.4f + 0.8f;
			vecModelAsteroid.push_back(std::make_pair(noise * sinf(((float)i / (float)verts) * 6.28318f),
				noise * cosf(((float)i / (float)verts) * 6.28318f)));
		}

		int nStars = 100; // Number of stars in background
		for (int i = 0; i < nStars; i++) {
			sStar star;
			star.x = (float)(rand() % ScreenWidth());
			star.y = (float)(rand() % ScreenHeight());
			float brightness = (rand() % 255) * 0.8f + 0.2f * 255; // This makes sure the star is not too dim
			star.color = olc::Pixel(brightness, brightness, brightness);
			vecStars.push_back(star);
		}

		ResetGame();

		return true;
	}

	void OneLoneCoder_Asteroids::ResetGame()
	{
		nLevel = 1;

		// Initialize Player Position
		player.x = ScreenWidth() / 2.0f;
		player.y = ScreenHeight() * 3.0f / 4.0f;
		player.dx = 0.0f;
		player.dy = 0.0f;
		player.angle = 0.0f;

		vecBullets.clear();
		vecAsteroids.clear();

		// Put in two asteroids
		vecAsteroids.push_back({ (int)16, ScreenWidth() - 20.0f, ScreenHeight() - 20.0f, 8.0f, -6.0f, 0.0f });
		vecAsteroids.push_back({ (int)16, ScreenWidth() - 100.0f, ScreenHeight() - 20.0f, -5.0f, 3.0f, 0.0f });

		if (nLevel > 1) {
			for (int i = 0; i < nLevel + 1; i++) {
				float ax, ay;
				do {
					ax = (float)(rand() % ScreenWidth());
					ay = (float)(rand() % ScreenHeight());
				} while (sqrt((player.x - ax) * (player.x - ax) + (player.y - ay) * (player.y - ay)) < 50); // Check distance from player

				float adx = 2.0f * (((float)rand() / RAND_MAX) - 0.5f); // Random velocities 
				float ady = 2.0f * (((float)rand() / RAND_MAX) - 0.5f);

				vecAsteroids.push_back({ (int)16, ax, ay, adx, ady, 0.0f });
			}
		}

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

		// Render Stars
		for (const auto& star : vecStars) {
			Draw((int)star.x, (int)star.y, star.color);
		}

		// Draw Earth as a circle
		FillCircle(ScreenWidth() / 2, ScreenHeight() / 2, EARTH_RADIUS, olc::BLUE);

		// Steer Ship
		if (GetKey(olc::Key::A).bHeld)
			player.angle -= 5.0f * fElapsedTime;
		if (GetKey(olc::Key::D).bHeld)
			player.angle += 5.0f * fElapsedTime;

		// Acceleration
		if (GetKey(olc::Key::W).bHeld)
		{
			player.dx += sin(player.angle) * 20.0f * fElapsedTime;
			player.dy += -cos(player.angle) * 20.0f * fElapsedTime;
		}

		// Velocity
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

			// Calculate direction towards the center of the screen
			float dirX = (ScreenWidth() * 0.5f - a.x);
			float dirY = (ScreenHeight() * 0.5f - a.y);
			float magnitude = sqrt(dirX * dirX + dirY * dirY);

			// Normalize the direction
			if (magnitude != 0)
			{
				dirX /= magnitude;
				dirY /= magnitude;
			}

			// Speed of asteroid
			float speed = 5.0f; 

			// Update asteroid's velocity based on direction
			a.dx = dirX * speed;
			a.dy = dirY * speed;

			// Updating positions
			a.x += a.dx * fElapsedTime;
			a.y += a.dy * fElapsedTime;
			a.angle += 0.5f * fElapsedTime;

			// Asteroid coordinates are kept in game space (toroidal mapping)
			WrapCoordinates(a.x, a.y, a.x, a.y);

			// Drawing Asteroids
			DrawWireFrameModel(vecModelAsteroid, a.x, a.y, a.angle, (float)a.nSize, olc::CYAN);
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

					for (int n = 0; n < 10; n++) // Generate 10 debris particles
					{
						vecAsteroidDebris.push_back({
							a.x, a.y,
							2.0f * (float(rand()) / float(RAND_MAX) - 0.5f),
							2.0f * (float(rand()) / float(RAND_MAX) - 0.5f),
							float(rand()) / float(RAND_MAX) * 2.0f * 3.14159f,
							olc::Pixel(255, 255, 0, 255) // Yellow with full alpha
							});
					}

					// Create child asteroids
					if (a.nSize > 4)
					{
						float angle1 = ((float)rand() / (float)RAND_MAX) * 6.283185f;
						float angle2 = ((float)rand() / (float)RAND_MAX) * 6.283185f;

						float offsetDistance = a.nSize; // Set an offset distance based on the size of the asteroid
						newAsteroids.push_back({ (int)a.nSize >> 1 ,a.x + offsetDistance * sinf(angle1), a.y + offsetDistance * cosf(angle1), 10.0f * sinf(angle1), 10.0f * cosf(angle1), 0.0f });
						newAsteroids.push_back({ (int)a.nSize >> 1 ,a.x + offsetDistance * sinf(angle2), a.y + offsetDistance * cosf(angle2), 10.0f * sinf(angle2), 10.0f * cosf(angle2), 0.0f });
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

		if (vecAsteroids.empty()) // If no asteroids, level complete. Generate more asteroids
		{
			// Level Clear
			nScore += 1000; // Large score for level progression
			vecAsteroids.clear();
			vecBullets.clear();
			nLevel++;

			// Spawning asteroids near the borders
			for (int i = 0; i < nLevel + 1; i++) {
				float ax, ay;

				int borderChoice = rand() % 4; // Choose a border (0:Top, 1:Right, 2:Bottom, 3:Left)

				switch (borderChoice) {
				case 0: // Top
					ax = (float)(rand() % ScreenWidth());
					ay = 0.0f;
					break;
				case 1: // Right
					ax = ScreenWidth() - 1.0f;
					ay = (float)(rand() % ScreenHeight());
					break;
				case 2: // Bottom
					ax = (float)(rand() % ScreenWidth());
					ay = ScreenHeight() - 1.0f;
					break;
				case 3: // Left
				default:
					ax = 0.0f;
					ay = (float)(rand() % ScreenHeight());
					break;
				}

				float adx = 2.0f * (((float)rand() / RAND_MAX) - 0.5f); // Random velocities
				float ady = 2.0f * (((float)rand() / RAND_MAX) - 0.5f);

				vecAsteroids.push_back({ (int)16, ax, ay, adx, ady, 0.0f });
			}
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
			//Draw(b.x, b.y);
			DrawLine(b.x, b.y, b.x + b.dx * 0.05, b.y + b.dy * 0.05, olc::RED);  // Change '0.1' to adjust the length of the bullet line.

		// Draw Ship
		DrawWireFrameModel(vecModelShip, player.x, player.y, player.angle, 1.0f);;

		// Rendering Score & Title
		DrawString(ScreenWidth()/2 - 30, ScreenHeight() - 15, "SCORE: " + std::to_string(nScore));
		DrawString(10, 10, "The Last Gatekeeper", olc::YELLOW, 2); // Displaying the title

		// Gravity
		for (auto& a : vecAsteroids)
		{
			// Calculate distance between player and asteroid
			float dx = a.x - player.x;
			float dy = a.y - player.y;
			float dist = sqrt(dx * dx + dy * dy);
			float distSquared = dist * dist + 1.0f;  // Adding 1 to avoid division by zero

			if (dist < a.nSize * 15) // Gravity only has an effect within 10 times the asteroid's size
			{
				// Calculate gravitational force (simplified)
				float gravityForce = (a.nSize / distSquared) * 300.0f;  // We multiply by 50 just as a scaling factor to make the force noticeable

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


		// Draw countries based on current rotation
		DrawRotatingEarth(fRotation);

		// Keep fRotation between 0 and 2*PI
		fRotation += fSpeed;
		if (fRotation > 2.0f * 3.14159f)
			fRotation -= 2.0f * 3.14159f;

		// Player dies if any asteroid gets too close to earth

		for (auto& a : vecAsteroids)
		{
				// Check distance from asteroid to center of screen
				float distToCenter = sqrt((a.x - ScreenWidth() / 2.0f) * (a.x - ScreenWidth() / 2.0f) + (a.y - ScreenHeight() / 2.0f) * (a.y - ScreenHeight() / 2.0f));
			if (distToCenter < EARTH_RADIUS + a.nSize)
				bDead = true;
		}

		for (auto& d : vecAsteroidDebris)
		{
			d.x += d.dx * fElapsedTime;
			d.y += d.dy * fElapsedTime;
			d.angle += 2.0f * fElapsedTime; // Rotate the debris
			d.color.a -= 30 * fElapsedTime; // Reduce alpha for fading effect

			// Wrap coordinates to keep debris on screen
			WrapCoordinates(d.x, d.y, d.x, d.y);

			// Draw debris as tiny triangles
			DrawWireFrameModel(vecModelShip, d.x, d.y, d.angle, 1.0f, d.color);
		}

		vecAsteroidDebris.erase(
			remove_if(vecAsteroidDebris.begin(), vecAsteroidDebris.end(),
				[&](sAsteroidDebris& d) { return d.color.a <= 0; }),
			vecAsteroidDebris.end());

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
		
	}

// other functions

	float OneLoneCoder_Asteroids::DegreeToRadian(float degree) {
		return degree * 3.14159f / 180.0f;
	}

	// Convert latitude to y-position with respect to the screen height
	float OneLoneCoder_Asteroids::LatitudeToY(float latitude) {
		float yRange = ScreenHeight() / 4;
		return ScreenHeight() / 2 - yRange * (latitude / 90.0f);
	}

	bool OneLoneCoder_Asteroids::LongitudeToX(float longitude, float latitude, float rotation, float& outX, float& outY) {
		float radianLongitude = DegreeToRadian(longitude) + rotation;

		// Handle wrap-around.
		while (radianLongitude < -PI)
			radianLongitude += 2.0f * PI;
		while (radianLongitude > PI)
			radianLongitude -= 2.0f * PI;

		outX = EARTH_RADIUS * cosf(radianLongitude) + ScreenWidth() / 2;
		outY = LatitudeToY(latitude);

		return cosf(radianLongitude + PI / 2) > 0;
	}

	void OneLoneCoder_Asteroids::DrawRotatingEarth(float rotation) {
		// Draw Earth with smooth shading to represent the atmosphere
		for (int y = 0; y < ScreenHeight(); y++) {
			for (int x = 0; x < ScreenWidth(); x++) {
				float distance = sqrt((x - ScreenWidth() / 2) * (x - ScreenWidth() / 2) + (y - ScreenHeight() / 2) * (y - ScreenHeight() / 2));
				if (distance < EARTH_RADIUS + ATMOSPHERE && distance > EARTH_RADIUS - ATMOSPHERE) {
					float alpha = 1.0f - abs(distance - EARTH_RADIUS) / ATMOSPHERE;
					Draw(x, y, olc::Pixel(135, 206, 235, alpha * 255)); // SkyBlue color
				}
				else if (distance <= EARTH_RADIUS) {
					Draw(x, y, olc::BLUE);
				}
			}
		}

		for (const auto& country : mapCountries) {
			float x, y;
			if (LongitudeToX(country.second.first, country.second.second, rotation, x, y)) {
				olc::Pixel color = countryColors[country.first];

				if (countryShapes[country.first].size() > 2) {
					for (size_t i = 0; i < countryShapes[country.first].size() - 2; i++) {
						float x0, y0, x1, y1, x2, y2;
						bool bV0 = LongitudeToX(country.second.first + countryShapes[country.first][0].first * fCountryScale,
							country.second.second + countryShapes[country.first][0].second * fCountryScale, rotation, x0, y0);
						bool bV1 = LongitudeToX(country.second.first + countryShapes[country.first][i + 1].first * fCountryScale,
							country.second.second + countryShapes[country.first][i + 1].second * fCountryScale, rotation, x1, y1);
						bool bV2 = LongitudeToX(country.second.first + countryShapes[country.first][i + 2].first * fCountryScale,
							country.second.second + countryShapes[country.first][i + 2].second * fCountryScale, rotation, x2, y2);

						if (bV0 && bV1 && bV2 &&
							IsPointInsideCircle(ScreenWidth() / 2, ScreenHeight() / 2, EARTH_RADIUS - ATMOSPHERE, x0, y0) &&
							IsPointInsideCircle(ScreenWidth() / 2, ScreenHeight() / 2, EARTH_RADIUS - ATMOSPHERE, x1, y1) &&
							IsPointInsideCircle(ScreenWidth() / 2, ScreenHeight() / 2, EARTH_RADIUS - ATMOSPHERE, x2, y2)) {
							FillTriangle(x0, y0, x1, y1, x2, y2, color);
						}
					}
				}
			}
		}
	}

int main()
{
	// Use olcPixelGameEngine derived app
	OneLoneCoder_Asteroids game;
	if (game.Construct(320, 200, 4, 4))
		game.Start();
	return 0;
}
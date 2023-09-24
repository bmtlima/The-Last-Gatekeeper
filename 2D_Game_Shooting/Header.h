
#ifndef ASTEROIDS_HEADER_H
#define ASTEROIDS_HEADER_H

#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <map>
#include <vector>
#include "olcPixelGameEngine.h"

class OneLoneCoder_Asteroids : public olc::PixelGameEngine
{
public:
    OneLoneCoder_Asteroids();

    bool OnUserCreate() override;

    void ResetGame();

    void WrapCoordinates(float ix, float iy, float& ox, float& oy);

    bool Draw(int x, int y, olc::Pixel col = olc::WHITE) override;

    bool IsPointInsideCircle(float cx, float cy, float radius, float x, float y);

    bool OnUserUpdate(float fElapsedTime) override;

    void DrawWireFrameModel(const std::vector<std::pair<float, float>>& vecModelCoordinates, float x, float y, float r = 0.0f, float s = 1.0f, olc::Pixel col = olc::WHITE);

    float DegreeToRadian(float degree);

    float LatitudeToY(float latitude);

    bool LongitudeToX(float longitude, float latitude, float rotation, float& outX, float& outY);

    void DrawRotatingEarth(float rotation);

    float GetPI() const { return PI; }

private:
    struct sSpaceObject
    {
        int nSize;
        float x, y, dx, dy, angle;
    };

    struct sAsteroidDebris
    {
        float x, y;   // Position
        float dx, dy; // Velocity
        float angle;  // Rotation
        olc::Pixel color; // Color with alpha for fading effect
    };

    struct sStar {
        float x, y;       // Position of star
        olc::Pixel color; // Color of star 
    };

    std::vector<sStar> vecStars;
    std::vector<sAsteroidDebris> vecAsteroidDebris;

    std::vector<sSpaceObject> vecAsteroids;
    std::vector<sSpaceObject> vecBullets;
    sSpaceObject player;
    //sSpaceObject enemy;
    bool bDead = false;
    int nScore = 0;
    int nLevel = 1;

    std::vector<std::pair<float, float>> vecModelShip;
    std::vector<std::pair<float, float>> vecModelAsteroid;

    std::vector<std::pair<float, float>> vecEarthModel;

    float fRotation = 0.0f;   // Rotation angle in radians
    float fSpeed = 0.005f;     // Rotation speed in radians per frame
    float fCountryScale = 4.0f;  // Scale up countries by a factor of 2

    const float PI = 3.14159f;
    const float EARTH_RADIUS = 30.0f;
    const float ATMOSPHERE = 4.0f; // Thickness of Earth's atmosphere for shading

    using Point = std::pair<float, float>;
    using Country = std::map<std::string, std::vector<Point>>;

    // A few example countries/landmarks and their longitudinal and latitudinal positions in degrees
    std::map<std::string, std::pair<float, float>> mapCountries = {
        {"US", {-98.0f, 37.0f}},
        {"Brazil", {-53.0f, -10.0f}},
        {"UK", {0.0f, 51.5f}},
        {"India", {78.0f, 20.5f}},
        {"Japan", {138.0f, 36.0f}},
        {"Australia", {133.0f, -25.5f}},
        {"Russia", {90.0f, 60.0f}},
        {"South Africa", {24.0f, -30.0f}}
    };

    std::map<std::string, std::vector<std::pair<float, float>>> countryShapes = {
    {"US", {{-3, 2}, {3, 2}, {2, 1}, {3, 0}, {0, -3}, {-3, 0}, {-2, 1}}},
    {"Brazil", {{-2, -1}, {2, -1}, {1, 2}, {-1, 2}}},
    {"UK", {{-1, 0.5}, {1, 0.5}, {0.5, -0.5}, {-0.5, -0.5}}},
    {"India", {{-2, 1}, {2, 1}, {1, -2}, {-1, -2}}},
    {"Japan", {{-0.5, 1.5}, {0.5, 1.5}, {0.5, -1.5}, {-0.5, -1.5}}},
    {"Australia", {{-2, -1}, {2, -1}, {1, 2}, {-1, 2}}},
    {"Russia", {{-4, 3}, {4, 3}, {3, -3}, {-3, -3}}},
    {"South Africa", {{-2, 1}, {2, 1}, {0, -2}}},
    };

    std::map<std::string, olc::Pixel> countryColors = {
    {"US", olc::GREEN},
    {"Brazil", olc::GREEN},
    {"UK", olc::GREEN},
    {"India", olc::GREEN},
    {"Japan", olc::GREEN},
    {"Australia", olc::GREEN},
    {"Russia", olc::GREEN},
    {"South Africa", olc::GREEN},
    };

 
};

#endif
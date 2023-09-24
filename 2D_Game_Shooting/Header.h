
#ifndef ASTEROIDS_HEADER_H
#define ASTEROIDS_HEADER_H

#include <iostream>
#include <string>
#include <algorithm>
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


private:
    struct sSpaceObject
    {
        int nSize;
        float x, y, dx, dy, angle;
    };

    std::vector<sSpaceObject> vecAsteroids;
    std::vector<sSpaceObject> vecBullets;
    sSpaceObject player;
    sSpaceObject enemy;
    bool bDead = false;
    int nScore = 0;

    std::vector<std::pair<float, float>> vecModelShip;
    std::vector<std::pair<float, float>> vecModelAsteroid;

    std::vector<std::pair<float, float>> vecEarthModel;

    float fRotation = 0.0f;   // Rotation angle in radians
    float fSpeed = 0.005f;     // Rotation speed in radians per frame
    float fCountryScale = 5.0f;  // Scale up countries by a factor of 2

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
    {"US", olc::RED},
    {"Brazil", olc::GREEN},
    {"UK", olc::CYAN},
    {"India", olc::YELLOW},
    {"Japan", olc::MAGENTA},
    {"Australia", olc::RED},
    {"Russia", olc::BLUE},
    {"South Africa", olc::WHITE},
    {"Antarctica", olc::GREY},
    {"Arctic", olc::GREY}
    };

    bool LongitudeToX(float longitude, float latitude, float rotation, float& outX, float& outY)
    {
        float radianLongitude = longitude * 3.14159f / 180.0f + rotation;
        float radianLatitude = latitude * 3.14159f / 180.0f;

        // Handle wrap around
        while (radianLongitude < -3.14159f)
            radianLongitude += 2.0f * 3.14159f;
        while (radianLongitude > 3.14159f)
            radianLongitude -= 2.0f * 3.14159f;

        outX = 50.0f * cosf(radianLongitude) + ScreenWidth() / 2;
        outY = LatitudeToY(latitude);

        // If the country is on the visible side of the Earth
        return cosf(radianLongitude + 3.14159f / 2) > 0;
    }

    // Convert latitude to y-position with respect to the screen height
    float LatitudeToY(float latitude) {
        float yRange = ScreenHeight() / 4; // max range in y-direction based on latitude
        return ScreenHeight() / 2 - yRange * (latitude / 90.0f);
    }
};

#endif
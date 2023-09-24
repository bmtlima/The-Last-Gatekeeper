
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
    int nLevel = 1;

    std::vector<std::pair<float, float>> vecModelShip;
    std::vector<std::pair<float, float>> vecModelAsteroid;

    std::vector<std::pair<float, float>> vecEarthModel;

    float fRotation = 0.0f;   // Rotation angle in radians
    float fSpeed = 0.005f;     // Rotation speed in radians per frame
    float fCountryScale = 2.5f;  // Scale up countries by a factor of 2

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
    {"Antarctica", olc::GREEN},
    {"Arctic", olc::GREEN}
    };

    float DegreeToRadian(float degree) {
        return degree * PI / 180.0f;
    }

    bool LongitudeToX(float longitude, float latitude, float rotation, float& outX, float& outY) {
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

    // Convert latitude to y-position with respect to the screen height
    float LatitudeToY(float latitude) {
        float yRange = ScreenHeight() / 4;
        return ScreenHeight() / 2 - yRange * (latitude / 90.0f);
    }

    std::pair<float, float> MidPoint(float x1, float y1, float x2, float y2) {
        return { (x1 + x2) / 2.0f, (y1 + y2) / 2.0f };
    }

    void DrawRotatingEarth(float rotation) {
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
};

#endif
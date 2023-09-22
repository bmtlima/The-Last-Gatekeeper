
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

};

#endif
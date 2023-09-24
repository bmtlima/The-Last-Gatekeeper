#include "pch.h"
#include "gtest/gtest.h"
#include "../2D_Game_Shooting/Header.h" 

class OneLoneCoder_AsteroidsTest : public ::testing::Test {
protected:
    OneLoneCoder_Asteroids game;

public:
    OneLoneCoder_Asteroids& GetGameInstance() { return game; }
};

TEST_F(OneLoneCoder_AsteroidsTest, TestDegreeToRadian) {
    float result = GetGameInstance().DegreeToRadian(180);
    ASSERT_FLOAT_EQ(result, GetGameInstance().GetPI());  // Using PI from Header
}

TEST_F(OneLoneCoder_AsteroidsTest, TestWrapCoordinates) {
    float ox, oy;
    game.WrapCoordinates(10, 20, ox, oy);
    ASSERT_FLOAT_EQ(ox, 10);
    ASSERT_FLOAT_EQ(oy, 20);
}

TEST_F(OneLoneCoder_AsteroidsTest, TestIsPointInsideCircle) {
    bool result = game.IsPointInsideCircle(10, 10, 5, 12, 10);
    ASSERT_TRUE(result);

    result = game.IsPointInsideCircle(10, 10, 5, 20, 10);
    ASSERT_FALSE(result);
}


TEST_F(OneLoneCoder_AsteroidsTest, TestLatitudeToY) {
    float result = game.LatitudeToY(45);
    ASSERT_FLOAT_EQ(result, 120 - (240 / 4) * (45 / 90.0f));
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



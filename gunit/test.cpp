
/*
#include "Header.h"
#include "gtest/gtest.h"

class AsteroidsTest : public ::testing::Test {
protected:
    OneLoneCoder_Asteroids asteroidsGame;
};

// Test for WrapCoordinates
TEST_F(AsteroidsTest, WrapCoordinates) {
    float ox, oy;

    asteroidsGame.WrapCoordinates(1.0f, 2.0f, ox, oy);
    // Expected values here should be based on your actual implementation of WrapCoordinates.
    EXPECT_FLOAT_EQ(ox, YOUR_EXPECTED_X_VALUE_HERE);
    EXPECT_FLOAT_EQ(oy, YOUR_EXPECTED_Y_VALUE_HERE);
}

// Test for IsPointInsideCircle
TEST_F(AsteroidsTest, PointInsideCircle) {
    EXPECT_TRUE(asteroidsGame.IsPointInsideCircle(5.0f, 5.0f, 5.0f, 5.0f, 5.0f)); // Assuming point at center is inside
    EXPECT_FALSE(asteroidsGame.IsPointInsideCircle(5.0f, 5.0f, 5.0f, 10.0f, 10.0f)); // Assuming this point is outside
}

// ... continue with other tests

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
*/

/*#include <gtest/gtest.h>
#include "../2D_Game_Shooting/Header.h"  

class GameTest : public ::testing::Test {
protected:
    OneLoneCoder_Asteroids game;
};*/
/*
TEST_F(GameTest, WrapCoordinates) {
    float outX, outY;

    game.WrapCoordinates(-5.0f, 5.0f, outX, outY);
    EXPECT_FLOAT_EQ(outX, game.ScreenWidth() - 5.0f);
    EXPECT_FLOAT_EQ(outY, 5.0f);

    game.WrapCoordinates(game.ScreenWidth() + 5.0f, game.ScreenHeight() + 10.0f, outX, outY);
    EXPECT_FLOAT_EQ(outX, 5.0f);
    EXPECT_FLOAT_EQ(outY, 10.0f);
}

TEST_F(GameTest, IsPointInsideCircle) {
    EXPECT_TRUE(game.IsPointInsideCircle(0, 0, 5, 3, 4));
    EXPECT_FALSE(game.IsPointInsideCircle(0, 0, 5, 6, 0));
}*/

/*TEST_F(GameTest, ResetGame) {
    game.ResetGame();

    EXPECT_FLOAT_EQ(game.player.x, game.ScreenWidth() / 2.0f);
    EXPECT_FLOAT_EQ(game.player.y, game.ScreenHeight() / 2.0f);
    EXPECT_FALSE(game.bDead);
    EXPECT_EQ(game.nScore, 0);
    EXPECT_TRUE(game.vecBullets.empty());
    EXPECT_EQ(game.vecAsteroids.size(), 2);
}*/

// Additional test cases can be added here.
/*
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
*/
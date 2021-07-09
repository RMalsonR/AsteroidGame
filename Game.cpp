#include "Engine.h"
#include "SpaceObjectUtils.h"
#include "LevelManager.h"
#include <memory.h>

std::vector<SpaceObject> vecAsteroids{};
std::vector<SpaceObject> vecBullets{};
std::vector<SpaceObject> vecNewAsteroids{};
std::vector<SpaceObject> vecAsteroidsToLoad{};
SpaceObject player{};

std::vector<std::pair<int, int>> vecShipModel{};
std::vector<std::pair<int, int>> vecLiveModel{};
std::vector<std::pair<int, int>> vecEngineFireModel{};
std::vector<std::pair<int, int>> vecAsteroidModel{};
float shootCounter = 0.0f;
bool isEngineUp = false;
bool isDead = false;
bool isWin = false;
bool isGameOver = false;
int currentLevel = 1;
float completeLevelScore = BEGIN_COMPLETE_LEVEL_SCORE;
float currentLevelScore = 0.0f;
float levelTime = 0.0f;
float highScore;
int livesCount;
std::vector<std::vector<std::pair<int, int>>> vecNumbersModels;
std::vector<std::pair<int, int>> vecZeroNumberModel;
std::vector<std::pair<int, int>> vecOneNumberModel;
std::vector<std::pair<int, int>> vecTwoNumberModel;
std::vector<std::pair<int, int>> vecThreeNumberModel;
std::vector<std::pair<int, int>> vecFourNumberModel;
std::vector<std::pair<int, int>> vecFiveNumberModel;
std::vector<std::pair<int, int>> vecSixNumberModel;
std::vector<std::pair<int, int>> vecSevenNumberModel;
std::vector<std::pair<int, int>> vecEightNumberModel;
std::vector<std::pair<int, int>> vecNineNumberModel;

std::vector<std::pair<int, int>> vecYCharModel;
std::vector<std::pair<int, int>> vecOCharModel;
std::vector<std::pair<int, int>> vecUCharModel;
std::vector<std::pair<int, int>> vecWCharModel;
std::vector<std::pair<int, int>> vecICharModel;
std::vector<std::pair<int, int>> vecNCharModel;
std::vector<std::pair<int, int>> vecGCharModel;
std::vector<std::pair<int, int>> vecACharModel;
std::vector<std::pair<int, int>> vecMCharModel;
std::vector<std::pair<int, int>> vecECharModel;
std::vector<std::pair<int, int>> vecVCharModel;
std::vector<std::pair<int, int>> vecRCharModel;

// initialize game data in this function
void initialize(){
    livesCount = MAX_LIVES;
    highScore = get_high_score();

    init_models();

    init_level();
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt) {

    if (is_key_pressed(VK_SPACE) && isWin || is_key_pressed(VK_SPACE) && isGameOver) {
        reset_game();
        return;
    }

    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();

    levelTime += dt;

    if (is_key_pressed(VK_LEFT)) {
        player.angle += SHIP_ANGLE_FACTOR * dt;
    }
    if (is_key_pressed(VK_RIGHT)) {
        player.angle -= SHIP_ANGLE_FACTOR * dt;
    }

    if(is_key_pressed(VK_UP)) {
        change_player_dx_dy(dt, SHIP_SPEED_FACTOR, false);
        isEngineUp = true;
    } else {
        change_player_dx_dy(dt, SHIP_SPEED_BRAKE_FACTOR, true);
        isEngineUp = false;
    }

    if(is_key_pressed(VK_SPACE))
        add_new_bullet();

    // Get away from float overflow
    if (shootCounter <= SHOOT_DELAY_SECONDS)
        shootCounter += dt;

    // Move player
    calc_space_object_coordinates(player, dt);

    // Move asteroids
    calc_space_object_coordinates(vecAsteroids, dt);

    if (isDead){
        reset_level();
        return;
    }

    // Move bullet and calc collision with asteroids
    for(auto &bullet : vecBullets){
        bullet.x += (int)(bullet.dx * dt);
        bullet.y += (int)(bullet.dy * dt);

        for(auto &asteroid : vecAsteroids){
            if (is_bullet_inside_polygon(bullet, get_object_points_coordinates(vecAsteroidModel, asteroid.x, asteroid.y, asteroid.angle, asteroid.size))){
                // set negative x, y BULLET coordinate to remove the object
                bullet.x = -10;
                bullet.y = -10;

                // add new asteroids
                add_new_asteroid(asteroid);

                // set negative x, y ASTEROID coordinate to -10 to remove the object
                asteroid.x = -10;
                asteroid.y = -10;
            }
        }
    }

    // Remove bullets that out of screen
    remove_destroyed_objects(vecBullets);

    // Add new asteroids to vecAsteroids pull and clear
    if (!vecNewAsteroids.empty()) {
        for (auto newAsteroid: vecNewAsteroids)
            vecAsteroids.emplace_back(newAsteroid);
        vecNewAsteroids.clear();
    }

    // Remove asteroids that out of screen
    remove_destroyed_objects(vecAsteroids);

    if (vecAsteroids.empty())
        load_next_level();
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw(){
    // clear backbuffer
    memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

    if (isGameOver) {
        draw_game_over_message();
        return;
    }

    if (isWin) {
        draw_win_message();
        draw_score((int)player.score, SCREEN_HEIGHT / 2 + 3 * SCORE_X_POS, SCREEN_WIDTH / 2 - 2 * SCORE_Y_POS);
        draw_score((int)highScore, SCREEN_HEIGHT - SCORE_X_POS, SCORE_Y_POS);
        return;
    }

    // Draw Ship
    draw_poly_model(vecShipModel, player.x, player.y, player.angle, player.size, WHITE_COLOR);
    if (isEngineUp)
        draw_poly_model(vecEngineFireModel, player.x, player.y, player.angle, player.size, WHITE_COLOR);

    // Draw asteroids
    for(auto &asteroid : vecAsteroids){
      draw_poly_model(vecAsteroidModel, asteroid.x, asteroid.y, asteroid.angle, asteroid.size, WHITE_COLOR);
    }

    // Draw bullets
    for(auto &bullet : vecBullets){
      fill_buffer(bullet.x, bullet.y, WHITE_COLOR);
    }

    // Draw Lives
    draw_lives();

    // Draw Score
    draw_score((int)player.score, SCORE_X_POS, SCORE_Y_POS);

    // Draw HighScore
    draw_score((int)highScore, SCREEN_HEIGHT - SCORE_X_POS, SCORE_Y_POS);
}

// free game data in this function
void finalize()
{
    check_and_write_new_high_score();
}


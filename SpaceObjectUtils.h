#pragma once
#include <cstdint>
#include <cmath>
#include <vector>
#include <algorithm>

// Color consts
#define WHITE_COLOR 4294967295

// Angle consts
#define SHIP_ANGLE_FACTOR 3.5f
#define LIVES_ROTATION 0.0f
#define SCORE_ROTATION 0.0f
#define TEXT_ROTATION 0.0f

// Speed consts
#define SHIP_SPEED_FACTOR 200.0f
#define SHIP_MAX_SPEED 450.0f
#define BULLET_SPEED_FACTOR 600.0f
#define SHIP_SPEED_BRAKE_FACTOR 80.0f
#define LARGEST_ASTEROID_SPEED_FACTOR 100.0f

// Size consts
#define LARGE_ASTEROID_SIZE 10.0f
#define SHIP_SIZE 4.0f
#define BULLET_SIZE 200.0f
#define ASTEROID_DOWNGRADE_FACTOR 4.0f
#define LIVES_SIZE 3.5f
#define SCORE_SIZE 3.5f
#define TEXT_SIZE 25.0f

// Time consts
#define SHOOT_DELAY_SECONDS 0.12f

// Other consts
#define ASTEROID_EDGES_COUNT 20
#define MIN_FACTOR_TO_SEE_MOVING 40.0f
#define NEW_ASTEROIDS_COUNT_DIVIDER 3.0f
#define LARGEST_ASTEROID_SCORE 100.0f
#define LIVES_X_POS 80
#define LIVES_Y_POS 80
#define SCORE_X_POS 40
#define SCORE_Y_POS 80
#define TEXT_X SCREEN_HEIGHT / 2
#define TEXT_Y SCREEN_WIDTH / 10


struct SpaceObject{
    int x;
    int y;
    float dx;
    float dy;
    float size;
    float angle;
    float score;
};

extern SpaceObject player;
extern std::vector<SpaceObject> vecAsteroids;
extern std::vector<SpaceObject> vecNewAsteroids;
extern std::vector<SpaceObject> vecBullets;

extern std::vector<std::pair<int, int>> vecShipModel;
extern std::vector<std::pair<int, int>> vecLiveModel;
extern std::vector<std::pair<int, int>> vecEngineFireModel;
extern std::vector<std::pair<int, int>> vecAsteroidModel;

extern std::vector<std::vector<std::pair<int, int>>> vecNumbersModels;
extern std::vector<std::pair<int, int>> vecZeroNumberModel;
extern std::vector<std::pair<int, int>> vecOneNumberModel;
extern std::vector<std::pair<int, int>> vecTwoNumberModel;
extern std::vector<std::pair<int, int>> vecThreeNumberModel;
extern std::vector<std::pair<int, int>> vecFourNumberModel;
extern std::vector<std::pair<int, int>> vecFiveNumberModel;
extern std::vector<std::pair<int, int>> vecSixNumberModel;
extern std::vector<std::pair<int, int>> vecSevenNumberModel;
extern std::vector<std::pair<int, int>> vecEightNumberModel;
extern std::vector<std::pair<int, int>> vecNineNumberModel;

extern std::vector<std::pair<int, int>> vecYCharModel;
extern std::vector<std::pair<int, int>> vecOCharModel;
extern std::vector<std::pair<int, int>> vecUCharModel;
extern std::vector<std::pair<int, int>> vecWCharModel;
extern std::vector<std::pair<int, int>> vecICharModel;
extern std::vector<std::pair<int, int>> vecNCharModel;
extern std::vector<std::pair<int, int>> vecGCharModel;
extern std::vector<std::pair<int, int>> vecACharModel;
extern std::vector<std::pair<int, int>> vecMCharModel;
extern std::vector<std::pair<int, int>> vecECharModel;
extern std::vector<std::pair<int, int>> vecVCharModel;
extern std::vector<std::pair<int, int>> vecRCharModel;

extern bool isDead;
extern float shootCounter;
extern bool isEngineUp;

std::pair<int, int> wrap_coordinates(int currX, int currY);

void fill_buffer(int x, int y, uint32_t color);

// Bresenham
void draw_line (int x1, int y1, int x2, int y2, uint32_t color);

std::vector<std::pair<int, int>> get_object_points_coordinates(const std::vector<std::pair<int, int>> &vecModelCoord,
                                                               int x, int y, float rotate,
                                                               float scale);

void draw_poly_model(const std::vector<std::pair<int, int>> &vecModelCoord,
                     int x, int y, float rotate,
                     float scale, uint32_t color);

void draw_lives();

void draw_score(int score, int x, int y);

bool is_object_to_remove(int x, int y);

bool is_valid_time_to_shoot();

bool is_bullet_inside_polygon(SpaceObject bullet, std::vector<std::pair<int, int>> asteroid);

void calc_space_object_coordinates(std::vector<SpaceObject>& spaceObjects, float dt);

void calc_space_object_coordinates(SpaceObject& spaceObject, float dt);

void change_player_dx_dy(float dt, float speedFactor, bool isBrake);

void add_new_bullet();

void add_new_asteroid(SpaceObject fromAsteroid);

void remove_destroyed_objects(std::vector<SpaceObject>& spaceObjects);

bool is_ship_crashed_with_asteroid(SpaceObject ship, SpaceObject asteroid);

void init_models();

void draw_win_message();

void draw_game_over_message();

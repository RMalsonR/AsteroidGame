#pragma once
#include <cstdint>
#include <vector>

// Color consts
#define WHITE_COLOR 4294967295

// Angle consts
#define SHIP_ANGLE_FACTOR 2.8f

// Speed consts
#define SHIP_SPEED_FACTOR 200.0f
#define BULLET_SPEED_FACTOR 600.0f
#define SHIP_SPEED_BRAKE_FACTOR 80.0f
#define LARGEST_ASTEROID_SPEED_FACTOR 100.0f

// Size consts
#define LARGE_ASTEROID_SIZE 10.0f
#define SHIP_SIZE 4.0f
#define BULLET_SIZE 200.0f
#define ASTEROID_DOWNGRADE_FACTOR 4.0f

// Time consts
#define SHOOT_DELAY_SECONDS 0.6f

// Other consts
#define ASTEROID_EDGES_COUNT 20
#define MIN_FACTOR_TO_SEE_MOVING 40.0f
#define NEW_ASTEROIDS_COUNT_DIVIDER 3.0f


struct SpaceObject{
    int x;
    int y;
    float dx;
    float dy;
    float size;
    float angle;
};

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

bool is_object_to_remove(int x, int y);

bool is_valid_time_to_shoot();

bool is_bullet_inside_polygon(SpaceObject bullet, std::vector<std::pair<int, int>> asteroid);

void calc_space_object_coordinates(std::vector<SpaceObject>& spaceObjects, float dt);

void calc_space_object_coordinates(SpaceObject& spaceObject, float dt);

void change_player_dx_dy(float dt, float speedFactor, bool isBrake);

void add_new_bullet();

void add_new_asteroid(SpaceObject fromAsteroid);

void remove_destroyed_objects(std::vector<SpaceObject>& spaceObjects);

#pragma once
#include <cstdint>
#include <vector>

// Color consts
#define WHITE_COLOR 4294967295

// Angle consts
#define SHIP_ANGLE_FACTOR 2.5f

// Speed consts
#define SHIP_SPEED_SCALE_FACTOR 200.0f
#define BULLET_SPEED_SCALE_FACTOR 350.0f
#define SHIP_BRAKE_BY_ROTATION_FACTOR 80.0f

// Size consts
#define LARGEST_ASTEROID_SIZE 10.0f
#define SHIP_SIZE 4.0f
#define BULLET_SIZE 200.0f

// Time consts
#define SHOOT_DELAY_SECONDS 0.6f

// Other consts
#define LARGEST_ASTEROID_EDGES_COUNT 20
#define MIN_FACTOR_TO_SEE_MOVING 50.0f


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

void draw_poly_model(const std::vector<std::pair<int, int>> &vecModelCoord,
                     int x, int y, float rotate,
                     float scale, uint32_t color);

bool is_bullet_out_of_screen(int x, int y);

bool is_valid_time_to_shoot();

bool is_point_inside_circle(int cX, int cY, float radius, int bX, int bY);

void calc_space_object_coordinates(std::vector<SpaceObject>& spaceObjects, float dt);

void calc_space_object_coordinates(SpaceObject& spaceObject, float dt);

void change_player_dx_dy(float dt, float speedFactor, bool is_brake);

void add_new_bullet();

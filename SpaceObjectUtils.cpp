#include "Engine.h"
#include "SpaceObjectUtils.h"

#include <vector>

extern SpaceObject player;
extern std::vector<SpaceObject> vecAsteroids;
extern std::vector<SpaceObject> vecBullets;

extern std::vector<std::pair<int, int>> vecShipModel;
extern std::vector<std::pair<int, int>> vecEngineFireModel;
extern std::vector<std::pair<int, int>> vecAsteroidModel;

extern int livesCount;
extern bool isDead;
extern float shootCounter;
extern bool isEngineUp;


std::pair<int, int> wrap_coordinates(int currX, int currY){
    int resX = currX, resY = currY;
    if (currX <= 0) {
        resX += SCREEN_HEIGHT;
    }
    if (currX >= SCREEN_HEIGHT){
        resX -= SCREEN_HEIGHT;
    }

    if (currY <= 0) {
        resY += SCREEN_WIDTH;
    }
    if (currY >= SCREEN_WIDTH){
        resY -= SCREEN_WIDTH;
    }
    return std::make_pair(resX, resY);
};


void fill_buffer(int x, int y, uint32_t color){
    std::pair<int, int> validCoord = wrap_coordinates(x, y);
    buffer[validCoord.first][validCoord.second] = color;
};


// Bresenham
void draw_line (int x1, int y1, int x2, int y2, uint32_t color = WHITE_COLOR) {
    int deltaX = abs((x2 - x1));
    int deltaY = abs((y2 - y1));
    int signX = x1 < x2 ? 1 : -1;
    int signY = y1 < y2 ? 1 : -1;
    int error = deltaX - deltaY;

    fill_buffer(x2, y2, color);

    while(x1 != x2 || y1 != y2){
        fill_buffer(x1, y1, color);
        int error2 = error * 2;
        if(error2 > -deltaY){
            error -= deltaY;
            x1 += signX;
        }
        if(error2 < deltaX){
            error += deltaX;
            y1 += signY;
        }
    }
}

std::vector<std::pair<int, int>> get_object_points_coordinates(
        const std::vector<std::pair<int, int>> &vecModelCoord,
        int x, int y, float rotate = 0.0f,
        float scale = 1.0f){
    std::vector<std::pair<int, int>> vecTransformedCoord;
    int nEdges = vecModelCoord.size();
    vecTransformedCoord.resize(nEdges);

    // Rotate model
    for (int i = 0; i < nEdges ; i++){
        vecTransformedCoord[i].first = vecModelCoord[i].first * cosf(rotate) - vecModelCoord[i].second * sinf(rotate);
        vecTransformedCoord[i].second = vecModelCoord[i].first * sinf(rotate) + vecModelCoord[i].second * cosf(rotate);
    }

    // Scale model
    for (int i = 0; i < nEdges ; i++){
        vecTransformedCoord[i].first *= scale;
        vecTransformedCoord[i].second *= scale;
    }

    // Translate model (move)
    for (int i = 0; i < nEdges; i++){
        vecTransformedCoord[i].first += x;
        vecTransformedCoord[i].second += y;
    }

    return vecTransformedCoord;
}


void draw_poly_model(const std::vector<std::pair<int, int>> &vecModelCoord,
                     int x, int y, float rotate = 0.0f,
                     float scale = 1.0f, uint32_t color = WHITE_COLOR){
    std::vector<std::pair<int, int>> vecTransformedCoord = get_object_points_coordinates(
            vecModelCoord, x, y, rotate, scale);
    int nEdges = vecTransformedCoord.size();

    // Draw lines to create a polygon
    for (int i = 0; i < nEdges; i++){
        int j = (i + 1);
        int x1 = vecTransformedCoord[i % nEdges].first;
        int y1 = vecTransformedCoord[i % nEdges].second;
        int x2 = vecTransformedCoord[j % nEdges].first;
        int y2 = vecTransformedCoord[j % nEdges].second;
        draw_line(x1, y1, x2, y2, color);
    }
}


bool is_bullet_out_of_screen(int x, int y){
    return x < 1 || y < 1 || x > SCREEN_HEIGHT || y > SCREEN_WIDTH;
}


bool is_valid_time_to_shoot(){
    if (shootCounter >= SHOOT_DELAY_SECONDS){
        shootCounter = 0.0f;
        return true;
    }
    return false;
}

bool is_point_inside_circle(int cX, int cY, float radius, int bX, int bY){
    return sqrt((bX - cX) * (bX - cX) + (bY - cY) * (bY - cY)) < radius;
}


void calc_space_object_coordinates(std::vector<SpaceObject>& spaceObjects, float dt){
    for(auto &spaceObject : spaceObjects){
        std::pair<float, float> spaceObjectNextCoord = wrap_coordinates(
                spaceObject.x + (int) (spaceObject.dx * dt),
                spaceObject.y + (int) (spaceObject.dy * dt));

        spaceObject.x = spaceObjectNextCoord.first;
        spaceObject.y = spaceObjectNextCoord.second;
    }
}

void calc_space_object_coordinates(SpaceObject& spaceObject, float dt){
    std::pair<float, float> spaceObjectNextCoord = wrap_coordinates(
            spaceObject.x + (int) (spaceObject.dx * dt),
            spaceObject.y + (int) (spaceObject.dy * dt));

    spaceObject.x = spaceObjectNextCoord.first;
    spaceObject.y = spaceObjectNextCoord.second;
}


void change_player_dx_dy(float dt, float speedFactor = SHIP_SPEED_SCALE_FACTOR, bool is_brake = false) {
    if (is_brake) {
        if (-MIN_FACTOR_TO_SEE_MOVING <= player.dx && player.dx <= MIN_FACTOR_TO_SEE_MOVING)
            if (-SHIP_BRAKE_BY_ROTATION_FACTOR <= player.dx && player.dx <= SHIP_BRAKE_BY_ROTATION_FACTOR)
                player.dx = 0.0f;

        if (-MIN_FACTOR_TO_SEE_MOVING <= player.dy && player.dy <= MIN_FACTOR_TO_SEE_MOVING)
            if (-SHIP_BRAKE_BY_ROTATION_FACTOR <= player.dy && player.dy <= SHIP_BRAKE_BY_ROTATION_FACTOR)
                player.dy = 0.0f;
    }

    player.dx += sin(player.angle) * speedFactor * dt;
    player.dy += -cos(player.angle) * speedFactor * dt;

}


void add_new_bullet(){
    if (is_valid_time_to_shoot())
        vecBullets.push_back(
                {player.x, player.y,
                        BULLET_SPEED_SCALE_FACTOR * sinf(player.angle),
                        -BULLET_SPEED_SCALE_FACTOR * cosf(player.angle),
                        BULLET_SIZE });
}


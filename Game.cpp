#include "Engine.h"
#include <stdlib.h>
#include <memory.h>

#include <stdio.h>
#include <vector>
#include <iostream>

using namespace std;

struct SpaceObject{
    int x;
    int y;
    float dx;
    float dy;
    float size;
    float angle;
};

vector<SpaceObject> vecAsteroids;
vector<SpaceObject> vecBullets;
SpaceObject player;

vector<pair<int, int>> vecShipModel;
vector<pair<int, int>> vecAsteroidModel;

const uint32_t WHITE_COLOR = (255 << 24) + (255 << 16) + (255 << 8) + 255;
const float SHIP_SPEED_SCALE_FACTOR = 200.0f;
const float SHIP_ANGLE_FACTOR = 3.5f;
const float BULLET_SPEED_SCALE_FACTOR = 250.0f;
const int THE_BIGGEST_OBJECT_SIZE = 50;


pair<int, int> wrapCoordinates(int currX, int currY){
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
    return make_pair(resX, resY);
}

void fillBuffer(int x, int y, uint32_t color = WHITE_COLOR){
    pair<int, int> valid_coord = wrapCoordinates(x, y);
    buffer[valid_coord.first][valid_coord.second] = color;
}


// Bresenham
//void drawLine (float x1, float y1, float x2, float y2, uint32_t color = WHITE_COLOR) {
//    float A, B, sign;
//    A = y2 - y1;
//    B = x1 - x2;
//
//    if (abs(A) > abs(B))
//        sign = 1;
//    else
//        sign = -1;
//
//    float signa, signb;
//
//    if (A < 0)
//        signa = -1;
//    else
//        signa = 1;
//
//    if (B < 0)
//        signb = -1;
//    else
//        signb = 1;
//
//    float f = 0;
//    fillBuffer(x1, y1, color);
//    float x = x1, y = y1;
//    if(sign < 0){
//        do {
//            f += A * signa;
//            if (f > 0){
//                f -= B * signb;
//                y += signa;
//            }
//            x -= signb;
//            fillBuffer(x, y, color);
//        } while (x != x2 || y != y2);
//    } else {
//        do {
//            f += B * signb;
//            if (f > 0){
//                f -= A* signa;
//                x -= signb;
//            }
//            y += signa;
//            fillBuffer(x, y, color);
//        } while (x != x2 || y != y2);
//    }
//}

void drawLine (int x1, int y1, int x2, int y2, uint32_t color = WHITE_COLOR) {
    int deltaX = abs((x2 - x1));
    int deltaY = abs((y2 - y1));
    int signX = x1 < x2 ? 1 : -1;
    int signY = y1 < y2 ? 1 : -1;
    int error = deltaX - deltaY;
    fillBuffer(x2, y2, color);
    while(x1 != x2 || y1 != y2)
    {
        fillBuffer(x1, y1, color);
        int error2 = error * 2;
        if(error2 > -deltaY)
        {
            error -= deltaY;
            x1 += signX;
        }
        if(error2 < deltaX)
        {
            error += deltaX;
            y1 += signY;
        }
    }
}

void drawPolyModel(vector<pair<int, int>> &vecModelCoord,
                   int x, int y, float rotate = 0.0f,
                   float scale = 1.0f, uint32_t color = WHITE_COLOR){
    vector<pair<int, int>> vecTransformedCoord;
    int nVerts = vecModelCoord.size();
    vecTransformedCoord.resize(nVerts);

    // Rotate model
    for (int i = 0; i < nVerts ; i++){
        vecTransformedCoord[i].first = vecModelCoord[i].first * cosf(rotate) - vecModelCoord[i].second * sinf(rotate);
        vecTransformedCoord[i].second = vecModelCoord[i].first * sinf(rotate) + vecModelCoord[i].second * cosf(rotate);
    }

    // Scale model
    for (int i = 0; i < nVerts ; i++){
        vecTransformedCoord[i].first *= scale;
        vecTransformedCoord[i].second *= scale;
    }

    // Translate model (move)
    for (int i = 0; i < nVerts; i++)
    {
        vecTransformedCoord[i].first += x;
        vecTransformedCoord[i].second += y;
    }

    // Draw lines to create a polygon
    for (int i = 0; i < 4; i++){
        int j = (i + 1);
        int x1 = vecTransformedCoord[i % 3].first;
        int y1 = vecTransformedCoord[i % 3].second;
        int x2 = vecTransformedCoord[j % 3].first;
        int y2 = vecTransformedCoord[j % 3].second;
        drawLine(x1, y1, x2, y2, color);
    }
}

bool is_bullet_out_of_screen(int x, int y){
    return x < 1 || y < 1 || x > SCREEN_HEIGHT || y > SCREEN_HEIGHT;
}

// initialize game data in this function
void initialize(){

    vecAsteroids.push_back({
            20,
            20,
            100.0f,
            -100.0f,
            80.0f,
            0.0f});

    player.x = SCREEN_HEIGHT / 2;
    player.y = SCREEN_WIDTH / 2;
    player.dx = 0.0f;
    player.dy = 0.0f;
    player.angle = 0.0f;
    player.size = 4.0f;

    vecShipModel = {
            {0.0f, -5.0f},
            {-2.5f, 2.5f},
            {2.5f, 2.5f}
    };


    int nVerts = 50;
    for (int i = 0; i < nVerts; i++){
        float radius = 5.0f;
        float a = ((float) i / (float)nVerts) * (float)M_PI * 2;
        vecAsteroidModel.emplace_back((int)(radius * sinf(a)), (int)(radius * cosf(a)));
    }

}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt){

    if (is_key_pressed(VK_LEFT))
        player.angle += SHIP_ANGLE_FACTOR * dt;
    if (is_key_pressed(VK_RIGHT))
        player.angle -= SHIP_ANGLE_FACTOR * dt;

    if(is_key_pressed(VK_UP)){
        player.dx += sin(player.angle) * SHIP_SPEED_SCALE_FACTOR * dt;
        player.dy += -cos(player.angle) * SHIP_SPEED_SCALE_FACTOR * dt;
    }

    if(is_key_pressed(VK_SPACE)){
        vecBullets.push_back(
                {player.x, player.y,
                 BULLET_SPEED_SCALE_FACTOR * sinf(player.angle),
                 -BULLET_SPEED_SCALE_FACTOR * cosf(player.angle),
                 100.0f });
    }

    pair<int, int> player_next_coord = wrapCoordinates(
            player.x + (int)(player.dx * dt),
            player.y + (int)(player.dy * dt));

    player.x = player_next_coord.first;
    player.y = player_next_coord.second;

    for(auto &asteroid : vecAsteroids){
        pair<float, float> asteroid_next_coord = wrapCoordinates(
                asteroid.x + (int)(asteroid.dx * dt),
                asteroid.y + (int)(asteroid.dy * dt));

        asteroid.x = asteroid_next_coord.first;
        asteroid.y = asteroid_next_coord.second;
    }

    for(auto &bullet : vecBullets){
        bullet.x += (int)(bullet.dx * dt);
        bullet.y += (int)(bullet.dy * dt);

        // TODO: too complexity. Need to remove by O(1).
        auto i = remove_if(vecBullets.begin(), vecBullets.end(), is_bullet_out_of_screen(bullet.x, bullet.y));
        vecBullets.erase(i);
    }

    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw(){
  // clear backbuffer
  memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

  // Draw Ship
  drawPolyModel(vecShipModel, player.x, player.y, player.angle, player.size);

  for(auto &asteroid : vecAsteroids){
      drawPolyModel(vecAsteroidModel, asteroid.x, asteroid.y, asteroid.angle, asteroid.size);
  }

  for(auto &bullet : vecBullets){
      fillBuffer(bullet.x, bullet.y);
  }

}

// free game data in this function
void finalize()
{
}


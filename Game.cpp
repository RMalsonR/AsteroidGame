#include "Engine.h"
#include "SpaceObjectUtils.h"
#include <cstdlib>
#include <memory.h>

#include <vector>

std::vector<SpaceObject> vecAsteroids;
std::vector<SpaceObject> vecBullets;
std::vector<SpaceObject> vecNewAsteroids;
SpaceObject player;

std::vector<std::pair<int, int>> vecShipModel;
std::vector<std::pair<int, int>> vecEngineFireModel;
std::vector<std::pair<int, int>> vecAsteroidModel;
float shootCounter = 0.0f;
bool isEngineUp = false;

// initialize game data in this function
void initialize(){

    vecBullets = {};
    vecNewAsteroids = {};

    vecAsteroids.push_back({
                                   100,
                                   100,
                                   LARGEST_ASTEROID_SPEED_FACTOR,
                                   -LARGEST_ASTEROID_SPEED_FACTOR,
                                   LARGE_ASTEROID_SIZE,
                                   0.0f});

    player.x = SCREEN_HEIGHT / 2;
    player.y = SCREEN_WIDTH / 2;
    player.dx = 0.0f;
    player.dy = 0.0f;
    player.angle = 0.0f;
    player.size = SHIP_SIZE;

    vecShipModel = {
            {0.0f, -5.0f},
            {-2.5f, 2.5f},
            {-1.0f, 1.0f},
            {1.0f, 1.0f},
            {2.5f, 2.5f}
    };

    vecEngineFireModel = {
            {-1.0f, 1.0f},
            {0.0f, 3.0f},
            {1.0f, 1.0f},
    };

    for (int i = 0; i < ASTEROID_EDGES_COUNT; i++){
        float randRadius = (float)random() / (float)RAND_MAX * 2.0f + 4.0f;
        float angle = ((float) i / (float)ASTEROID_EDGES_COUNT) * ((float)M_PI * 2.0f);
        int x = (int)(randRadius * sinf(angle));
        int y = (int)(randRadius * cosf(angle));
        vecAsteroidModel.emplace_back(x, y);
    }
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt) {

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
            vecAsteroids.push_back(newAsteroid);
        vecNewAsteroids.clear();
    }

    // Remove asteroids that out of screen
    remove_destroyed_objects(vecAsteroids);

    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw(){
  // clear backbuffer
  memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

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
}

// free game data in this function
void finalize()
{
}


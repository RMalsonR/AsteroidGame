#include "Engine.h"
#include "SpaceObjectUtils.h"


extern SpaceObject player;
extern std::vector<SpaceObject> vecAsteroids;
extern std::vector<SpaceObject> vecNewAsteroids;
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
        resX += SCREEN_HEIGHT - 1;
    }
    if (currX >= SCREEN_HEIGHT - 1){
        resX -= SCREEN_HEIGHT - 1;
    }

    if (currY <= 0) {
        resY += SCREEN_WIDTH - 1;
    }
    if (currY >= SCREEN_WIDTH - 1){
        resY -= SCREEN_WIDTH - 1;
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


bool is_object_to_remove(int x, int y){
    return x < 0 || y < 0 || x > SCREEN_HEIGHT - 1 || y > SCREEN_WIDTH - 1;
}


bool is_valid_time_to_shoot(){
    if (shootCounter >= SHOOT_DELAY_SECONDS){
        shootCounter = 0.0f;
        return true;
    }
    return false;
}


bool is_two_lines_are_crossing(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4){
    if(y3 >= y4)
    {
        std::swap(x3,x4);
        std::swap(y3,y4);
    }

    int dx1 = x2 - x1;
    int dx2 = x4 - x3;
    int dy1 = y2 - y1;
    int dy2 = y4 - y3;

    float k1 = (dx1 == 0) ? 0.0f : (dy1 == 0) ? 1.0f : (float)(y2-y1)/(float)(x2-x1);
    float k2 = (dx2 == 0) ? 0.0f : (dy2 == 0) ? 1.0f : (float)(y4-y3)/(float)(x4-x3);
    if (k1==k2)
        return false;
    float b1 = (float)y1 - (k1 * (float)x1);
    float b2 = (float)y3 - (k2 * (float)x3);
    float x = (b2 - b1)/(k1 - k2);//точка пересечения
    float y = k1 * x + b1;//точка пересечения

//    return (((x1<=x4)&&(x4<=x2)&&(y2>y3)&&((float)y3>=y))||((x1<=x3)&&(x3<=x2)&&((float)y3<=y)));
//    return ((((x1<=x) && (x2>=x) && (x3<=x) && (x4 >=x)) || ((y1<=y) && (y2>=y) && (y3<=y)  && (y4>=y))));
    return (x >= (float)x3) && (x <= (float)x4) && (y >= (float)y3) && (y <= (float)y4);
}


bool is_bullet_inside_polygon(SpaceObject bullet, std::vector<std::pair<int, int>> asteroid){
    int asteroidEdgesCount = asteroid.size();

    bool result = false;

    for (int i = 0; i < asteroidEdgesCount; i++){
        int j = (i + 1);
        int aX1 = asteroid[i % asteroidEdgesCount].first;
        int aY1 = asteroid[i % asteroidEdgesCount].second;
        int aX2 = asteroid[j % asteroidEdgesCount].first;
        int aY2 = asteroid[j % asteroidEdgesCount].second;

        if ( (aY1 < bullet.y && aY2 >= bullet.y || aY2 < bullet.y && aY1 >= bullet.y) &&
             (aX1 + (bullet.y - aY1) / (aY2 - aY1) * (aX2 - aX1) < bullet.x) )
            result = !result;
    }
//    return ((crossCount % 2 != 0) && crossCount != 0);
    return result;
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


void change_player_dx_dy(float dt, float speedFactor = SHIP_SPEED_FACTOR, bool isBrake = false) {
    float xAngle = sinf(player.angle);
    float yAngle = -cosf(player.angle);

    if (isBrake) {
        speedFactor = - speedFactor;
        if (MIN_FACTOR_TO_SEE_MOVING <= player.dx || player.dx <= -MIN_FACTOR_TO_SEE_MOVING) {
            if (-SHIP_SPEED_BRAKE_FACTOR <= player.dx && player.dx <= SHIP_SPEED_BRAKE_FACTOR)
                player.dx = 0.0f;
            else {
                if (player.dx * xAngle >= 0.0f)
                    player.dx += xAngle * speedFactor * dt;
                else
                    player.dx -= xAngle * speedFactor * dt;
            }
        }

        if (MIN_FACTOR_TO_SEE_MOVING <= player.dy || player.dy <= -MIN_FACTOR_TO_SEE_MOVING) {
            if (-SHIP_SPEED_BRAKE_FACTOR <= player.dy && player.dy <= SHIP_SPEED_BRAKE_FACTOR)
                player.dy = 0.0f;
            else {
                if (player.dy * yAngle >= 0.0f)
                    player.dy += yAngle * speedFactor * dt;
                else
                    player.dy -= yAngle * speedFactor * dt;
            }
        }
        return;
    }

    player.dx += xAngle * speedFactor * dt;
    player.dy += yAngle * speedFactor * dt;

}


void add_new_bullet(){
    if (is_valid_time_to_shoot())
        vecBullets.emplace_back(
                SpaceObject{
                    player.x, player.y,
                    BULLET_SPEED_FACTOR * sinf(player.angle),
                    -BULLET_SPEED_FACTOR * cosf(player.angle),
                    BULLET_SIZE });
}

void add_new_asteroid(SpaceObject fromAsteroid){
    float newASteroidSize = fromAsteroid.size - ASTEROID_DOWNGRADE_FACTOR;

    // 6.0f size asteroid faster than 10.0f size by 2.0f. 2.0f size asteroid faster than 10.0f size by 4.0f
    float newAsteroidSpeedScaleFactor = (LARGE_ASTEROID_SIZE - newASteroidSize) / 2.0f;
    if (newASteroidSize > 0.0f){
        int newAsteroidsCount = (int)(fromAsteroid.size / NEW_ASTEROIDS_COUNT_DIVIDER);
        for (int i = 0; i < newAsteroidsCount; i++){
            // ((float)i/(float)newAsteroidsCount) *
            float randomFloat = ((float)random()/(float)RAND_MAX);
            float newAngle = randomFloat * ((float)M_PI * 2.0f);
            float sinAngle = sinf(newAngle);
            float cosAngle = cosf(newAngle);

            int newX = fromAsteroid.x, newY = fromAsteroid.y;
            float newDx = LARGEST_ASTEROID_SPEED_FACTOR * newAsteroidSpeedScaleFactor * sinAngle;
            float newDy = LARGEST_ASTEROID_SPEED_FACTOR * newAsteroidSpeedScaleFactor * cosAngle;
            vecNewAsteroids.emplace_back(SpaceObject{newX, newY, newDx, newDy, newASteroidSize, 0.0f});
        }
    }
}


void remove_destroyed_objects(std::vector<SpaceObject>& spaceObjects){
    if (!spaceObjects.empty()){
        auto i = remove_if(spaceObjects.begin(), spaceObjects.end(), [&](SpaceObject spaceObject) {
            return is_object_to_remove(spaceObject.x, spaceObject.y);});
        if(i != spaceObjects.end())
            spaceObjects.erase(i);
    }
}


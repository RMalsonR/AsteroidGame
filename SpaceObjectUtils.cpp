#include "Engine.h"
#include "SpaceObjectUtils.h"
#include "LevelManager.h"


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

void draw_number_or_char(const std::vector<std::pair<int, int>> &vecModelCoord,
                         int x, int y, float rotate = 0.0f,
                         float scale = 1.0f, uint32_t color = WHITE_COLOR){
    std::vector<std::pair<int, int>> vecTransformedCoord = get_object_points_coordinates(
            vecModelCoord, x, y, rotate, scale);
    int nEdges = vecTransformedCoord.size();

    for (int i = 0; i < nEdges - 1; i++){
        int j = (i + 1);
        int x1 = vecTransformedCoord[i].first;
        int y1 = vecTransformedCoord[i].second;
        int x2 = vecTransformedCoord[j].first;
        int y2 = vecTransformedCoord[j].second;
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


int calc_area(std::pair<int, int> a,
              std::pair<int, int> b,
              std::pair<int, int> c){
    return (b.first - a.first) * (c.second - a.second) - (b.second - a.second) * (c.first - a.first);
}


bool is_intersect(int a, int b, int c, int d){
    if (a > b)
        std::swap (a, b);
    if (c > d)
        std::swap (c, d);
    return std::max(a,c) <= std::min(b,d);
}


bool is_two_lines_are_crossing(std::pair<int, int> a1,
                               std::pair<int, int> a2 ,
                               std::pair<int, int> b1,
                               std::pair<int, int> b2){
    return is_intersect(a1.first, a2.first, b1.first, b2.first) &&
            is_intersect(a1.second, a2.second, b1.second, b2.second) &&
            calc_area(a1, a2, b1) * calc_area(a1, a2, b2) <= 0 &&
            calc_area(b1, b2, a1) * calc_area(b1, b2, a2) <= 0;


//    if(y1 >= y2) {
//        std::swap(x1,x2);
//        std::swap(y1,y2);
//    }
//    if(y3 >= y4)
//    {
//        std::swap(x3,x4);
//        std::swap(y3,y4);
//    }

//    if(y3 >= y4)
//    {
//        std::swap(x3,x4);
//        std::swap(y3,y4);
//    }

//    int dx1 = x2 - x1;
//    int dx2 = x4 - x3;
//    int dy1 = y2 - y1;
//    int dy2 = y4 - y3;
//
//    float k1 = (dx1 == 0) ? 0.0f : (dy1 == 0) ? 1.0f : (float)(y2-y1)/(float)(x2-x1);
//    float k2 = (dx2 == 0) ? 0.0f : (dy2 == 0) ? 1.0f : (float)(y4-y3)/(float)(x4-x3);
//    if (k1==k2)
//        return false;
//    float b1 = (float)y1 - (k1 * (float)x1);
//    float b2 = (float)y3 - (k2 * (float)x3);
//    float x = (b2 - b1)/(k1 - k2);//точка пересечения
//    float y = k1 * x + b1;//точка пересечения
//
//    return ((x >= (float)x1) && (x <= (float)x2) && (x >= (float)x3) && (x <= (float)x4)) && ((y >= (float)y1) && (y <= (float)y2) && (y >= (float)y3) && (y <= (float)y4));

//    return (((x1<=x4)&&(x4<=x2)&&(y2>y3)&&((float)y3>=y))||((x1<=x3)&&(x3<=x2)&&((float)y3<=y)));
//    return ((((x1<=x) && (x2>=x) && (x3<=x) && (x4 >=x)) || ((y1<=y) && (y2>=y) && (y3<=y)  && (y4>=y))));
//    return (x >= (float)x3) && (x <= (float)x4) && (y >= (float)y3) && (y <= (float)y4);
}

bool is_ship_crashed_with_asteroid(SpaceObject ship, SpaceObject asteroid){
    std::vector<std::pair<int, int>> vecShipCoordinates = get_object_points_coordinates(vecShipModel, ship.x, ship.y,
                                                                                        ship.angle, ship.size);
    std::vector<std::pair<int, int>> vecAsteroidCoordinates = get_object_points_coordinates(vecShipModel, asteroid.x,
                                                                                            asteroid.y, asteroid.angle,
                                                                                            asteroid.size);
    int shipEdgesCount = vecShipCoordinates.size();
    int asteroidEdgesCount = vecAsteroidCoordinates.size();

    for (int i = 0; i < shipEdgesCount; i++){
        int iNext = i + 1;
        std::pair<int, int> s1 = vecShipCoordinates[i % shipEdgesCount];
        std::pair<int, int> s2 = vecShipCoordinates[iNext % shipEdgesCount];
//        int sX1 = vecShipCoordinates[i % shipEdgesCount].first;
//        int sY1 = vecShipCoordinates[i % shipEdgesCount].second;
//        int sX2 = vecShipCoordinates[iNext % shipEdgesCount].first;
//        int sY2 = vecShipCoordinates[iNext % shipEdgesCount].second;
        for (int j = 0; j < asteroidEdgesCount; ++j) {
            int jNext = j + 1;
            std::pair<int, int> a1 = vecAsteroidCoordinates[j % asteroidEdgesCount];
            std::pair<int, int> a2 = vecAsteroidCoordinates[jNext % asteroidEdgesCount];
//            int aX1 = vecAsteroidCoordinates[i % asteroidEdgesCount].first;
//            int aY1 = vecAsteroidCoordinates[i % asteroidEdgesCount].second;
//            int aX2 = vecAsteroidCoordinates[j % asteroidEdgesCount].first;
//            int aY2 = vecAsteroidCoordinates[j % asteroidEdgesCount].second;
            if (is_two_lines_are_crossing(s1, s2, a1, a2))
                return true;
        }
    }
    return false;
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
        if (is_ship_crashed_with_asteroid(player, spaceObject)){
            isDead = true;
            break;
        }

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
        speedFactor = -speedFactor;
        if (MIN_FACTOR_TO_SEE_MOVING <= player.dx || player.dx <= -MIN_FACTOR_TO_SEE_MOVING) {
            if (-SHIP_SPEED_BRAKE_FACTOR <= player.dx && player.dx <= SHIP_SPEED_BRAKE_FACTOR)
                player.dx = 0.0f;
            else {
//                if (player.dx * xAngle >= 0.0f)
                if (player.dx >= 0.0f)
//                    player.dx += xAngle * speedFactor * dt;
                    player.dx += speedFactor * dt;
                else
//                    player.dx -= xAngle * speedFactor * dt;
                    player.dx -= speedFactor * dt;
            }
        }

        if (MIN_FACTOR_TO_SEE_MOVING <= player.dy || player.dy <= -MIN_FACTOR_TO_SEE_MOVING) {
            if (-SHIP_SPEED_BRAKE_FACTOR <= player.dy && player.dy <= SHIP_SPEED_BRAKE_FACTOR)
                player.dy = 0.0f;
            else {
//                if (player.dy * yAngle >= 0.0f)
                if (player.dy >= 0.0f)
//                    player.dy += yAngle * speedFactor * dt;
                    player.dy += speedFactor * dt;
                else
//                    player.dy -= yAngle * speedFactor * dt;
                    player.dy -= speedFactor * dt;
            }
        }
        return;
    }

    player.dx += xAngle * speedFactor * dt;
    player.dy += yAngle * speedFactor * dt;

    // limit max speed
    if (fabs(player.dx) > SHIP_MAX_SPEED)
        player.dx = player.dx > 0.0f ? SHIP_MAX_SPEED : -SHIP_MAX_SPEED;

    if (fabs(player.dy) > SHIP_MAX_SPEED)
        player.dy = player.dy > 0.0f ? SHIP_MAX_SPEED : -SHIP_MAX_SPEED;
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
    currentLevelScore += fromAsteroid.score;
    player.score += fromAsteroid.score;
    float newASteroidSize = fromAsteroid.size - ASTEROID_DOWNGRADE_FACTOR;

    // 6.0f size asteroid SPEED twice more than 10.0f size
    // 2.0f size asteroid SPEED four(4) times more than 10.0f size
    float newAsteroidSpeedScoreScaleFactor = (LARGE_ASTEROID_SIZE - newASteroidSize) / 2.0f;

    // 6.0f size asteroid SCORE twice more than 10.0f size
    // 2.0f size asteroid SCORE four(4) times more than 10.0f size
    float newAsteroidScore = newAsteroidSpeedScoreScaleFactor * LARGEST_ASTEROID_SCORE;
    if (newASteroidSize > 0.0f){
        int newAsteroidsCount = (int)(fromAsteroid.size / NEW_ASTEROIDS_COUNT_DIVIDER);
        for (int i = 0; i < newAsteroidsCount; i++){
            // ((float)i/(float)newAsteroidsCount) *
            float randomFloat = ((float)random()/(float)RAND_MAX);
            float newAngle = randomFloat * ((float)M_PI * 2.0f);
            float sinAngle = sinf(newAngle);
            float cosAngle = cosf(newAngle);

            int newX = fromAsteroid.x, newY = fromAsteroid.y;
            float newDx = LARGEST_ASTEROID_SPEED_FACTOR * newAsteroidSpeedScoreScaleFactor * sinAngle;
            float newDy = LARGEST_ASTEROID_SPEED_FACTOR * newAsteroidSpeedScoreScaleFactor * cosAngle;
            vecNewAsteroids.emplace_back(SpaceObject{newX, newY, newDx, newDy,
                                                     newASteroidSize, 0.0f, newAsteroidScore});
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

void init_models(){
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

    vecLiveModel = {
            {-5.0f, 0.0f},
            {2.5f, -2.5f},
            {1.0f, -1.0f},
            {1.0f, 1.0f},
            {2.5f, 2.5f}
    };

    vecZeroNumberModel = {
        {-2.0f, 2.0f},
        {-2.0f, -2.0f},
        {2.0f, -2.0f},
        {2.0f, 2.0f},
        {-2.0f, 2.0f}
    };

    vecOneNumberModel = {
        {-2.0f, 0.0f},
        {2.0f, 0.0f}
    };

    vecTwoNumberModel = {
        {-2.0f, -2.0f,},
        {-2.0f, 2.0f},
        {0.0f, 2.0f},
        {0.0f, -2.0f},
        {2.0f, -2.0f},
        {2.0f, 2.0f}
    };

    vecThreeNumberModel = {
        {-2.0f, -2.0f,},
        {-2.0f, 2.0f},
        {0.0f, -2.0f},
        {2.0f, 2.0f},
        {2.0f, -2.0f}
    };

    vecFourNumberModel = {
        {-2.0f, -2.0f,},
        {0.0f, -2.0f},
        {0.0f, 2.0f},
        {-2.0f, 2.0f},
        {2.0f, 2.0f}
    };

    vecFiveNumberModel = {
        {-2.0f, 2.0f,},
        {-2.0f, -2.0f},
        {0.0f, -2.0f},
        {0.0f, 2.0f},
        {2.0f, 2.0f},
        {2.0f, -2.0f}
    };

    vecSixNumberModel = {
        {-2.0f, 2.0f,},
        {-2.0f, -2.0f},
        {2.0f, -2.0f},
        {2.0f, 2.0f},
        {0.0f, 2.0f},
        {0.0f, -2.0f}
    };

    vecSevenNumberModel = {
        {0.0f, -2.0f,},
        {-2.0f, -2.0f},
        {-2.0f, 2.0f},
        {2.0f, 2.0f}
    };

    vecEightNumberModel = {
        {-2.0f, 2.0f},
        {-2.0f, -2.0f},
        {2.0f, -2.0f},
        {2.0f, 2.0f},
        {-2.0f, 2.0f},
        {0.0f, 2.0f},
        {0.0f, -2.0f}
    };

    vecNineNumberModel = {
        {0.0f, 2.0f},
        {0.0f, -2.0f},
        {-2.0f, -2.0f},
        {-2.0f, 2.0f},
        {2.0f, 2.0f},
        {2.0f, -2.0f}
    };

    vecNumbersModels = {vecZeroNumberModel, vecOneNumberModel, vecTwoNumberModel, vecThreeNumberModel,
                        vecFourNumberModel, vecFiveNumberModel, vecSixNumberModel, vecSevenNumberModel,
                        vecEightNumberModel, vecNineNumberModel};

    vecYCharModel = {
        {-2.0f, -2.0f},
        {0.0f, 0.0f},
        {-2.0f, 2.0f},
        {0.0f, 0.0f},
        {2.0f, 0.0f}
    };

    vecOCharModel = vecZeroNumberModel;

    vecUCharModel = {
        {-2.0f, -2.0f},
        {2.0f, -2.0f},
        {2.0f, 2.0f},
        {-2.0f, 2.0f}
    };

    vecWCharModel = {
        {-2.0f, -2.0f},
        {2.0f, -2.0f},
        {0.0f, 0.0f},
        {2.0f, 2.0f},
        {-2.0f, 2.0f}
    };

    vecICharModel = vecOneNumberModel;

    vecNCharModel = {
            {2.0f, -2.0f},
            {-2.0f, -2.0f},
            {2.0f, 2.0f},
            {-2.0f, 2.0f}
    };

    vecGCharModel = {
        {-2.0f, 2.0f},
        {-2.0f, -2.0f},
        {2.0f, -2.0f},
        {2.0f, 2.0f},
        {0.0f, 2.0f},
        {0.0f, 0.0f}
    };

    vecACharModel = {
        {2.0f, -2.0f},
        {-2.0f, 0.0f},
        {2.0f, 2.0f},
        {0.0f, 1.0f},
        {0.0f, -1.0f}
    };

    vecMCharModel = {
        {2.0f, -2.0f},
        {-2.0f, -2.0f},
        {0.0f, 0.0f},
        {-2.0f, 2.0f},
        {2.0f, 2.0f}
    };

    vecECharModel = {
        {-2.0f, 2.0f},
        {-2.0f, -2.0f},
        {0.0f, 0.0f},
        {2.0f, -2.0f},
        {2.0f, 2.0f}
    };

    vecVCharModel = {
        {-2.0f, -2.0f},
        {2.0f, 0.0f},
        {-2.0f, 2.0f}
    };

    vecRCharModel = {
        {2.0f, -2.0f},
        {-2.0f, -2.0f},
        {-2.0f, 2.0f},
        {0.0f, 2.0f},
        {0.0f, -1.0f},
        {2.0f, 2.0f},
    };
}


void draw_lives(){
    for (int i = 0; i < livesCount; i++){
        draw_poly_model(vecLiveModel, LIVES_X_POS, LIVES_Y_POS + 30 * i, LIVES_ROTATION, LIVES_SIZE, WHITE_COLOR);
    }
}

void draw_score(int score, int x, int y){
    std::string scoreString = std::to_string(score);
    for (int i = 0; i < scoreString.size() || i == 0; i++) {
        draw_number_or_char(vecNumbersModels[scoreString[i] - '0'],
                            x, y + 30 * i,
                            SCORE_ROTATION, SCORE_SIZE, WHITE_COLOR);
    }
}

void draw_game_over_message(){
    draw_number_or_char(vecGCharModel, TEXT_X, TEXT_Y, TEXT_ROTATION, TEXT_SIZE, WHITE_COLOR);
    draw_number_or_char(vecACharModel, TEXT_X, TEXT_Y + 110, TEXT_ROTATION, TEXT_SIZE, WHITE_COLOR);
    draw_number_or_char(vecMCharModel, TEXT_X, TEXT_Y + 220, TEXT_ROTATION, TEXT_SIZE, WHITE_COLOR);
    draw_number_or_char(vecECharModel, TEXT_X, TEXT_Y + 330, TEXT_ROTATION, TEXT_SIZE, WHITE_COLOR);

    draw_number_or_char(vecOCharModel, TEXT_X, TEXT_Y + 480, TEXT_ROTATION, TEXT_SIZE, WHITE_COLOR);
    draw_number_or_char(vecVCharModel, TEXT_X, TEXT_Y + 590, TEXT_ROTATION, TEXT_SIZE, WHITE_COLOR);
    draw_number_or_char(vecECharModel, TEXT_X, TEXT_Y + 700, TEXT_ROTATION, TEXT_SIZE, WHITE_COLOR);
    draw_number_or_char(vecRCharModel, TEXT_X, TEXT_Y + 810, TEXT_ROTATION, TEXT_SIZE, WHITE_COLOR);

}


void draw_win_message(){
    draw_number_or_char(vecYCharModel, TEXT_X, TEXT_Y + 110, TEXT_ROTATION, TEXT_SIZE, WHITE_COLOR);
    draw_number_or_char(vecOCharModel, TEXT_X, TEXT_Y + 220, TEXT_ROTATION, TEXT_SIZE, WHITE_COLOR);
    draw_number_or_char(vecUCharModel, TEXT_X, TEXT_Y + 330, TEXT_ROTATION, TEXT_SIZE, WHITE_COLOR);

    draw_number_or_char(vecWCharModel, TEXT_X, TEXT_Y + 480, TEXT_ROTATION, TEXT_SIZE, WHITE_COLOR);
    draw_number_or_char(vecICharModel, TEXT_X, TEXT_Y + 590, TEXT_ROTATION, TEXT_SIZE, WHITE_COLOR);
    draw_number_or_char(vecNCharModel, TEXT_X, TEXT_Y + 700, TEXT_ROTATION, TEXT_SIZE, WHITE_COLOR);
}

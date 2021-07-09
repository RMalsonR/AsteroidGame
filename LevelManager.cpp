#include "Engine.h"
#include "SpaceObjectUtils.h"
#include "LevelManager.h"

void set_player_at_start_pos(){
    player.x = SCREEN_HEIGHT / 2;
    player.y = SCREEN_WIDTH / 2;
    player.dx = 0.0f;
    player.dy = 0.0f;
    player.angle = 0.0f;
}


void reset_objects(){
    if (!vecAsteroids.empty())
        vecAsteroids.clear();
    if (!vecBullets.empty())
        vecBullets.clear();
    set_player_at_start_pos();
    levelTime = 0.0f;
}


void load_data_from_file(int levelCount){
    std::string currentPath = CURRENT_PATH;
    std::string fileName = LEVEL_FILE_NAME_TEMPLATE;
    fileName.insert(INSERT_POSITION_TO_FILE_NAME, std::to_string(levelCount));
    fileName.insert(0, currentPath);

    std::ifstream levelFile(fileName);
    std::string fileLine;

    // Skip headers
    std::getline(levelFile, fileLine);

    while (std::getline(levelFile, fileLine)){
        std::stringstream parseLine(fileLine);

        int x, y;
        float dx, dy, size, angle, score;
        char sep;

        parseLine >> x;
        parseLine >> sep;
        parseLine >> y;
        parseLine >> sep;
        parseLine >> dx;
        parseLine >> sep;
        parseLine >> dy;
        parseLine >> sep;
        parseLine >> size;
        parseLine >> sep;
        parseLine >> angle;
        parseLine >> sep;
        parseLine >> score;

        vecAsteroidsToLoad.emplace_back(SpaceObject{
            x, y, dx, dy, size, angle, score
        });

        parseLine.clear();
        parseLine.str("");
    }

    levelFile.close();
}


void copy_asteroid_from_loaded_vector(){
    for (auto asteroid : vecAsteroidsToLoad)
        vecAsteroids.emplace_back(asteroid);
}


void load_level(int levelCount, bool isNeedLoadFile = true){
    reset_objects();
    if (isNeedLoadFile) {
        vecAsteroidsToLoad.clear();
        load_data_from_file(levelCount);
    }
    copy_asteroid_from_loaded_vector();
}

void load_next_level(){
    currentLevel += 1;
    currentLevelScore = 0.0f;
    player.score += completeLevelScore / levelTime + ((float)livesCount * SCORE_FOR_EACH_LIVE);
    completeLevelScore *= LEVEL_SCORE_SCALE_FACTOR;
    if (currentLevel > MAX_LEVEL) {
        isWin = true;
        reset_objects();
    }
    else
        load_level(currentLevel);
}

void init_level(){
    load_level(currentLevel);
    player.size = SHIP_SIZE;
    player.score = 0;
}


void reset_level() {
    livesCount -= 1;
    player.score -= currentLevelScore;
    currentLevelScore = 0.0f;
    isDead = false;

    if (livesCount <= 0){
        isGameOver = true;
        reset_objects();
    }
    else
        load_level(currentLevel, false);
}


float get_high_score(){
    std::string fileName = HIGH_SCORE_FILE_NAME;
    float highScoreTemp = 0.0f;

    std::ifstream highScoreFile(fileName);
    std::string fileLine;

    std::getline(highScoreFile, fileLine);
    std::stringstream parseLine(fileLine);

    parseLine >> highScoreTemp;

    highScoreFile.close();
    parseLine.clear();
    parseLine.str("");
    return highScoreTemp;
}


void write_new_high_score(float newHighScore){
    std::string fileName = HIGH_SCORE_FILE_NAME;
    std::ofstream highScoreFile(fileName);

    highScoreFile << newHighScore;
    highScoreFile.close();
}


void check_and_write_new_high_score(){
    if (isWin){
        if (player.score > highScore)
            write_new_high_score(player.score);
    }
}


void reset_game(){
    check_and_write_new_high_score();
    isDead = false;
    isWin = false;
    isGameOver = false;
    currentLevel = 1;
    currentLevelScore = 0.0f;
    completeLevelScore = BEGIN_COMPLETE_LEVEL_SCORE;
    levelTime = 0.0f;
    livesCount = MAX_LIVES;
    reset_objects();
    init_level();
}


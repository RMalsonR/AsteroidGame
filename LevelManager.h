#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

extern std::vector<SpaceObject> vecAsteroidsToLoad;
extern int livesCount;
extern int currentLevel;

extern float completeLevelScore;
extern float currentLevelScore;
extern float highScore;

extern float levelTime;

extern bool isWin;
extern bool isGameOver;

#define MAX_LEVEL 5
#define MAX_LIVES 5

#define CURRENT_PATH std::__fs::filesystem::current_path()
#define LEVEL_FILE_NAME_TEMPLATE "/level.csv"
#define HIGH_SCORE_FILE_NAME "highScore.txt"
#define INSERT_POSITION_TO_FILE_NAME 6

#define SCORE_FOR_EACH_LIVE 250.0f
#define BEGIN_COMPLETE_LEVEL_SCORE 500.0f

#define LEVEL_SCORE_SCALE_FACTOR 1.5f

void reset_game();

void reset_level();

void init_level();

void load_next_level();

float get_high_score();

void write_new_high_score(float newHighScore);

void check_and_write_new_high_score();

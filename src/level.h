#pragma once

#include "constants.h"
#include "enemy.h"
#include <cstring>

// Tile types
constexpr int TILE_EMPTY = 0;
constexpr int TILE_SOLID = 1;
constexpr int TILE_PLATFORM = 2;  // One-way platform
constexpr int TILE_SPIKE = 3;
constexpr int TILE_LADDER = 4;

struct EnemySpawn {
    int type;
    float x, y;
};

struct Level {
    int tiles[MAP_ROWS][MAP_COLS];
    int bgColor[3] = {24, 24, 48};  // Background color
    int tileColor[3] = {80, 80, 120}; // Tile color
    int accentColor[3] = {120, 120, 180}; // Accent
    float playerStartX = 16;
    float playerStartY = 176;
    EnemySpawn enemies[32];
    int numEnemies = 0;
    int mapWidth = MAP_COLS; // Actual used width

    void clear() {
        memset(tiles, 0, sizeof(tiles));
        numEnemies = 0;
    }

    int getTile(int col, int row) const {
        if (col < 0 || col >= MAP_COLS || row < 0 || row >= MAP_ROWS) return TILE_SOLID;
        return tiles[row][col];
    }

    bool isSolid(int col, int row) const {
        int t = getTile(col, row);
        return t == TILE_SOLID;
    }

    bool isSpike(int col, int row) const {
        return getTile(col, row) == TILE_SPIKE;
    }

    void addEnemy(int type, float x, float y) {
        if (numEnemies < 32) {
            enemies[numEnemies++] = {type, x, y};
        }
    }
};

// Procedural level generation
namespace LevelGen {

    inline void generateFloor(Level& lvl, int startCol, int endCol, int row) {
        for (int c = startCol; c < endCol && c < MAP_COLS; c++) {
            for (int r = row; r < MAP_ROWS; r++) {
                lvl.tiles[r][c] = TILE_SOLID;
            }
        }
    }

    inline void generatePlatform(Level& lvl, int col, int row, int width) {
        for (int c = col; c < col + width && c < MAP_COLS; c++) {
            lvl.tiles[row][c] = TILE_SOLID;
        }
    }

    inline void generatePit(Level& lvl, int col, int width) {
        for (int c = col; c < col + width && c < MAP_COLS; c++) {
            for (int r = 0; r < MAP_ROWS; r++) {
                lvl.tiles[r][c] = TILE_EMPTY;
            }
        }
        // Spikes at the bottom
        for (int c = col; c < col + width && c < MAP_COLS; c++) {
            lvl.tiles[MAP_ROWS - 1][c] = TILE_SPIKE;
        }
    }

    inline void generateStage1(Level& lvl) {
        lvl.clear();
        lvl.bgColor[0] = 16; lvl.bgColor[1] = 16; lvl.bgColor[2] = 48;
        lvl.tileColor[0] = 60; lvl.tileColor[1] = 100; lvl.tileColor[2] = 140;
        lvl.accentColor[0] = 80; lvl.accentColor[1] = 140; lvl.accentColor[2] = 200;
        lvl.playerStartX = 24;
        lvl.playerStartY = 160;
        lvl.mapWidth = 60;

        // Ground floor
        generateFloor(lvl, 0, 12, 12);
        generateFloor(lvl, 15, 25, 12);
        generateFloor(lvl, 28, 40, 12);
        generateFloor(lvl, 43, 60, 12);

        // Pits
        generatePit(lvl, 12, 3);
        generatePit(lvl, 25, 3);
        generatePit(lvl, 40, 3);

        // Platforms
        generatePlatform(lvl, 8, 9, 3);
        generatePlatform(lvl, 13, 7, 2);
        generatePlatform(lvl, 20, 6, 4);
        generatePlatform(lvl, 32, 8, 3);
        generatePlatform(lvl, 36, 5, 2);
        generatePlatform(lvl, 45, 7, 3);

        // Walls / obstacles
        lvl.tiles[11][18] = TILE_SOLID;
        lvl.tiles[10][18] = TILE_SOLID;
        lvl.tiles[11][35] = TILE_SOLID;
        lvl.tiles[10][35] = TILE_SOLID;
        lvl.tiles[9][35] = TILE_SOLID;

        // Enemies
        lvl.addEnemy(ENEMY_MET, 7 * 16, 11 * 16);
        lvl.addEnemy(ENEMY_MET, 22 * 16, 11 * 16);
        lvl.addEnemy(ENEMY_SNIPER, 16 * 16, 10 * 16 - 8);
        lvl.addEnemy(ENEMY_FLYING, 30 * 16, 6 * 16);
        lvl.addEnemy(ENEMY_MET, 34 * 16, 11 * 16);
        lvl.addEnemy(ENEMY_SNIPER, 46 * 16, 10 * 16 - 8);
        lvl.addEnemy(ENEMY_FLYING, 50 * 16, 5 * 16);
    }

    inline void generateStage2(Level& lvl) {
        lvl.clear();
        lvl.bgColor[0] = 40; lvl.bgColor[1] = 8; lvl.bgColor[2] = 8;
        lvl.tileColor[0] = 160; lvl.tileColor[1] = 60; lvl.tileColor[2] = 40;
        lvl.accentColor[0] = 200; lvl.accentColor[1] = 100; lvl.accentColor[2] = 60;
        lvl.playerStartX = 24;
        lvl.playerStartY = 160;
        lvl.mapWidth = 60;

        // More vertical layout with platforms
        generateFloor(lvl, 0, 8, 12);
        generateFloor(lvl, 10, 18, 13);
        generateFloor(lvl, 22, 35, 12);
        generateFloor(lvl, 38, 50, 13);
        generateFloor(lvl, 52, 60, 12);

        generatePit(lvl, 8, 2);
        generatePit(lvl, 18, 4);
        generatePit(lvl, 35, 3);
        generatePit(lvl, 50, 2);

        // Staircase platforms
        generatePlatform(lvl, 4, 10, 2);
        generatePlatform(lvl, 6, 8, 2);
        generatePlatform(lvl, 12, 9, 3);
        generatePlatform(lvl, 15, 7, 2);
        generatePlatform(lvl, 24, 8, 3);
        generatePlatform(lvl, 28, 6, 2);
        generatePlatform(lvl, 31, 9, 3);
        generatePlatform(lvl, 40, 9, 2);
        generatePlatform(lvl, 44, 7, 3);
        generatePlatform(lvl, 54, 9, 3);

        // Enemies
        lvl.addEnemy(ENEMY_SNIPER, 5 * 16, 10 * 16 - 8);
        lvl.addEnemy(ENEMY_MET, 14 * 16, 12 * 16);
        lvl.addEnemy(ENEMY_FLYING, 20 * 16, 4 * 16);
        lvl.addEnemy(ENEMY_SNIPER, 26 * 16, 10 * 16 - 8);
        lvl.addEnemy(ENEMY_MET, 30 * 16, 11 * 16);
        lvl.addEnemy(ENEMY_FLYING, 36 * 16, 5 * 16);
        lvl.addEnemy(ENEMY_SNIPER, 42 * 16, 11 * 16 - 8);
        lvl.addEnemy(ENEMY_MET, 48 * 16, 12 * 16);
    }

    inline void generateStage3(Level& lvl) {
        lvl.clear();
        lvl.bgColor[0] = 8; lvl.bgColor[1] = 32; lvl.bgColor[2] = 8;
        lvl.tileColor[0] = 40; lvl.tileColor[1] = 140; lvl.tileColor[2] = 50;
        lvl.accentColor[0] = 60; lvl.accentColor[1] = 200; lvl.accentColor[2] = 80;
        lvl.playerStartX = 24;
        lvl.playerStartY = 160;
        lvl.mapWidth = 60;

        // Complex layout
        generateFloor(lvl, 0, 6, 12);
        generateFloor(lvl, 9, 16, 13);
        generateFloor(lvl, 19, 30, 12);
        generateFloor(lvl, 33, 42, 11);
        generateFloor(lvl, 45, 60, 12);

        generatePit(lvl, 6, 3);
        generatePit(lvl, 16, 3);
        generatePit(lvl, 30, 3);
        generatePit(lvl, 42, 3);

        // Higher platforms
        generatePlatform(lvl, 2, 9, 3);
        generatePlatform(lvl, 7, 6, 2);
        generatePlatform(lvl, 11, 8, 3);
        generatePlatform(lvl, 21, 7, 4);
        generatePlatform(lvl, 26, 5, 3);
        generatePlatform(lvl, 35, 7, 3);
        generatePlatform(lvl, 39, 5, 2);
        generatePlatform(lvl, 47, 8, 3);
        generatePlatform(lvl, 52, 6, 3);

        // Enemies
        lvl.addEnemy(ENEMY_MET, 4 * 16, 11 * 16);
        lvl.addEnemy(ENEMY_FLYING, 8 * 16, 3 * 16);
        lvl.addEnemy(ENEMY_SNIPER, 12 * 16, 11 * 16 - 8);
        lvl.addEnemy(ENEMY_MET, 23 * 16, 11 * 16);
        lvl.addEnemy(ENEMY_FLYING, 28 * 16, 4 * 16);
        lvl.addEnemy(ENEMY_SNIPER, 36 * 16, 9 * 16 - 8);
        lvl.addEnemy(ENEMY_FLYING, 41 * 16, 3 * 16);
        lvl.addEnemy(ENEMY_MET, 49 * 16, 11 * 16);
        lvl.addEnemy(ENEMY_SNIPER, 55 * 16, 10 * 16 - 8);
    }

    inline void generateBossRoom(Level& lvl) {
        lvl.clear();
        lvl.bgColor[0] = 32; lvl.bgColor[1] = 0; lvl.bgColor[2] = 48;
        lvl.tileColor[0] = 120; lvl.tileColor[1] = 40; lvl.tileColor[2] = 160;
        lvl.accentColor[0] = 180; lvl.accentColor[1] = 80; lvl.accentColor[2] = 220;
        lvl.playerStartX = 24;
        lvl.playerStartY = 160;
        lvl.mapWidth = 16; // Single screen boss room

        // Floor
        generateFloor(lvl, 0, 16, 12);

        // Walls
        for (int r = 0; r < MAP_ROWS; r++) {
            lvl.tiles[r][0] = TILE_SOLID;
            lvl.tiles[r][15] = TILE_SOLID;
        }
        // Ceiling
        for (int c = 0; c < 16; c++) {
            lvl.tiles[0][c] = TILE_SOLID;
        }

        // Small platforms
        generatePlatform(lvl, 3, 8, 3);
        generatePlatform(lvl, 10, 8, 3);

        // Boss
        lvl.addEnemy(ENEMY_BOSS, 10 * 16, 9 * 16);
    }

    inline void generate(Level& lvl, int stageIndex) {
        switch (stageIndex) {
            case 0: generateStage1(lvl); break;
            case 1: generateStage2(lvl); break;
            case 2: generateStage3(lvl); break;
            case 3: generateBossRoom(lvl); break;
            default: generateStage1(lvl); break;
        }
    }
}

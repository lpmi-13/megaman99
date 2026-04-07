// ============================================================
// MEGAMAN99
// A Megaman clone for the Miyoo Mini platform
// Built with SDL2 - no external assets required
// ============================================================

#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include "constants.h"
#include "renderer.h"
#include "input.h"
#include "audio.h"
#include "player.h"
#include "enemy.h"
#include "bullet.h"
#include "particle.h"
#include "level.h"
#include "draw.h"

// ============================================================
// Game State
// ============================================================
struct Game {
    int state = STATE_TITLE;
    int currentStage = 0;
    int stageSelectCursor = 0;
    int totalStages = 4; // 3 stages + boss
    int stagesCleared = 0;
    bool stageComplete[4] = {};
    int transitionTimer = 0;
    int titleBlink = 0;

    Player player;
    Level level;
    Enemy enemies[32];
    int numEnemies = 0;
    Bullet enemyBullets[16];
    Particle particles[64];

    Renderer renderer;
    Input input;
    Audio audio;

    float cameraX = 0;
    bool running = true;

    bool init() {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
            fprintf(stderr, "SDL init failed: %s\n", SDL_GetError());
            return false;
        }

        if (!renderer.init()) return false;
        if (!audio.init()) return false;
        audio.generateSounds();

        srand(static_cast<unsigned>(time(nullptr)));
        return true;
    }

    void loadStage(int stage) {
        currentStage = stage;
        LevelGen::generate(level, stage);

        // Spawn enemies
        numEnemies = level.numEnemies;
        for (int i = 0; i < numEnemies && i < 32; i++) {
            enemies[i].spawn(level.enemies[i].type,
                           level.enemies[i].x, level.enemies[i].y);
        }

        // Reset enemy bullets
        for (auto& b : enemyBullets) b.active = false;

        // Reset particles
        for (auto& p : particles) p.active = false;

        // Place player
        player.reset(level.playerStartX, level.playerStartY);
        cameraX = 0;
    }

    void spawnParticles(float x, float y, int count, int r, int g, int b) {
        for (int i = 0; i < count; i++) {
            for (auto& p : particles) {
                if (!p.active) {
                    p.spawn(x, y, r, g, b);
                    break;
                }
            }
        }
    }

    // --------------------------------------------------------
    // Update
    // --------------------------------------------------------
    void update() {
        switch (state) {
            case STATE_TITLE: updateTitle(); break;
            case STATE_STAGE_SELECT: updateStageSelect(); break;
            case STATE_PLAYING: updatePlaying(); break;
            case STATE_PAUSED: updatePaused(); break;
            case STATE_GAME_OVER: updateGameOver(); break;
            case STATE_STAGE_CLEAR: updateStageClear(); break;
        }
    }

    void updateTitle() {
        titleBlink++;
        if (input.isPressed(Input::BTN_START) || input.isPressed(Input::BTN_JUMP)) {
            state = STATE_STAGE_SELECT;
            audio.playMenu();
        }
        if (input.isPressed(Input::BTN_PAUSE)) {
            running = false;
        }
    }

    void updateStageSelect() {
        if (input.isPressed(Input::BTN_LEFT)) {
            stageSelectCursor = (stageSelectCursor + totalStages - 1) % totalStages;
            audio.playMenu();
        }
        if (input.isPressed(Input::BTN_RIGHT)) {
            stageSelectCursor = (stageSelectCursor + 1) % totalStages;
            audio.playMenu();
        }
        if (input.isPressed(Input::BTN_UP)) {
            stageSelectCursor = (stageSelectCursor + totalStages - 2) % totalStages;
            audio.playMenu();
        }
        if (input.isPressed(Input::BTN_DOWN)) {
            stageSelectCursor = (stageSelectCursor + 2) % totalStages;
            audio.playMenu();
        }

        if (input.isPressed(Input::BTN_START) || input.isPressed(Input::BTN_JUMP)) {
            // Boss room only available after clearing 3 stages
            if (stageSelectCursor == 3 && stagesCleared < 3) {
                // Can't enter boss yet
                audio.playHit();
            } else {
                loadStage(stageSelectCursor);
                state = STATE_PLAYING;
                audio.playMenu();
            }
        }
        if (input.isPressed(Input::BTN_PAUSE)) {
            state = STATE_TITLE;
        }
    }

    void updatePlaying() {
        if (input.isPressed(Input::BTN_PAUSE)) {
            state = STATE_PAUSED;
            return;
        }

        // Update player
        player.update(input, level, audio);

        // Camera follows player
        float targetCamX = player.pos.x - GAME_WIDTH / 2 + PLAYER_WIDTH / 2;
        float maxCam = (level.mapWidth * TILE_SIZE) - GAME_WIDTH;
        if (maxCam < 0) maxCam = 0;
        if (targetCamX < 0) targetCamX = 0;
        if (targetCamX > maxCam) targetCamX = maxCam;
        cameraX += (targetCamX - cameraX) * 0.1f;

        // Update enemies
        for (int i = 0; i < numEnemies; i++) {
            if (!enemies[i].active) continue;

            bool wantsToShoot = enemies[i].update(player.pos.x, player.pos.y);

            if (wantsToShoot) {
                // Find free enemy bullet
                for (auto& b : enemyBullets) {
                    if (!b.active) {
                        float bx = enemies[i].pos.x + enemies[i].width / 2;
                        float by = enemies[i].pos.y + enemies[i].height / 2;
                        b.spawn(bx, by, enemies[i].getShootDirX(), enemies[i].getShootDirY(), false);
                        b.damage = (enemies[i].type == ENEMY_BOSS) ? 3 : 2;
                        break;
                    }
                }
            }

            // Enemy-player collision
            if (player.alive && player.invincibleTimer == 0) {
                if (player.getRect().overlaps(enemies[i].getRect())) {
                    int dmg = (enemies[i].type == ENEMY_BOSS) ? 4 : 2;
                    player.takeDamage(dmg, audio);
                }
            }
        }

        // Update enemy bullets
        for (auto& b : enemyBullets) {
            if (!b.active) continue;
            b.update();

            // Check vs level
            int col = static_cast<int>(b.pos.x + b.width / 2) / TILE_SIZE;
            int row = static_cast<int>(b.pos.y + b.height / 2) / TILE_SIZE;
            if (level.isSolid(col, row)) {
                b.active = false;
                continue;
            }

            // Check vs player
            if (player.alive && player.invincibleTimer == 0 && b.getRect().overlaps(player.getRect())) {
                player.takeDamage(b.damage, audio);
                b.active = false;
            }
        }

        // Player bullets vs enemies
        for (auto& pb : player.bullets) {
            if (!pb.active) continue;
            for (int i = 0; i < numEnemies; i++) {
                if (!enemies[i].active) continue;
                if (pb.getRect().overlaps(enemies[i].getRect())) {
                    enemies[i].takeDamage(pb.damage);
                    pb.active = false;

                    if (enemies[i].type == ENEMY_BOSS) {
                        audio.playBossHit();
                    } else {
                        audio.playHit();
                    }

                    if (!enemies[i].active) {
                        // Enemy destroyed - spawn particles
                        spawnParticles(
                            enemies[i].pos.x + enemies[i].width / 2,
                            enemies[i].pos.y + enemies[i].height / 2,
                            12, 255, 200, 60);

                        // Check if boss was defeated
                        if (enemies[i].type == ENEMY_BOSS) {
                            state = STATE_STAGE_CLEAR;
                            transitionTimer = 0;
                            audio.playStageClear();
                        }
                    }
                    break;
                }
            }
        }

        // Update particles
        for (auto& p : particles) p.update();

        // Check player death
        if (!player.alive && player.deathTimer > DEATH_ANIMATION_FRAMES) {
            if (player.lives <= 0) {
                state = STATE_GAME_OVER;
                transitionTimer = 0;
            } else {
                // Respawn
                player.reset(level.playerStartX, level.playerStartY);
                cameraX = 0;
                // Reset enemies
                for (int i = 0; i < numEnemies; i++) {
                    enemies[i].spawn(level.enemies[i].type,
                                   level.enemies[i].x, level.enemies[i].y);
                }
                for (auto& b : enemyBullets) b.active = false;
            }
        }

        // Stage end check (reached right edge of non-boss stages)
        if (currentStage < 3 && player.alive) {
            if (player.pos.x >= (level.mapWidth - 2) * TILE_SIZE) {
                state = STATE_STAGE_CLEAR;
                transitionTimer = 0;
                audio.playStageClear();
            }
        }
    }

    void updatePaused() {
        if (input.isPressed(Input::BTN_PAUSE) || input.isPressed(Input::BTN_START)) {
            state = STATE_PLAYING;
        }
    }

    void updateGameOver() {
        transitionTimer++;
        if (transitionTimer > 180) {
            if (input.isPressed(Input::BTN_START) || input.isPressed(Input::BTN_JUMP)) {
                // Reset game
                player.lives = 3;
                player.hp = PLAYER_MAX_HP;
                stagesCleared = 0;
                for (int i = 0; i < 4; i++) stageComplete[i] = false;
                state = STATE_TITLE;
            }
        }
    }

    void updateStageClear() {
        transitionTimer++;
        if (transitionTimer > 120) {
            if (!stageComplete[currentStage]) {
                stageComplete[currentStage] = true;
                stagesCleared++;
            }
            if (currentStage == 3) {
                // Beat the game!
                // Show credits / return to title after delay
                if (transitionTimer > 300) {
                    state = STATE_TITLE;
                    stagesCleared = 0;
                    for (int i = 0; i < 4; i++) stageComplete[i] = false;
                    player.lives = 3;
                }
            } else {
                state = STATE_STAGE_SELECT;
            }
        }
    }

    // --------------------------------------------------------
    // Render
    // --------------------------------------------------------
    void render() {
        renderer.beginFrame();

        switch (state) {
            case STATE_TITLE: renderTitle(); break;
            case STATE_STAGE_SELECT: renderStageSelect(); break;
            case STATE_PLAYING:
            case STATE_PAUSED:
                renderPlaying();
                if (state == STATE_PAUSED) renderPaused();
                break;
            case STATE_GAME_OVER: renderGameOver(); break;
            case STATE_STAGE_CLEAR: renderStageClear(); break;
        }

        renderer.endFrame();
    }

    void renderTitle() {
        // Background
        renderer.drawRect(0, 0, GAME_WIDTH, GAME_HEIGHT, 8, 8, 32);

        // Stars
        srand(42); // Fixed seed for consistent stars
        for (int i = 0; i < 40; i++) {
            int sx = rand() % GAME_WIDTH;
            int sy = rand() % (GAME_HEIGHT - 60);
            int bright = 100 + rand() % 155;
            renderer.drawRect(sx, sy, 1, 1, bright, bright, bright);
        }
        srand(static_cast<unsigned>(time(nullptr)));

        // Title
        Draw::textCentered(renderer, 40, "MEGAMAN99", COL_MEGA_LIGHT_R, COL_MEGA_LIGHT_G, COL_MEGA_LIGHT_B, 3);

        // Megaman silhouette
        int cx = GAME_WIDTH / 2 - 16;
        int cy = 90;
        renderer.drawRect(cx + 6, cy, 20, 6, COL_MEGA_R, COL_MEGA_G, COL_MEGA_B);
        renderer.drawRect(cx + 4, cy + 6, 24, 8, COL_MEGA_R, COL_MEGA_G, COL_MEGA_B);
        renderer.drawRect(cx + 8, cy + 14, 16, 12, COL_MEGA_R, COL_MEGA_G, COL_MEGA_B);
        renderer.drawRect(cx + 2, cy + 16, 8, 8, COL_MEGA_LIGHT_R, COL_MEGA_LIGHT_G, COL_MEGA_LIGHT_B);
        renderer.drawRect(cx + 22, cy + 16, 10, 6, COL_MEGA_LIGHT_R, COL_MEGA_LIGHT_G, COL_MEGA_LIGHT_B);
        renderer.drawRect(cx + 6, cy + 26, 8, 10, COL_MEGA_R, COL_MEGA_G, COL_MEGA_B);
        renderer.drawRect(cx + 18, cy + 26, 8, 10, COL_MEGA_R, COL_MEGA_G, COL_MEGA_B);

        // Blinking text
        if ((titleBlink / 30) % 2 == 0) {
            Draw::textCentered(renderer, 170, "PRESS START", 255, 255, 255, 2);
        }

        // Credits
        Draw::textCentered(renderer, 210, "A MEGAMAN CLONE", 140, 140, 140, 1);
        Draw::textCentered(renderer, 222, "FOR MIYOO MINI", 140, 140, 140, 1);
    }

    void renderStageSelect() {
        renderer.drawRect(0, 0, GAME_WIDTH, GAME_HEIGHT, 16, 16, 48);

        Draw::textCentered(renderer, 16, "STAGE SELECT", 255, 255, 255, 2);

        // Stage grid (2x2)
        const char* stageNames[] = {"ELEC MAN", "FIRE MAN", "WOOD MAN", "DR WILY"};
        int stageColors[][3] = {
            {60, 100, 200}, // Blue
            {200, 60, 40},  // Red
            {40, 160, 60},  // Green
            {160, 40, 200}  // Purple
        };

        for (int i = 0; i < totalStages; i++) {
            int gx = (i % 2) * 120 + 20;
            int gy = (i / 2) * 80 + 60;

            // Box
            int cr = stageColors[i][0];
            int cg = stageColors[i][1];
            int cb = stageColors[i][2];

            if (i == stageSelectCursor) {
                // Selected - bright outline
                renderer.drawRectOutline(gx - 2, gy - 2, 104, 64, 255, 255, 255);
                renderer.drawRect(gx, gy, 100, 60, cr, cg, cb);
            } else {
                renderer.drawRect(gx, gy, 100, 60, cr / 2, cg / 2, cb / 2);
            }

            // Check mark if completed
            if (stageComplete[i]) {
                renderer.drawRect(gx + 80, gy + 4, 12, 8, 255, 255, 60);
            }

            // Locked indicator for boss
            if (i == 3 && stagesCleared < 3) {
                Draw::text(renderer, gx + 20, gy + 20, "LOCKED", 120, 120, 120, 1);
            } else {
                // Stage name
                Draw::text(renderer, gx + 8, gy + 24, stageNames[i], 255, 255, 255, 1);
            }
        }

        // Instructions
        Draw::textCentered(renderer, 220, "PRESS A TO START", 180, 180, 180, 1);
    }

    void renderPlaying() {
        // Background color
        renderer.drawRect(0, 0, GAME_WIDTH, GAME_HEIGHT,
            level.bgColor[0], level.bgColor[1], level.bgColor[2]);

        // Background decorations (parallax stars/dots)
        int bgScroll = static_cast<int>(cameraX * 0.3f);
        srand(currentStage * 100 + 7);
        for (int i = 0; i < 20; i++) {
            int sx = (rand() % (GAME_WIDTH + 100)) - (bgScroll % (GAME_WIDTH + 100));
            int sy = rand() % GAME_HEIGHT;
            if (sx < 0) sx += GAME_WIDTH + 100;
            if (sx < GAME_WIDTH) {
                renderer.drawRect(sx, sy, 1, 1,
                    level.bgColor[0] + 30, level.bgColor[1] + 30, level.bgColor[2] + 30);
            }
        }
        srand(static_cast<unsigned>(time(nullptr)));

        int camX = static_cast<int>(cameraX);

        // Level tiles
        Draw::level(renderer, level, camX);

        // Enemies
        for (int i = 0; i < numEnemies; i++) {
            Draw::enemy(renderer, enemies[i], camX);
        }

        // Player
        Draw::megaman(renderer, player, camX);

        // Player bullets
        for (const auto& b : player.bullets) {
            Draw::bullet(renderer, b, camX);
        }

        // Enemy bullets
        for (const auto& b : enemyBullets) {
            Draw::bullet(renderer, b, camX);
        }

        // Particles
        for (const auto& p : particles) {
            Draw::particle(renderer, p, camX);
        }

        // Death explosion
        if (!player.alive) {
            int dt = player.deathTimer;
            if (dt < DEATH_ANIMATION_FRAMES) {
                int px = static_cast<int>(player.pos.x) - camX + PLAYER_WIDTH / 2;
                int py = static_cast<int>(player.pos.y) + PLAYER_HEIGHT / 2;
                for (int i = 0; i < 8; i++) {
                    float angle = i * 3.14159f * 2.0f / 8.0f;
                    float dist = dt * 2.0f;
                    int ex = px + static_cast<int>(cosf(angle) * dist);
                    int ey = py + static_cast<int>(sinf(angle) * dist);
                    renderer.drawRect(ex - 2, ey - 2, 4, 4,
                        COL_MEGA_LIGHT_R, COL_MEGA_LIGHT_G, COL_MEGA_LIGHT_B);
                }
            }
        }

        // HUD
        Draw::hud(renderer, player, currentStage);
    }

    void renderPaused() {
        // Semi-transparent overlay
        renderer.drawRect(0, 0, GAME_WIDTH, GAME_HEIGHT, 0, 0, 0, 128);
        Draw::textCentered(renderer, 100, "PAUSED", 255, 255, 255, 3);
        Draw::textCentered(renderer, 140, "PRESS START", 180, 180, 180, 1);
    }

    void renderGameOver() {
        renderer.drawRect(0, 0, GAME_WIDTH, GAME_HEIGHT, 0, 0, 0);

        int alpha = (transitionTimer < 60) ? transitionTimer * 4 : 255;
        (void)alpha; // Using full brightness text instead

        Draw::textCentered(renderer, 80, "GAME OVER", 200, 40, 40, 3);

        if (transitionTimer > 180) {
            Draw::textCentered(renderer, 160, "PRESS START", 180, 180, 180, 1);
            Draw::textCentered(renderer, 180, "TO CONTINUE", 180, 180, 180, 1);
        }
    }

    void renderStageClear() {
        renderPlaying();

        // Overlay
        renderer.drawRect(0, 0, GAME_WIDTH, GAME_HEIGHT, 0, 0, 0, 160);

        if (currentStage == 3) {
            // Beat the game
            Draw::textCentered(renderer, 60, "CONGRATULATIONS", 255, 220, 60, 2);
            Draw::textCentered(renderer, 100, "YOU DEFEATED", 255, 255, 255, 2);
            Draw::textCentered(renderer, 130, "DR WILY", 200, 60, 240, 2);
            Draw::textCentered(renderer, 180, "THANK YOU", COL_MEGA_LIGHT_R, COL_MEGA_LIGHT_G, COL_MEGA_LIGHT_B, 2);
            Draw::textCentered(renderer, 210, "FOR PLAYING", COL_MEGA_LIGHT_R, COL_MEGA_LIGHT_G, COL_MEGA_LIGHT_B, 2);
        } else {
            Draw::textCentered(renderer, 80, "STAGE CLEAR", 255, 220, 60, 3);

            if (transitionTimer > 60) {
                Draw::textCentered(renderer, 140, "READY FOR", 255, 255, 255, 1);
                Draw::textCentered(renderer, 155, "NEXT STAGE", 255, 255, 255, 1);
            }
        }
    }

    void shutdown() {
        audio.shutdown();
        renderer.shutdown();
        SDL_Quit();
    }
};

// ============================================================
// Main Entry Point
// ============================================================
int main(int /*argc*/, char* /*argv*/[]) {
    Game game;

    if (!game.init()) {
        fprintf(stderr, "Failed to initialize game!\n");
        return 1;
    }

    printf("Megaman99 started!\n");
    printf("Controls: Arrow keys = Move, Space/Ctrl = Jump, Z/Alt = Shoot\n");
    printf("Enter = Start, Escape = Pause/Menu\n");

    Uint32 frameStart;
    int frameTime;

    while (game.running) {
        frameStart = SDL_GetTicks();

        // Handle events
        SDL_Event e;
        game.input.update();
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                game.running = false;
            }
            game.input.handleEvent(e);
        }

        game.update();
        game.render();

        // Frame rate limiting
        frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    game.shutdown();
    printf("Megaman99 exited cleanly.\n");
    return 0;
}

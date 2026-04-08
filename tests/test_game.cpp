// ============================================================
// Megaman99 - Unit Tests
// Minimal test framework (no external dependencies)
// ============================================================

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

// Pull in game headers (no SDL dependencies in these)
#include "../src/vec2.h"
#include "../src/rect.h"
#include "../src/constants.h"
#include "../src/bullet.h"
#include "../src/particle.h"
#include "../src/enemy.h"
#include "../src/level.h"

// ============================================================
// Minimal test framework
// ============================================================
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) static void test_##name()
#define RUN_TEST(name) do { \
    tests_run++; \
    printf("  %-50s ", #name); \
    try { test_##name(); tests_passed++; printf("[PASS]\n"); } \
    catch (...) { tests_failed++; printf("[FAIL] (exception)\n"); } \
} while(0)

#define ASSERT_TRUE(expr) do { if (!(expr)) { \
    printf("[FAIL]\n    line %d: ASSERT_TRUE(%s)\n", __LINE__, #expr); \
    tests_failed++; return; } } while(0)

#define ASSERT_FALSE(expr) do { if (expr) { \
    printf("[FAIL]\n    line %d: ASSERT_FALSE(%s)\n", __LINE__, #expr); \
    tests_failed++; return; } } while(0)

#define ASSERT_EQ(a, b) do { if ((a) != (b)) { \
    printf("[FAIL]\n    line %d: ASSERT_EQ(%s, %s)\n", __LINE__, #a, #b); \
    tests_failed++; return; } } while(0)

#define ASSERT_NEAR(a, b, eps) do { if (fabs((a) - (b)) > (eps)) { \
    printf("[FAIL]\n    line %d: ASSERT_NEAR(%s, %s) got %f vs %f\n", __LINE__, #a, #b, (double)(a), (double)(b)); \
    tests_failed++; return; } } while(0)

#define SUITE(name) do { printf("\n[%s]\n", name); } while(0)

// ============================================================
// Vec2 Tests
// ============================================================
TEST(vec2_default_construction) {
    Vec2 v;
    ASSERT_NEAR(v.x, 0.0f, 0.001f);
    ASSERT_NEAR(v.y, 0.0f, 0.001f);
}

TEST(vec2_parameterized_construction) {
    Vec2 v(3.0f, 4.0f);
    ASSERT_NEAR(v.x, 3.0f, 0.001f);
    ASSERT_NEAR(v.y, 4.0f, 0.001f);
}

TEST(vec2_addition) {
    Vec2 a(1.0f, 2.0f);
    Vec2 b(3.0f, 4.0f);
    Vec2 c = a + b;
    ASSERT_NEAR(c.x, 4.0f, 0.001f);
    ASSERT_NEAR(c.y, 6.0f, 0.001f);
}

TEST(vec2_subtraction) {
    Vec2 a(5.0f, 7.0f);
    Vec2 b(2.0f, 3.0f);
    Vec2 c = a - b;
    ASSERT_NEAR(c.x, 3.0f, 0.001f);
    ASSERT_NEAR(c.y, 4.0f, 0.001f);
}

TEST(vec2_scalar_multiply) {
    Vec2 v(2.0f, 3.0f);
    Vec2 r = v * 2.5f;
    ASSERT_NEAR(r.x, 5.0f, 0.001f);
    ASSERT_NEAR(r.y, 7.5f, 0.001f);
}

TEST(vec2_plus_equals) {
    Vec2 a(1.0f, 2.0f);
    a += Vec2(3.0f, 4.0f);
    ASSERT_NEAR(a.x, 4.0f, 0.001f);
    ASSERT_NEAR(a.y, 6.0f, 0.001f);
}

TEST(vec2_minus_equals) {
    Vec2 a(5.0f, 7.0f);
    a -= Vec2(2.0f, 3.0f);
    ASSERT_NEAR(a.x, 3.0f, 0.001f);
    ASSERT_NEAR(a.y, 4.0f, 0.001f);
}

// ============================================================
// Rect Tests
// ============================================================
TEST(rect_default_construction) {
    Rect r;
    ASSERT_NEAR(r.x, 0.0f, 0.001f);
    ASSERT_NEAR(r.w, 0.0f, 0.001f);
}

TEST(rect_parameterized_construction) {
    Rect r(10.0f, 20.0f, 30.0f, 40.0f);
    ASSERT_NEAR(r.x, 10.0f, 0.001f);
    ASSERT_NEAR(r.y, 20.0f, 0.001f);
    ASSERT_NEAR(r.w, 30.0f, 0.001f);
    ASSERT_NEAR(r.h, 40.0f, 0.001f);
}

TEST(rect_right_bottom) {
    Rect r(10.0f, 20.0f, 30.0f, 40.0f);
    ASSERT_NEAR(r.right(), 40.0f, 0.001f);
    ASSERT_NEAR(r.bottom(), 60.0f, 0.001f);
}

TEST(rect_center) {
    Rect r(10.0f, 20.0f, 30.0f, 40.0f);
    ASSERT_NEAR(r.centerX(), 25.0f, 0.001f);
    ASSERT_NEAR(r.centerY(), 40.0f, 0.001f);
}

TEST(rect_overlaps_true) {
    Rect a(0, 0, 10, 10);
    Rect b(5, 5, 10, 10);
    ASSERT_TRUE(a.overlaps(b));
    ASSERT_TRUE(b.overlaps(a));
}

TEST(rect_overlaps_false_no_contact) {
    Rect a(0, 0, 10, 10);
    Rect b(20, 20, 10, 10);
    ASSERT_FALSE(a.overlaps(b));
    ASSERT_FALSE(b.overlaps(a));
}

TEST(rect_overlaps_edge_touching_is_false) {
    // Exactly touching edges should NOT overlap (uses strict inequality)
    Rect a(0, 0, 10, 10);
    Rect b(10, 0, 10, 10);
    ASSERT_FALSE(a.overlaps(b));
}

TEST(rect_overlaps_contained) {
    Rect outer(0, 0, 100, 100);
    Rect inner(25, 25, 10, 10);
    ASSERT_TRUE(outer.overlaps(inner));
    ASSERT_TRUE(inner.overlaps(outer));
}

TEST(rect_zero_width_inside_overlaps) {
    // Zero-width rect positioned inside another still reports overlap
    Rect a(5, 5, 0, 10);
    Rect b(0, 0, 10, 10);
    ASSERT_TRUE(a.overlaps(b));
}

TEST(rect_zero_width_outside_no_overlap) {
    // Zero-width rect positioned outside does not overlap
    Rect a(15, 5, 0, 10);
    Rect b(0, 0, 10, 10);
    ASSERT_FALSE(a.overlaps(b));
}

TEST(rect_zero_size_at_origin_no_overlap) {
    // Fully degenerate rect at origin: 0+0 > 0 is false, so no overlap
    Rect a(0, 0, 0, 0);
    Rect b(0, 0, 10, 10);
    ASSERT_FALSE(a.overlaps(b));
}

// ============================================================
// Bullet Tests
// ============================================================
TEST(bullet_default_inactive) {
    Bullet b;
    ASSERT_FALSE(b.active);
    ASSERT_TRUE(b.fromPlayer);
    ASSERT_EQ(b.damage, 1);
}

TEST(bullet_spawn) {
    Bullet b;
    b.spawn(10.0f, 20.0f, 5.0f, 0.0f, true);
    ASSERT_TRUE(b.active);
    ASSERT_TRUE(b.fromPlayer);
    ASSERT_NEAR(b.pos.x, 10.0f, 0.001f);
    ASSERT_NEAR(b.pos.y, 20.0f, 0.001f);
    ASSERT_NEAR(b.vel.x, 5.0f, 0.001f);
    ASSERT_EQ(b.lifetime, 0);
}

TEST(bullet_spawn_enemy) {
    Bullet b;
    b.spawn(10.0f, 20.0f, -3.0f, 0.0f, false);
    ASSERT_TRUE(b.active);
    ASSERT_FALSE(b.fromPlayer);
}

TEST(bullet_update_moves) {
    Bullet b;
    b.spawn(0.0f, 0.0f, 5.0f, 2.0f);
    b.update();
    ASSERT_NEAR(b.pos.x, 5.0f, 0.001f);
    ASSERT_NEAR(b.pos.y, 2.0f, 0.001f);
    ASSERT_EQ(b.lifetime, 1);
    ASSERT_TRUE(b.active);
}

TEST(bullet_inactive_does_not_move) {
    Bullet b;
    b.pos = {10.0f, 10.0f};
    b.vel = {5.0f, 0.0f};
    b.active = false;
    b.update();
    ASSERT_NEAR(b.pos.x, 10.0f, 0.001f);
}

TEST(bullet_expires_after_max_lifetime) {
    Bullet b;
    b.spawn(0, 0, 1, 0);
    b.maxLifetime = 5;
    for (int i = 0; i < 5; i++) b.update();
    ASSERT_FALSE(b.active);
}

TEST(bullet_getRect) {
    Bullet b;
    b.spawn(10.0f, 20.0f, 0, 0);
    Rect r = b.getRect();
    ASSERT_NEAR(r.x, 10.0f, 0.001f);
    ASSERT_NEAR(r.y, 20.0f, 0.001f);
    ASSERT_NEAR(r.w, 6.0f, 0.001f);
    ASSERT_NEAR(r.h, 4.0f, 0.001f);
}

// ============================================================
// Particle Tests
// ============================================================
TEST(particle_default_inactive) {
    Particle p;
    ASSERT_FALSE(p.active);
}

TEST(particle_spawn_activates) {
    Particle p;
    p.spawn(10.0f, 20.0f, 255, 0, 0);
    ASSERT_TRUE(p.active);
    ASSERT_NEAR(p.pos.x, 10.0f, 0.001f);
    ASSERT_NEAR(p.pos.y, 20.0f, 0.001f);
    ASSERT_EQ(p.r, 255);
    ASSERT_EQ(p.g, 0);
    ASSERT_EQ(p.b, 0);
    ASSERT_EQ(p.life, 0);
}

TEST(particle_expires) {
    Particle p;
    p.spawn(0, 0, 255, 255, 255);
    p.maxLife = 3;
    for (int i = 0; i < 3; i++) p.update();
    ASSERT_FALSE(p.active);
}

TEST(particle_inactive_does_not_update) {
    Particle p;
    p.pos = {5, 5};
    p.vel = {1, 1};
    p.active = false;
    float origX = p.pos.x;
    p.update();
    ASSERT_NEAR(p.pos.x, origX, 0.001f);
}

// ============================================================
// Enemy Tests
// ============================================================
TEST(enemy_default_inactive) {
    Enemy e;
    ASSERT_FALSE(e.active);
}

TEST(enemy_spawn_met) {
    Enemy e;
    e.spawn(ENEMY_MET, 100.0f, 200.0f);
    ASSERT_TRUE(e.active);
    ASSERT_EQ(e.type, ENEMY_MET);
    ASSERT_EQ(e.hp, 2);
    ASSERT_EQ(e.maxHp, 2);
    ASSERT_EQ(e.width, 16);
    ASSERT_EQ(e.height, 16);
    ASSERT_NEAR(e.pos.x, 100.0f, 0.001f);
    ASSERT_NEAR(e.startX, 100.0f, 0.001f);
}

TEST(enemy_spawn_sniper) {
    Enemy e;
    e.spawn(ENEMY_SNIPER, 50, 60);
    ASSERT_EQ(e.hp, 5);
    ASSERT_EQ(e.width, 16);
    ASSERT_EQ(e.height, 24);
}

TEST(enemy_spawn_flying) {
    Enemy e;
    e.spawn(ENEMY_FLYING, 50, 60);
    ASSERT_EQ(e.hp, 2);
    ASSERT_EQ(e.width, 16);
    ASSERT_EQ(e.height, 16);
}

TEST(enemy_spawn_boss) {
    Enemy e;
    e.spawn(ENEMY_BOSS, 50, 60);
    ASSERT_EQ(e.hp, 28);
    ASSERT_EQ(e.maxHp, 28);
    ASSERT_EQ(e.width, 32);
    ASSERT_EQ(e.height, 32);
}

TEST(enemy_take_damage) {
    Enemy e;
    e.spawn(ENEMY_SNIPER, 100, 100);
    ASSERT_EQ(e.hp, 5);
    e.takeDamage(2);
    ASSERT_EQ(e.hp, 3);
    ASSERT_TRUE(e.active);
}

TEST(enemy_take_lethal_damage) {
    Enemy e;
    e.spawn(ENEMY_FLYING, 100, 100);
    ASSERT_EQ(e.hp, 2);
    e.takeDamage(5);
    ASSERT_FALSE(e.active);
}

TEST(enemy_met_invulnerable_when_hidden) {
    Enemy e;
    e.spawn(ENEMY_MET, 100, 100);
    ASSERT_EQ(e.state, 0); // Hidden
    int hp_before = e.hp;
    e.takeDamage(1);
    ASSERT_EQ(e.hp, hp_before); // No damage taken
    ASSERT_TRUE(e.active);
}

TEST(enemy_met_vulnerable_when_exposed) {
    Enemy e;
    e.spawn(ENEMY_MET, 100, 100);
    e.state = 1; // Force exposed state
    int hp_before = e.hp;
    e.takeDamage(1);
    ASSERT_EQ(e.hp, hp_before - 1);
}

TEST(enemy_inactive_does_not_update) {
    Enemy e;
    e.active = false;
    bool shot = e.update(0, 0);
    ASSERT_FALSE(shot);
}

TEST(enemy_getRect) {
    Enemy e;
    e.spawn(ENEMY_BOSS, 10, 20);
    Rect r = e.getRect();
    ASSERT_NEAR(r.x, 10.0f, 0.001f);
    ASSERT_NEAR(r.y, 20.0f, 0.001f);
    ASSERT_NEAR(r.w, 32.0f, 0.001f);
    ASSERT_NEAR(r.h, 32.0f, 0.001f);
}

TEST(enemy_faces_player) {
    Enemy e;
    e.spawn(ENEMY_MET, 100, 100);
    // Player to the right
    e.update(200, 100);
    ASSERT_TRUE(e.facingRight);
    // Player to the left
    e.update(0, 100);
    ASSERT_FALSE(e.facingRight);
}

TEST(enemy_shoot_direction) {
    Enemy e;
    e.spawn(ENEMY_SNIPER, 100, 100);
    e.facingRight = true;
    ASSERT_NEAR(e.getShootDirX(), BULLET_SPEED, 0.001f);
    ASSERT_NEAR(e.getShootDirY(), 0.0f, 0.001f);

    e.facingRight = false;
    ASSERT_NEAR(e.getShootDirX(), -BULLET_SPEED, 0.001f);
}

// ============================================================
// Level Tests
// ============================================================
TEST(level_clear) {
    Level lvl;
    lvl.tiles[5][5] = TILE_SOLID;
    lvl.numEnemies = 3;
    lvl.clear();
    ASSERT_EQ(lvl.tiles[5][5], TILE_EMPTY);
    ASSERT_EQ(lvl.numEnemies, 0);
}

TEST(level_getTile_in_bounds) {
    Level lvl;
    lvl.clear();
    lvl.tiles[3][7] = TILE_SOLID;
    ASSERT_EQ(lvl.getTile(7, 3), TILE_SOLID);
    ASSERT_EQ(lvl.getTile(0, 0), TILE_EMPTY);
}

TEST(level_getTile_out_of_bounds_returns_solid) {
    Level lvl;
    lvl.clear();
    ASSERT_EQ(lvl.getTile(-1, 0), TILE_SOLID);
    ASSERT_EQ(lvl.getTile(MAP_COLS, 0), TILE_SOLID);
    ASSERT_EQ(lvl.getTile(0, -1), TILE_SOLID);
    ASSERT_EQ(lvl.getTile(0, MAP_ROWS), TILE_SOLID);
    ASSERT_EQ(lvl.getTile(-100, -100), TILE_SOLID);
}

TEST(level_isSolid) {
    Level lvl;
    lvl.clear();
    lvl.tiles[5][5] = TILE_SOLID;
    ASSERT_TRUE(lvl.isSolid(5, 5));
    ASSERT_FALSE(lvl.isSolid(0, 0));
}

TEST(level_isSpike) {
    Level lvl;
    lvl.clear();
    lvl.tiles[10][5] = TILE_SPIKE;
    ASSERT_TRUE(lvl.isSpike(5, 10));
    ASSERT_FALSE(lvl.isSpike(0, 0));
}

TEST(level_addEnemy) {
    Level lvl;
    lvl.clear();
    ASSERT_EQ(lvl.numEnemies, 0);
    lvl.addEnemy(ENEMY_MET, 100, 200);
    ASSERT_EQ(lvl.numEnemies, 1);
    ASSERT_EQ(lvl.enemies[0].type, ENEMY_MET);
    ASSERT_NEAR(lvl.enemies[0].x, 100.0f, 0.001f);
    ASSERT_NEAR(lvl.enemies[0].y, 200.0f, 0.001f);
}

TEST(level_addEnemy_max_cap) {
    Level lvl;
    lvl.clear();
    for (int i = 0; i < 40; i++) {
        lvl.addEnemy(ENEMY_MET, i * 10.0f, 0);
    }
    ASSERT_EQ(lvl.numEnemies, 32); // Capped at 32
}

TEST(level_generate_stage1_has_ground) {
    Level lvl;
    LevelGen::generateStage1(lvl);
    // Stage 1 should have ground at row 12+ for columns 0-11
    ASSERT_TRUE(lvl.isSolid(0, 12));
    ASSERT_TRUE(lvl.isSolid(5, 13));
    ASSERT_TRUE(lvl.isSolid(11, 14));
}

TEST(level_generate_stage1_has_pits) {
    Level lvl;
    LevelGen::generateStage1(lvl);
    // Pit at col 12-14 should have spikes at bottom
    ASSERT_TRUE(lvl.isSpike(12, MAP_ROWS - 1));
    ASSERT_TRUE(lvl.isSpike(13, MAP_ROWS - 1));
}

TEST(level_generate_stage1_has_enemies) {
    Level lvl;
    LevelGen::generateStage1(lvl);
    ASSERT_TRUE(lvl.numEnemies > 0);
    ASSERT_TRUE(lvl.numEnemies <= 32);
}

TEST(level_generate_all_stages) {
    // Ensure no crashes when generating all stages
    Level lvl;
    for (int i = 0; i < 4; i++) {
        LevelGen::generate(lvl, i);
        ASSERT_TRUE(lvl.mapWidth > 0);
        ASSERT_TRUE(lvl.mapWidth <= MAP_COLS);
    }
}

TEST(level_generate_default_fallback) {
    // Out-of-range stage index falls back to stage 1
    Level lvl;
    LevelGen::generate(lvl, 99);
    Level ref;
    LevelGen::generateStage1(ref);
    ASSERT_EQ(lvl.mapWidth, ref.mapWidth);
    ASSERT_EQ(lvl.numEnemies, ref.numEnemies);
}

TEST(level_boss_room_enclosed) {
    Level lvl;
    LevelGen::generateBossRoom(lvl);
    // Walls on left and right
    for (int r = 0; r < MAP_ROWS; r++) {
        ASSERT_TRUE(lvl.isSolid(0, r));
        ASSERT_TRUE(lvl.isSolid(15, r));
    }
    // Ceiling
    for (int c = 0; c < 16; c++) {
        ASSERT_TRUE(lvl.isSolid(c, 0));
    }
    // Has boss enemy
    bool hasBoss = false;
    for (int i = 0; i < lvl.numEnemies; i++) {
        if (lvl.enemies[i].type == ENEMY_BOSS) hasBoss = true;
    }
    ASSERT_TRUE(hasBoss);
}

TEST(level_generateFloor) {
    Level lvl;
    lvl.clear();
    LevelGen::generateFloor(lvl, 2, 5, 10);
    // Columns 2-4, rows 10 to MAP_ROWS-1 should be solid
    ASSERT_TRUE(lvl.isSolid(2, 10));
    ASSERT_TRUE(lvl.isSolid(4, MAP_ROWS - 1));
    ASSERT_FALSE(lvl.isSolid(2, 9)); // Above floor
    ASSERT_FALSE(lvl.isSolid(5, 10)); // Past end
}

TEST(level_generatePlatform) {
    Level lvl;
    lvl.clear();
    LevelGen::generatePlatform(lvl, 5, 3, 4);
    ASSERT_TRUE(lvl.isSolid(5, 3));
    ASSERT_TRUE(lvl.isSolid(8, 3));
    ASSERT_FALSE(lvl.isSolid(9, 3));
    ASSERT_FALSE(lvl.isSolid(5, 4)); // Only one row
}

TEST(level_generatePit) {
    Level lvl;
    lvl.clear();
    // First fill with ground
    LevelGen::generateFloor(lvl, 0, 10, 10);
    // Now carve a pit
    LevelGen::generatePit(lvl, 3, 2);
    ASSERT_FALSE(lvl.isSolid(3, 10)); // Pit cleared floor
    ASSERT_FALSE(lvl.isSolid(4, 10));
    ASSERT_TRUE(lvl.isSpike(3, MAP_ROWS - 1)); // Spikes at bottom
    ASSERT_TRUE(lvl.isSpike(4, MAP_ROWS - 1));
    ASSERT_TRUE(lvl.isSolid(2, 10)); // Adjacent floor intact
}

// ============================================================
// Constants sanity checks
// ============================================================
TEST(constants_game_dimensions) {
    ASSERT_TRUE(GAME_WIDTH > 0);
    ASSERT_TRUE(GAME_HEIGHT > 0);
    ASSERT_TRUE(TILE_SIZE > 0);
    ASSERT_TRUE(MAP_COLS > 0);
    ASSERT_TRUE(MAP_ROWS > 0);
}

TEST(constants_physics) {
    ASSERT_TRUE(GRAVITY > 0);
    ASSERT_TRUE(MAX_FALL_SPEED > 0);
    ASSERT_TRUE(PLAYER_SPEED > 0);
    ASSERT_TRUE(PLAYER_JUMP_FORCE < 0); // Negative = upward
    ASSERT_TRUE(BULLET_SPEED > 0);
}

TEST(constants_player) {
    ASSERT_TRUE(PLAYER_WIDTH > 0);
    ASSERT_TRUE(PLAYER_HEIGHT > 0);
    ASSERT_TRUE(PLAYER_MAX_HP > 0);
    ASSERT_TRUE(MAX_BULLETS > 0);
}

TEST(constants_frame_timing) {
    ASSERT_TRUE(TARGET_FPS > 0);
    ASSERT_TRUE(FRAME_DELAY > 0);
    ASSERT_EQ(FRAME_DELAY, 1000 / TARGET_FPS);
}

// ============================================================
// Integration-style tests
// ============================================================
TEST(bullet_enemy_collision) {
    Bullet b;
    b.spawn(50, 50, 0, 0);
    Enemy e;
    e.spawn(ENEMY_SNIPER, 48, 48);
    ASSERT_TRUE(b.getRect().overlaps(e.getRect()));
}

TEST(bullet_enemy_no_collision_far_apart) {
    Bullet b;
    b.spawn(0, 0, 0, 0);
    Enemy e;
    e.spawn(ENEMY_MET, 200, 200);
    ASSERT_FALSE(b.getRect().overlaps(e.getRect()));
}

TEST(enemy_damage_to_death_sequence) {
    Enemy e;
    e.spawn(ENEMY_MET, 100, 100);
    e.state = 1; // Expose so damage works
    ASSERT_EQ(e.hp, 2);

    e.takeDamage(1);
    ASSERT_EQ(e.hp, 1);
    ASSERT_TRUE(e.active);

    e.takeDamage(1);
    ASSERT_EQ(e.hp, 0);
    ASSERT_FALSE(e.active);
}

TEST(boss_takes_many_hits) {
    Enemy boss;
    boss.spawn(ENEMY_BOSS, 100, 100);
    for (int i = 0; i < 27; i++) {
        boss.takeDamage(1);
        ASSERT_TRUE(boss.active);
    }
    boss.takeDamage(1); // 28th hit
    ASSERT_FALSE(boss.active);
}

TEST(multiple_bullets_track_lifetime) {
    Bullet bullets[3];
    for (int i = 0; i < 3; i++) {
        bullets[i].spawn(i * 20.0f, 0, BULLET_SPEED, 0);
        bullets[i].maxLifetime = 10;
    }
    for (int frame = 0; frame < 10; frame++) {
        for (auto& b : bullets) b.update();
    }
    for (auto& b : bullets) {
        ASSERT_FALSE(b.active);
    }
}

TEST(level_player_start_position_on_ground) {
    // Verify player starts above solid ground in each stage
    for (int stage = 0; stage < 4; stage++) {
        Level lvl;
        LevelGen::generate(lvl, stage);
        int col = static_cast<int>(lvl.playerStartX) / TILE_SIZE;
        int row = static_cast<int>(lvl.playerStartY + PLAYER_HEIGHT) / TILE_SIZE;
        // There should be solid ground at or near the player start
        bool foundGround = false;
        for (int r = row; r < MAP_ROWS; r++) {
            if (lvl.isSolid(col, r)) {
                foundGround = true;
                break;
            }
        }
        ASSERT_TRUE(foundGround);
    }
}

// ============================================================
// Main
// ============================================================
int main() {
    printf("========================================\n");
    printf("  Megaman99 Test Suite\n");
    printf("========================================\n");

    SUITE("Vec2");
    RUN_TEST(vec2_default_construction);
    RUN_TEST(vec2_parameterized_construction);
    RUN_TEST(vec2_addition);
    RUN_TEST(vec2_subtraction);
    RUN_TEST(vec2_scalar_multiply);
    RUN_TEST(vec2_plus_equals);
    RUN_TEST(vec2_minus_equals);

    SUITE("Rect");
    RUN_TEST(rect_default_construction);
    RUN_TEST(rect_parameterized_construction);
    RUN_TEST(rect_right_bottom);
    RUN_TEST(rect_center);
    RUN_TEST(rect_overlaps_true);
    RUN_TEST(rect_overlaps_false_no_contact);
    RUN_TEST(rect_overlaps_edge_touching_is_false);
    RUN_TEST(rect_overlaps_contained);
    RUN_TEST(rect_zero_width_inside_overlaps);
    RUN_TEST(rect_zero_width_outside_no_overlap);
    RUN_TEST(rect_zero_size_at_origin_no_overlap);

    SUITE("Bullet");
    RUN_TEST(bullet_default_inactive);
    RUN_TEST(bullet_spawn);
    RUN_TEST(bullet_spawn_enemy);
    RUN_TEST(bullet_update_moves);
    RUN_TEST(bullet_inactive_does_not_move);
    RUN_TEST(bullet_expires_after_max_lifetime);
    RUN_TEST(bullet_getRect);

    SUITE("Particle");
    RUN_TEST(particle_default_inactive);
    RUN_TEST(particle_spawn_activates);
    RUN_TEST(particle_expires);
    RUN_TEST(particle_inactive_does_not_update);

    SUITE("Enemy");
    RUN_TEST(enemy_default_inactive);
    RUN_TEST(enemy_spawn_met);
    RUN_TEST(enemy_spawn_sniper);
    RUN_TEST(enemy_spawn_flying);
    RUN_TEST(enemy_spawn_boss);
    RUN_TEST(enemy_take_damage);
    RUN_TEST(enemy_take_lethal_damage);
    RUN_TEST(enemy_met_invulnerable_when_hidden);
    RUN_TEST(enemy_met_vulnerable_when_exposed);
    RUN_TEST(enemy_inactive_does_not_update);
    RUN_TEST(enemy_getRect);
    RUN_TEST(enemy_faces_player);
    RUN_TEST(enemy_shoot_direction);

    SUITE("Level");
    RUN_TEST(level_clear);
    RUN_TEST(level_getTile_in_bounds);
    RUN_TEST(level_getTile_out_of_bounds_returns_solid);
    RUN_TEST(level_isSolid);
    RUN_TEST(level_isSpike);
    RUN_TEST(level_addEnemy);
    RUN_TEST(level_addEnemy_max_cap);
    RUN_TEST(level_generate_stage1_has_ground);
    RUN_TEST(level_generate_stage1_has_pits);
    RUN_TEST(level_generate_stage1_has_enemies);
    RUN_TEST(level_generate_all_stages);
    RUN_TEST(level_generate_default_fallback);
    RUN_TEST(level_boss_room_enclosed);
    RUN_TEST(level_generateFloor);
    RUN_TEST(level_generatePlatform);
    RUN_TEST(level_generatePit);

    SUITE("Constants");
    RUN_TEST(constants_game_dimensions);
    RUN_TEST(constants_physics);
    RUN_TEST(constants_player);
    RUN_TEST(constants_frame_timing);

    SUITE("Integration");
    RUN_TEST(bullet_enemy_collision);
    RUN_TEST(bullet_enemy_no_collision_far_apart);
    RUN_TEST(enemy_damage_to_death_sequence);
    RUN_TEST(boss_takes_many_hits);
    RUN_TEST(multiple_bullets_track_lifetime);
    RUN_TEST(level_player_start_position_on_ground);

    printf("\n========================================\n");
    printf("  Results: %d/%d passed", tests_passed, tests_run);
    if (tests_failed > 0) printf(", %d FAILED", tests_failed);
    printf("\n========================================\n");

    return tests_failed > 0 ? 1 : 0;
}

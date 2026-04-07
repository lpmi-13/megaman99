#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cstdio>
#include <string>
#include <unordered_map>

class Audio {
public:
    bool init() {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            fprintf(stderr, "SDL_mixer init failed: %s\n", Mix_GetError());
            // Audio is optional, don't fail
            available_ = false;
            return true;
        }
        Mix_AllocateChannels(8);
        available_ = true;
        return true;
    }

    // Generate simple square wave sound effects procedurally
    void generateSounds() {
        if (!available_) return;
        sfx_shoot = generateSquareWave(880, 80, 64);    // Short high beep
        sfx_hit = generateSquareWave(220, 150, 80);     // Low thud
        sfx_jump = generateSweep(400, 800, 100, 48);    // Rising sweep
        sfx_land = generateSquareWave(150, 50, 40);     // Short thump
        sfx_death = generateSweep(600, 100, 500, 80);   // Falling sweep
        sfx_menu = generateSquareWave(660, 60, 48);     // Menu blip
        sfx_boss_hit = generateSquareWave(300, 200, 96); // Heavy hit
        sfx_stage_clear = generateSweep(400, 1200, 400, 64); // Victory sweep
    }

    void playShoot() { playChunk(sfx_shoot); }
    void playHit() { playChunk(sfx_hit); }
    void playJump() { playChunk(sfx_jump); }
    void playLand() { playChunk(sfx_land); }
    void playDeath() { playChunk(sfx_death); }
    void playMenu() { playChunk(sfx_menu); }
    void playBossHit() { playChunk(sfx_boss_hit); }
    void playStageClear() { playChunk(sfx_stage_clear); }

    void shutdown() {
        freeChunk(sfx_shoot);
        freeChunk(sfx_hit);
        freeChunk(sfx_jump);
        freeChunk(sfx_land);
        freeChunk(sfx_death);
        freeChunk(sfx_menu);
        freeChunk(sfx_boss_hit);
        freeChunk(sfx_stage_clear);
        if (available_) Mix_CloseAudio();
    }

private:
    bool available_ = false;
    Mix_Chunk* sfx_shoot = nullptr;
    Mix_Chunk* sfx_hit = nullptr;
    Mix_Chunk* sfx_jump = nullptr;
    Mix_Chunk* sfx_land = nullptr;
    Mix_Chunk* sfx_death = nullptr;
    Mix_Chunk* sfx_menu = nullptr;
    Mix_Chunk* sfx_boss_hit = nullptr;
    Mix_Chunk* sfx_stage_clear = nullptr;

    void playChunk(Mix_Chunk* chunk) {
        if (chunk && available_) Mix_PlayChannel(-1, chunk, 0);
    }

    void freeChunk(Mix_Chunk*& chunk) {
        if (chunk) {
            Mix_FreeChunk(chunk);
            chunk = nullptr;
        }
    }

    // Generate a square wave tone
    Mix_Chunk* generateSquareWave(int freq, int duration_ms, int volume) {
        int sample_rate = 44100;
        int samples = (sample_rate * duration_ms) / 1000;
        int buf_size = samples * 2; // 16-bit mono
        Uint8* buffer = new Uint8[buf_size];
        Sint16* buf16 = reinterpret_cast<Sint16*>(buffer);

        int period = sample_rate / freq;
        for (int i = 0; i < samples; i++) {
            // Square wave with envelope
            float envelope = 1.0f - static_cast<float>(i) / samples;
            Sint16 val = ((i % period) < period / 2) ? volume * 128 : -(volume * 128);
            buf16[i] = static_cast<Sint16>(val * envelope);
        }

        Mix_Chunk* chunk = new Mix_Chunk();
        chunk->allocated = 1;
        chunk->abuf = buffer;
        chunk->alen = buf_size;
        chunk->volume = MIX_MAX_VOLUME / 2;
        return chunk;
    }

    // Generate a frequency sweep
    Mix_Chunk* generateSweep(int freq_start, int freq_end, int duration_ms, int volume) {
        int sample_rate = 44100;
        int samples = (sample_rate * duration_ms) / 1000;
        int buf_size = samples * 2;
        Uint8* buffer = new Uint8[buf_size];
        Sint16* buf16 = reinterpret_cast<Sint16*>(buffer);

        for (int i = 0; i < samples; i++) {
            float t = static_cast<float>(i) / samples;
            float freq = freq_start + (freq_end - freq_start) * t;
            float envelope = 1.0f - t;
            int period = static_cast<int>(sample_rate / freq);
            if (period < 1) period = 1;
            Sint16 val = ((i % period) < period / 2) ? volume * 128 : -(volume * 128);
            buf16[i] = static_cast<Sint16>(val * envelope);
        }

        Mix_Chunk* chunk = new Mix_Chunk();
        chunk->allocated = 1;
        chunk->abuf = buffer;
        chunk->alen = buf_size;
        chunk->volume = MIX_MAX_VOLUME / 2;
        return chunk;
    }
};

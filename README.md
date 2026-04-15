# Megaman99

A Megaman-style action platformer built in C++ with SDL2, targeting the **Miyoo Mini** handheld. Also builds and runs natively on Linux, macOS, and WSL for development.

Originally a repo called `tommy-game` ("a miniyoo game for tommy") — now a complete Megaman clone with stages, enemies, a boss fight, and a full menu system.

## Gameplay

You play as a blue-armored hero with a plasma buster. Fight through three stages, then take on Dr. Wily in the boss room.

### Controls

**Native build (keyboard):**

| Action | Key |
|--------|-----|
| Move left / right | Arrow keys |
| Jump | Space or Left Ctrl |
| Shoot | Z or Left Alt |
| Start / Confirm | Enter |
| Pause / Menu | Escape |

**Miyoo Mini:** standard D-pad plus A (jump), B (shoot), Start (pause/confirm), Menu (exit to title).

### Core mechanics

- **Variable-height jump** — tap for a short hop, hold for full height
- **3-bullet limit** — classic Megaman rule; only 3 of your shots can be on screen at once
- **Invincibility frames** — 90 frames of flashing invulnerability after taking damage
- **3 lives per run** — lose them all and it's game over; pressing Start returns to the title
- **Health bar** — 28 HP total, displayed on the left side of the HUD
- **Instant-death hazards** — spikes and pits kill immediately regardless of HP

## Levels

There are **4 stages**, accessed from the stage select screen:

| Stage | Theme | Color | Notes |
|-------|-------|-------|-------|
| **Elec Man** | Electric facility | Blue | Starter stage — teaches platforming and shooting |
| **Fire Man** | Burning fortress | Red | Staircase platforms, denser enemies |
| **Wood Man** | Forest stronghold | Green | Higher platforms, more flying enemies |
| **Dr. Wily** | Wily's lair | Purple | Boss room — **locked until all 3 action stages are cleared** |

Each of the 3 action stages is a horizontally-scrolling level ~60 tiles wide with ground segments, pits (with spikes at the bottom), raised platforms, and walls. Reach the right edge of the stage to clear it.

The boss room is a single-screen enclosed arena with walls, ceiling, and two platforms. The boss jumps, lands, and fires a 3-shot burst in a repeating cycle. Takes 28 hits to defeat.

### Enemies

| Enemy | HP | Behavior |
|-------|-----|----------|
| **Met** | 2 | Hides under a hard hat (invulnerable) until you approach, then pops up and fires |
| **Sniper Joe** | 5 | Patrols back and forth, periodically fires a bullet when you're in range |
| **Flying** | 2 | Propeller enemy that chases you in a sine-wave pattern |
| **Boss (Dr. Wily)** | 28 | Jump → land → 3-shot burst cycle; contact damage is 4 |

### Progression

- Clear a stage by reaching the far right edge (or defeating the boss in stage 4)
- Cleared stages show a yellow checkmark on the stage select screen
- The boss room (stage 4) is locked until all 3 action stages are cleared
- Beating the boss shows a congratulations screen and returns to the title

## Local development

### Prerequisites

You need a C++17 compiler and SDL2 development libraries.

**macOS** (Homebrew):
```bash
brew install sdl2 sdl2_mixer sdl2_image sdl2_ttf
```

**Ubuntu / Debian / WSL**:
```bash
sudo apt-get update
sudo apt-get install build-essential libsdl2-dev libsdl2-mixer-dev libsdl2-image-dev libsdl2-ttf-dev
```

**Fedora / RHEL**:
```bash
sudo dnf install gcc-c++ make SDL2-devel SDL2_mixer-devel SDL2_image-devel SDL2_ttf-devel
```

**Arch**:
```bash
sudo pacman -S base-devel sdl2 sdl2_mixer sdl2_image sdl2_ttf
```

**Windows**: Use WSL2 with Ubuntu and follow the Ubuntu steps — SDL2 on native Windows is doable but painful.

### Build and run

From the repo root:

```bash
make         # builds ./megaman99
./megaman99  # or: make run
```

Debug build (symbols, no optimization):

```bash
make clean && DEBUG=1 make
```

Iterate with:

```bash
make clean && make && ./megaman99
```

A full rebuild is fast — the whole game is a single compilation unit (`src/main.cpp` pulls in every header).

### Running tests

```bash
make test
```

This builds `tests/test_game.cpp` and runs all 68 unit tests. The test binary has **no SDL dependency** — it only exercises the pure game logic headers (`Vec2`, `Rect`, `Bullet`, `Enemy`, `Level`, etc.). Tests should take under a second and pass cleanly before any commit.

### Project layout

```
megaman99/
├── src/
│   ├── main.cpp           # Game loop, state machine, collision
│   ├── player.h           # Player movement, jumping, shooting
│   ├── enemy.h            # 4 enemy types with AI
│   ├── level.h            # Tile map + procedural level generation
│   ├── draw.h             # Software sprite renderer + block font
│   ├── renderer.h         # SDL2 framebuffer setup
│   ├── audio.h            # Procedural square-wave sound effects
│   ├── input.h            # Keyboard + Miyoo Mini button mapping
│   ├── bullet.h           # Bullet entity
│   ├── particle.h         # Particle effects
│   ├── vec2.h / rect.h    # Math primitives
│   └── constants.h        # Physics, dimensions, colors, button map
├── tests/
│   └── test_game.cpp      # 68 unit tests (no SDL dependency)
├── .github/workflows/
│   ├── ci.yml             # Builds and tests on every PR
│   └── release.yml        # Builds a release on version tag push
├── Makefile               # Native + Miyoo Mini cross-compilation
└── README.md
```

### Troubleshooting

- **`sdl2-config: command not found`** — SDL2 dev headers aren't installed. See the prerequisites above.
- **Black window on startup** — the game falls back to a software renderer if hardware acceleration fails. On Linux, try `SDL_VIDEODRIVER=x11 ./megaman99`.
- **No audio** — audio is optional; `Mix_OpenAudio` failures are logged but the game keeps running.
- **Tests fail after a code change** — run `./test_game` directly to see the full per-test output (the Makefile only shows the summary on failure).

## Cross-compiling for the Miyoo Mini

The Miyoo Mini runs Linux on an ARM Cortex-A7 with a 640x480 display. You need the Miyoo Mini toolchain (`arm-linux-gnueabihf-g++`) and a sysroot with SDL2:

```bash
PLATFORM=miyoo MIYOO_SYSROOT=/path/to/miyoo-sysroot make
make install
```

The `install` target stages a packaged build under `release/megaman99/`. Copy that directory to `/mnt/SDCARD/App/megaman99/` on the device's SD card.

Platform-specific tweaks controlled by `-DPLATFORM_MIYOO`:
- Fullscreen window
- Button mapping uses the Miyoo's SDL key assignments (Space=A, LCtrl=B, etc.)
- ARM-specific compiler flags (`-mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard`)

## Releases (GitHub Actions)

Two workflows live in `.github/workflows/`, both pinned to commit SHAs for supply-chain safety.

### `ci.yml` — runs on every PR and push to main

1. Checks out the code (`actions/checkout@v6.0.2`)
2. Installs SDL2 dev libraries via apt
3. Builds the game with `make`
4. Builds and runs the test suite with `make test`

If tests fail, the PR is marked failing. All PRs must pass CI before merging.

### `release.yml` — runs when a `v*` tag is pushed to main

Trigger: `push` events on tags matching `v*` (e.g. `v1`, `v1.0.0`, `v2.3.1`).

Steps:
1. Checks out the tagged commit
2. Installs SDL2 dev libraries
3. Builds the game and runs the full test suite (release won't publish if tests fail)
4. Packages the binary, any assets, `README.md`, and `LICENSE` into `release/megaman99-linux-x86_64.tar.gz`
5. Creates a GitHub release using `softprops/action-gh-release@v2.6.1` with auto-generated release notes, attaching the tarball

The workflow has `contents: write` permission scoped to just this job so it can publish the release.

### Cutting a release

To publish a new version, tag a commit on `main` and push it:

```bash
git checkout main
git pull
git tag v1.0.0
git push origin v1.0.0
```

That's it — the release workflow picks it up, builds, tests, and publishes the release with the tarball attached. The release will appear under **Releases** in the GitHub repo within a few minutes.

Tag naming: use `v<major>.<minor>.<patch>` (e.g. `v1.0.0`). Simple numeric tags like `v1` also work since the trigger pattern is `v*`.

## License

See [LICENSE](LICENSE).

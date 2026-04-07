# Megaman99 - Makefile
# Supports native build and Miyoo Mini cross-compilation

GAME_NAME := megaman99

# Source files
SRC_DIR := src
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:.cpp=.o)

# Common flags
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
LDFLAGS :=

# Detect build target
ifeq ($(PLATFORM),miyoo)
    # Miyoo Mini cross-compilation
    CROSS_COMPILE ?= arm-linux-gnueabihf-
    CXX := $(CROSS_COMPILE)g++
    CXXFLAGS += -mcpu=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard
    CXXFLAGS += -DPLATFORM_MIYOO
    SYSROOT ?= $(MIYOO_SYSROOT)
    ifneq ($(SYSROOT),)
        CXXFLAGS += --sysroot=$(SYSROOT)
        LDFLAGS += --sysroot=$(SYSROOT)
    endif
    CXXFLAGS += $(shell $(SYSROOT)/usr/bin/sdl2-config --cflags 2>/dev/null || echo "-I$(SYSROOT)/usr/include/SDL2 -D_REENTRANT")
    LDFLAGS += $(shell $(SYSROOT)/usr/bin/sdl2-config --libs 2>/dev/null || echo "-lSDL2 -lSDL2_mixer -lSDL2_image -lSDL2_ttf")
    LDFLAGS += -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lpthread
else
    # Native build (Linux/Mac)
    CXX := g++
    CXXFLAGS += -DPLATFORM_NATIVE
    CXXFLAGS += $(shell sdl2-config --cflags)
    LDFLAGS += $(shell sdl2-config --libs) -lSDL2_mixer -lSDL2_image -lSDL2_ttf
endif

# Debug build
ifeq ($(DEBUG),1)
    CXXFLAGS += -g -DDEBUG -O0
endif

.PHONY: all clean run

all: $(GAME_NAME)

$(GAME_NAME): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(SRC_DIR)/*.o $(GAME_NAME)

run: $(GAME_NAME)
	./$(GAME_NAME)

# Install to Miyoo Mini SD card
install: $(GAME_NAME)
	@echo "Copy $(GAME_NAME) to your Miyoo Mini SD card under /mnt/SDCARD/App/megaman99/"
	@mkdir -p release/megaman99
	cp $(GAME_NAME) release/megaman99/
	cp -r assets release/megaman99/ 2>/dev/null || true
	@echo "Done! Copy release/megaman99/ to your SD card."

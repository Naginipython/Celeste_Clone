#pragma once
#include <SDL2/SDL.h>

#include <iostream>
#include <cassert>
// For IO
#include <sys/stat.h>

constexpr int FPS = 60;
constexpr double UPDATE_DELAY = 1.0 / FPS;
constexpr int WORLD_WIDTH = 320;
constexpr int WORLD_HEIGHT = 180;
constexpr int TILESIZE = 8;

#define BIT(x) 1 << (x)
#define println(x) std::cout << x << std::endl
#define TRACE(x) println("\x1b[32mTRACE: " << x << "\x1b[0m");
#define WARN(x) println("\x1b[33mWARNING: " << x << "\x1b[0m");
#define ERROR(x) println("\x1b[31mERROR: " << x << "\x1b[0m");
#define ASSERT(x, msg) if (!(x)) { ERROR(msg); assert(false); }

// IO stuff
long get_timestamp(const std::string& path);
bool file_exists(const std::string& path);
long get_file_size(const std::string& path);
std::string read_file(const std::string& path);
void write_file(const std::string& path, const std::string& content);
bool copy_file(const std::string& src, const std::string& dest);
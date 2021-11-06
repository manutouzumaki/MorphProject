#ifndef PLATFORM_H
#define PLATFORM_H

// c libraries
#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define Assert(condition) assert(condition) 
#define ArrayCount(Array) (sizeof(Array)/sizeof((Array)[0]))

#define Kilobytes(Value) ((Value)*1024LL)
#define Megabytes(Value) (Kilobytes(Value)*1024LL)
#define Gigabytes(Value) (Megabytes(Value)*1024LL)
#define Terabytes(Value) (Gigabytes(Value)*1024LL)

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

typedef float  r32;
typedef double r64;

#include "math.h"
#include "arena.h"

// opaque structs define in specific platforms
struct window;
struct renderer;

#define PLATFORM_CREATE_WINDOW(name) window *name(i32 PosX, i32 PosY, i32 Width, i32 Height, char *Name, arena *Arena)
#define PLATFORM_CREATE_RENDERER(name) renderer *name(window *Window, arena *Arena)
#define PLATFORM_DELETE_RENDERER(name) void name(renderer *Renderer)
#define PLATFORM_READ_ENTIRE_FILE(name) void *name(char *FileName, size_t *FileSizePtr, arena *Arena)
#define PLATFORM_CLEAR_BUFFER(name) void name(r32 R, r32 G, r32 B, r32 A, renderer *Renderer)
#define PLATFORM_PRESENT(name) void name(renderer *Renderer)
// platform functions prototypes
PLATFORM_CREATE_WINDOW(PlatformCreateWindow);
PLATFORM_CREATE_RENDERER(PlatformCreateRenderer);
PLATFORM_DELETE_RENDERER(PlatformDeleteRenderer);
PLATFORM_READ_ENTIRE_FILE(PlatformReadEntireFile);
PLATFORM_CLEAR_BUFFER(PlatformClearBuffer);
PLATFORM_PRESENT(PlatformPresent);

#include "main.cpp"

#include "math.cpp"
#include "arena.cpp"

#endif

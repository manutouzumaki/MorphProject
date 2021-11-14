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

#define WND_WIDTH 800
#define WND_HEIGHT 600

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
#include "utility.h"

struct window;
struct renderer;
struct shader;
struct const_buffer;
struct mesh;
struct texture;

// GAME and GENERAL Use
#define PLAFORM_CREATE_WINDOW(name) window *name(u32 X, u32 Y, u32 Width, u32 Height, char *Name, arena *Arena)
#define PLAFORM_CREATE_RENDERER(name) renderer *name(window *Window, arena *Arena)
#define COMPILE_SHADERS_FROM_FILE(name) shader *name(renderer *Renderer, char *VSFileName, char *PSFileName, arena *Arena)
#define CREATE_MESH(name) mesh *name(renderer *Renderer, r32 *Vertices, u32 VerticesCount, arena *Arena)
#define CREATE_TEXTURE(name) texture *name(renderer *Renderer, char *FileName, arena *Arena)
#define CREATE_CONST_BUFFER(name) const_buffer *name(renderer *Renderer, u32 DataSize, arena *Arena)
#define MAP_CONST_BUFFER(name) void name(renderer *Renderer, const_buffer *ConstBuffer, void *BufferData, u32 DataSize, u32 Register)
#define RENDER_MESH(name) void name(renderer *Renderer, mesh *Mesh, shader *Shader, texture *Texture)
#define RENDER_MESH_NO_TEX(name) void name(renderer *Renderer, mesh *Mesh, shader *Shader)
#define RENDER_FRAME(name) void name(renderer *Renderer, mesh *Mesh, shader *Shader, texture *Texture, const_buffer *ConstBuffer, u32 TileWidth, u32 TileHeight, u32 FrameX, u32 FrameY)
#define GET_TEXTURE_INFO(name) texture_info name(texture *Texture)

PLAFORM_CREATE_WINDOW(PlatformCreateWindow);
PLAFORM_CREATE_RENDERER(PlatformCreateRenderer);
COMPILE_SHADERS_FROM_FILE(CompileShadersFromFile);
CREATE_MESH(CreateMesh);
CREATE_TEXTURE(CreateTexture);
CREATE_CONST_BUFFER(CreateConstBuffer);
MAP_CONST_BUFFER(MapConstBuffer);
RENDER_MESH(RenderMesh);
RENDER_MESH_NO_TEX(RenderMesh);
RENDER_FRAME(RenderFrame);
GET_TEXTURE_INFO(GetTextureInfo);

#endif

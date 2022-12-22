#pragma once
#include "shader.h"
#include <vector>

namespace gxb
{
struct Level;
struct PathPt;
}; // namespace gxb

namespace render
{
void DrawLevel(const unsigned int vaoEntities, const Shader &progOne, const unsigned int vaoOctree,
			   const gxb::Level *const);
void DrawLoadingScreen(const unsigned int vao, const Shader &prog, const float pos, const float rot, const float scale);
void clearScreen();
unsigned int BuildLevelVao(const gxb::Level *);
void logOpenGLInfo();
void SetGlFlags();
unsigned int BuildSpatialGridVao(const std::vector<float> &);
unsigned int BuildLoadingScreenVao();

inline std::vector<unsigned> highlighted_entities{};
}; // namespace render

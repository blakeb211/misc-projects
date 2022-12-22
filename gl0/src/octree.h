#pragma once
#include "flags.h"
#include "gamelib.h"
#include <climits>
#include <vector>

using v3 = glm::vec3;
using iv3 = glm::ivec3;
/* **************************************************************
 *  This file builds a uniform grid for collision testing
 *  ***************************************************************/

namespace SpatialGrid
{

// forward declarations
void Subdivide();
int CalcSideLength();
iv3 PosToGridCoords(const v3 &pos);
void TestingStuffForOctree();
void SetupOctree(const gxb::Level *const);
void UpdateGrid(gxb::Entity *const);
void ClearGrid();
const std::vector<unsigned> &FindNearestNeighbors(const gxb::Entity *const);
const std::vector<float> &GetVertBufGridLinesRef();
size_t GetVertBufGridLinesSize();
iv3 GridIndexToId(const size_t idx);
size_t GridCoordsToIndex(const iv3 coords);
bool FineGrainCollisionCheck(const gxb::Entity *const a, const gxb::Entity *const b, v3 resultant_accel_modifier);

// Globals
inline gxb::Level *level;					  
inline std::vector<float> vertbufGridLines{}; // cell edges of spatial grid 
inline constexpr float targetSideL = 1.74f;
inline constexpr int MAX_CELL_OCCUPATION_PER_ENTITY = 16;
inline constexpr int MAX_ENTITIES = 500;
inline int numCells{};
inline float cellL{}, worldL{};
// keep track of which cells each entity is in
inline std::vector<std::vector<unsigned>> entity_id_to_list_of_cell_ids{MAX_ENTITIES};
// these are scratch pad vectors used in the UpdateGrid()
inline std::vector<iv3> grid_cells_entity_intersects;
inline std::vector<unsigned> grid_ids_entity_intersects;
inline std::vector<unsigned> nearest_neighbor_entity_ids;
// grid coordinates e.g. 0,1,2 that correspond to each cell in grid
inline std::vector<iv3> id;

constexpr size_t default_cell_list_length = 20;

struct BoundingBox
{
	v3 min, max;
};

void AddLinesToVertBuf(const BoundingBox x);
// Individual cell of the uniform grid
struct Cell
{
	Cell()
	{
		list.resize(0);
	}
	Cell(BoundingBox bb) : bb{bb}
	{
		list.resize(0);
	}
	// extent of this cell
	BoundingBox bb;
	// list of object ids that are partly or wholly in this cell
	std::vector<int> list{default_cell_list_length};
};

inline Cell topNode; // whole world
// uniform grid cells; size is numCells^3
inline std::vector<Cell> grid;

} // namespace SpatialGrid

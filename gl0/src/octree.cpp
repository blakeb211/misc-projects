#include "octree.h"

const std::vector<float>& SpatialGrid::GetVertBufGridLinesRef() {
	return vertbufGridLines;
}

size_t SpatialGrid::GetVertBufGridLinesSize()
{
	return vertbufGridLines.size();
}

iv3 SpatialGrid::GridIndexToId(const size_t idx)
{
	if constexpr (Flags::USE_ASSERTIONS)
		assert(idx < id.size() && idx >= 0);

	return id[idx];
}

void SpatialGrid::AddLinesToVertBuf(const SpatialGrid::BoundingBox x)
{
	const auto min = x.min;
	const auto max = x.max;

	auto push_floats = [&](v3 corner) {
		vertbufGridLines.push_back(corner.x);
		vertbufGridLines.push_back(corner.y);
		vertbufGridLines.push_back(corner.z);
	};

	auto push_cube = [&](v3 min, v3 max) {
		// GL_LINES draws verts 0 and 1 as first line, then 2 and 3 as next line
		push_floats(min);
		push_floats(min + v3(cellL, 0.f, 0.f));
		push_floats(min);
		push_floats(min + v3(0.f, cellL, 0.f));
		push_floats(min);
		push_floats(min + v3(0.f, 0.f, cellL));

		push_floats(max);
		push_floats(max - v3(cellL, 0.f, 0.f));
		push_floats(max);
		push_floats(max - v3(0.f, cellL, 0.f));
		push_floats(max);
		push_floats(max - v3(0.f, 0.f, cellL));

		push_floats(max - v3(0.f, cellL, 0.f));
		push_floats(max - v3(cellL, cellL, 0.f));
		push_floats(max - v3(0.f, cellL, 0.f));
		push_floats(max - v3(0.f, cellL, cellL));

		push_floats(max - v3(cellL, 0.f, 0.f));
		push_floats(max - v3(cellL, cellL, 0.f));
		push_floats(max - v3(cellL, 0.f, 0.f));
		push_floats(max - v3(cellL, 0.f, cellL));

		push_floats(max - v3(0.f, 0.f, cellL));
		push_floats(max - v3(0.f, cellL, cellL));
		push_floats(max - v3(0.f, 0.f, cellL));
		push_floats(max - v3(cellL, 0.f, cellL));
	};
	// real action happens here
	// adds 12 sets of line vertices to vertices vector so they can be drawn
	push_cube(min, max);
}

// get the grid id from position
// returns (0,0,0) to (numCells-1,numCells-1,numCells-1)
iv3 SpatialGrid::PosToGridCoords(const v3 &pos)
{
	const auto &min = topNode.bb.min;
	const auto diff = (pos - min) / cellL;
	// truncate to integer values
	return iv3{trunc(diff.x), trunc(diff.y), trunc(diff.z)};
}

size_t SpatialGrid::GridCoordsToIndex(const iv3 coords)
{
	if (coords.x < 0 || coords.y < 0 || coords.z < 0 || coords.x > numCells - 1 || coords.y > numCells - 1 ||
		coords.z > numCells - 1)
		return UINT_MAX;
	return coords.x + numCells * (coords.z + numCells * coords.y);
}

void SpatialGrid::ClearGrid()
{
	const auto sz = grid.size();
	for (int i = 0; i < sz; i++)
		grid[i].list.resize(0);
}

void SpatialGrid::UpdateGrid(gxb::Entity *const o)
{
	if constexpr (Flags::USE_ASSERTIONS)
		assert(o != nullptr);

	const auto &o_mesh = level->GetMesh(o->mesh_hash);
	const auto &o_radius = o_mesh->spherical_diameter / 2.0f;

	grid_cells_entity_intersects.resize(0);
	grid_ids_entity_intersects.resize(0);

	grid_cells_entity_intersects.push_back(PosToGridCoords(o->pos)); // center of obj

	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(0.f, 0.f, 1.f))); // +z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(0.f, 0.f, -1.f))); // -z boundary of obj

	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(1.f, 0.f, 0.f))); // +x boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(-1.f, 0.f, 0.f))); // -x boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(0.f, 1.f, 0.f))); // +y boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(0.f, -1.f, 0.f))); // -y boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(1.f, 1.f, 0.f))); // +x+y boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(1.f, -1.f, 0.f))); // +x-y boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(-1.f, 1.f, 0.f))); // -x+y boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(-1.f, -1.f, 0.f))); // -x-y boundary of obj

	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(1.f, 0.f, 1.f))); // +x+z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(-1.f, 0.f, 1.f))); // -x+z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(0.f, 1.f, 1.f))); // +y+z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(0.f, -1.f, 1.f))); // -y+z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(1.f, 1.f, 1.f))); // +x+y+z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(1.f, -1.f, 1.f))); // +x-y+z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(-1.f, 1.f, 1.f))); // -x+y+z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(-1.f, -1.f, 1.f))); // -x-y+z boundary of obj

	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(1.f, 0.f, -1.f))); // +x-z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(-1.f, 0.f, -1.f))); // -x-z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(0.f, 1.f, -1.f))); // +y-z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(0.f, -1.f, -1.f))); // -y-z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(1.f, 1.f, -1.f))); // +x+y-z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(1.f, -1.f, -1.f))); // +x-y-z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(-1.f, 1.f, -1.f))); // -x+y-z boundary of obj
	grid_cells_entity_intersects.push_back(
		PosToGridCoords(o->pos + o_radius * v3(-1.f, -1.f, -1.f))); // -x-y-z boundary of obj

	auto const &entity_id = o->id;

	decltype(grid_cells_entity_intersects)::iterator end_it;

	//# include<algorithm>// sort
	//#include<tuple>// tie
	//    std::vector<glm::vec3>a = { ... };
	//    std::sort(a.begin(), a.end(),
	//      [](const glm::vec3& lhs, const glm::vec3& rhs) {
	//        // Sort in ascending order by key x, y priority
	//        return std::tie(lhs.x, lhs.z) < std::tie(rhs.x, rhs.z);
	//      });
	const auto less_than_iv3 = [](const iv3 &a, const iv3 &b) { return a.x < b.x && a.y < b.y && a.z < b.z; };
	std::sort(grid_cells_entity_intersects.begin(), grid_cells_entity_intersects.end(), less_than_iv3);
	end_it = std::unique(grid_cells_entity_intersects.begin(), grid_cells_entity_intersects.end());

	for (auto curr_coord_it = grid_cells_entity_intersects.begin(); curr_coord_it != end_it; curr_coord_it++)
	{
		size_t curr_idx = GridCoordsToIndex(*curr_coord_it);
		if (curr_idx != UINT_MAX)
		{
			grid[curr_idx].list.push_back(entity_id);
			grid_ids_entity_intersects.push_back(static_cast<unsigned>(curr_idx));
		}
	}

	// Build entity_id_to_list_of_cell_ids for easy nearest neighbor searching
	// convert grid_cells_entity_intersects to grid_ids_entity_intersects
	// copy grid_ids_entity_intersects to entity_id_to_list_of_cell_ids
	entity_id_to_list_of_cell_ids[entity_id].resize(grid_ids_entity_intersects.size());
	std::copy(grid_ids_entity_intersects.begin(), grid_ids_entity_intersects.end(),
			  entity_id_to_list_of_cell_ids[entity_id].begin());
}

// find min and max of x,y,z objects in level and build a uniform
// grid enclosing it
void SpatialGrid::SetupOctree(const gxb::Level *const level)
{
	if constexpr (Flags::USE_ASSERTIONS)
		assert(level != NULL);

	grid_cells_entity_intersects.resize(30);

	const auto sz = entity_id_to_list_of_cell_ids.size();
	for (int i = 0; i < sz; i++)
	{
		entity_id_to_list_of_cell_ids[i] = std::vector<unsigned>(MAX_CELL_OCCUPATION_PER_ENTITY);
		entity_id_to_list_of_cell_ids[i].resize(0);
	}

	// match object id to cells that it occupies
	SpatialGrid::level = const_cast<gxb::Level *>(level);
	glm::vec3 min{0}, max{0};

	for (auto &i : level->objects)
	{
		if (i->pos.x < min.x)
			min.x = i->pos.x;
		if (i->pos.y < min.y)
			min.y = i->pos.y;
		if (i->pos.z < min.z)
			min.z = i->pos.z;

		if (i->pos.x > max.x)
			max.x = i->pos.x;
		if (i->pos.y > max.y)
			max.y = i->pos.y;
		if (i->pos.z > max.z)
			max.z = i->pos.z;
	}

	// add a buffer around world
	auto orig_dim = max - min;
	// want a cubic SpatialGrid, so take max dimension and use that for x,y, and z
	// expand max and min both outward 10%; this is very arbitrary
	min -= 0.1f * (orig_dim);
	max += 0.1f * (orig_dim);
	auto expanded_dim = max - min;
	Log::PrintLn("level min, max:", glm::to_string(min), glm::to_string(max));
	// take max dimension as our cube's side length
	float cubic_dim = std::max({expanded_dim.x, expanded_dim.y, expanded_dim.z});
	Log::PrintLn("SpatialGrid extent:", cubic_dim);
	worldL = cubic_dim;

	max = min + v3(cubic_dim, cubic_dim, cubic_dim);

	// create SpatialGrid
	topNode = Cell{BoundingBox{min, max}};
	// Subdivide topNode until
	// 		a bin contains fewer than a given number of points
	// 		a bin reaches a minimum size based on the length of its edges
	// AddLinesToVertBuf(topNode.bb);
	numCells = CalcSideLength();
	Log::PrintLn("ideal cellL found: ", cellL, "with numcells:", numCells);

	Subdivide();
//	TestingStuffForOctree();
}

// fxn uses and modifies namespace globals to calculate
// the side length and number of cells per dimension of the uniform grid
int SpatialGrid::CalcSideLength()
{
	if constexpr (Flags::USE_ASSERTIONS)
		assert(cellL < worldL);

	auto numCells = std::floor(worldL / targetSideL);
	while (worldL / cellL > numCells)
	{
		cellL += 0.00001f;
	}
	return (int)(std::roundf(worldL / cellL));
}

// divide world into uniform grid of numCells x numCells x numCells chunks
// with side lengths equal to cellL >= targetSideL
void SpatialGrid::Subdivide()
{
	// start from min, add cellL to the x-coord numcells times
	const auto &min = topNode.bb.min;
	v3 old_min{};
	v3 new_min{};
	Log::PrintLn("topNode.bb.min", glm::to_string(min));
	// outer loop is over Y
	for (int k = 0; k < numCells; k++)
	{
		old_min = min + (float)(k)*v3(0.f, cellL, 0.f);
		// middle loop is over Z
		for (int j = 0; j < numCells; j++)
		{
			if (j != 0)
			{
				old_min = old_min + v3(0.f, 0.f, cellL);
			}
			// inner loop is over X
			for (int i = 0; i < numCells; i++)
			{
				new_min = old_min + (float)(i)*v3(cellL, 0.f, 0.f);
				BoundingBox bb{new_min, new_min + v3{cellL, cellL, cellL}};
				// add Node and cell id to vectors
				grid.push_back(Cell{bb});
				id.push_back(iv3{i, k, j});
				AddLinesToVertBuf(bb);
			}
		}
	}
}

const std::vector<unsigned> &  SpatialGrid::FindNearestNeighbors(const gxb::Entity *const o)
{
	nearest_neighbor_entity_ids.resize(0);
	unsigned object_id = o->id;
	const auto &occupied_grid_ids = entity_id_to_list_of_cell_ids[object_id];

	for (const auto &grid_id : occupied_grid_ids)
	{
		for (const auto &curr_entity_id : grid[grid_id].list)
		{
			nearest_neighbor_entity_ids.push_back(curr_entity_id);
		}
	}
	// unique only works on a sorted vector
	std::sort(nearest_neighbor_entity_ids.begin(), nearest_neighbor_entity_ids.end());
	nearest_neighbor_entity_ids.erase(
		std::unique(nearest_neighbor_entity_ids.begin(), nearest_neighbor_entity_ids.end()),
		nearest_neighbor_entity_ids.end());
	// remove the object own id from the neighbor list
	nearest_neighbor_entity_ids.erase(
		std::remove(nearest_neighbor_entity_ids.begin(), nearest_neighbor_entity_ids.end(), object_id),
		nearest_neighbor_entity_ids.end());
	return nearest_neighbor_entity_ids;
}

bool SpatialGrid::FineGrainCollisionCheck(const gxb::Entity *const a, const gxb::Entity *const b, v3 resultant_accel_modifier)
{
	assert(level != nullptr);
	gxb::mesh* ma = level->GetMesh(a->mesh_hash);
	gxb::mesh* mb = level->GetMesh(b->mesh_hash);
	// translate mesh to object positions
	// rotate mesh to object rotations
	return false;
}

//@TODO: ADD PROPER TESTS USING CTEST OR GOOGLE TEST
void SpatialGrid::TestingStuffForOctree()
{
	Log::PrintLn("TESTING::TestingStuffForOctree");
	using std::cout, std::endl;
	cout << "id.size():" << id.size() << endl;
	Log::PrintLn("sizeof ivec3: ", sizeof(iv3));
	Log::PrintLn("sizeof size_t: ", sizeof(size_t));
	Log::PrintLn("sizeof unsigned: ", sizeof(unsigned));
}


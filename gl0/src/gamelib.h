#pragma once
#include "camera.h"
#include "flags.h"
#include "glm.h"
#include "headers.h"
#include "log.h"
#include <magic_enum.h>

//@DESIGN: prefer passing a variable around to creating a global. watch out for
// thread issues.

using v3 = glm::vec3;

namespace gxb
{
inline const auto app_root = std::string{R"(c:\cprojects\gl0\)"};
inline const auto texture_path = app_root + R"(textures\)";
inline const auto model_root = app_root + R"(models\)";
inline const auto level_root = app_root + R"(levels\)";
inline const auto shader_root = app_root + R"(shaders\)";

inline const unsigned int SCR_WIDTH = 1024;
inline const unsigned int SCR_HEIGHT = 768;

inline std::hash<std::string> str_hasher;



enum class EntityType
{
	unknown,
	hero,
	box,
	ground,
	moving_ground_x,
	moving_ground_z,
	moving_ground_y,
	fruit,
	baddie
};

inline std::map<std::string, EntityType> str_to_type{
	{"hero", EntityType::hero},
	{"box", EntityType::box},
	{"ground", EntityType::ground},
	{"fruit", EntityType::fruit},
	{"baddie", EntityType::baddie},
	{"moving_ground_x", EntityType::moving_ground_x},
	{"moving_ground_z", EntityType::moving_ground_z},
	{"moving_ground_y", EntityType::moving_ground_y},
};

inline std::map<EntityType, std::string> type_to_str{};

inline void initTypeToStrMap()
{
	auto create_entry = [](decltype(*str_to_type.begin()) thing) { type_to_str[thing.second] = thing.first; };
	std::for_each(str_to_type.begin(), str_to_type.end(), create_entry);
}

//********************************************************
//					IdFactory
//********************************************************
struct IdFactory
{
	static unsigned getNewId()
	{
		int retVal = 0;
		retVal = count_;
		count_++;
		return retVal;
	}

  private:
	IdFactory()
	{
	}
	inline static unsigned count_{0};
};

//********************************************************

struct PathPt
{
	v3 pos;
	float dist;
};

// A simple binary state machine for moving platforms
struct BinaryFsm
{
	enum class States
	{
		pos = 1,
		neg = -1
	};
	States current{States::pos};
	void check_transition(glm::vec3 displacement, v3 facing, float target, const v3 pos_dir)
	{
	    float dist = glm::length(displacement);
		float signed_magnitude_disp = displacement.x + displacement.y + displacement.z;
		if (magic_enum::enum_name(current) == "pos" && glm::dot(facing, pos_dir) > 0 && signed_magnitude_disp > target)
		{
			current = States::neg;
		}
		if (magic_enum::enum_name(current) == "neg" && glm::dot(facing, -1.f * pos_dir) > 0 && signed_magnitude_disp < -1.0f * target)
		{
			current = States::pos;
		}
	}
};

struct Entity
{
	Entity() = default;
	const unsigned id{IdFactory::getNewId()};
	std::size_t mesh_hash{};
	EntityType type;
	v3 pos{};
	v3 rot{};
	v3 pos_start{}; // could be in derived type instead
	v3 pos_last{};
	v3 vel{};
	v3 accel{};
	bool has_been_added_to_grid{false};
	BinaryFsm state_machine{BinaryFsm::States::pos};
};


struct mesh
{
	mesh() = default;
	std::string name{};
	std::size_t hash_code{};
	std::vector<v3> vertices;
	std::vector<v3> faces;
	std::vector<v3> normals;
	size_t pos_first_vert{};
	float spherical_diameter{};
};

struct CamPath
{
	CamPath() = delete;
	CamPath(std::vector<v3> control_points)
	{
		cps = control_points;
	}
	void createPathFromCps() 
	{
		using std::vector;
		// cps.size() == 3 + 2
		for (size_t cpIdx = 0; cpIdx <= cps.size() - 3; cpIdx += 2)
		{
			const v3 &p0 = cps[cpIdx];
			const v3 &p1 = cps[cpIdx + 1];
			const v3 &p2 = cps[cpIdx + 2];
			float x, y, z, t;
			x = y = z = t = 0.0f;
			while (t <= 1.0)
			{
				x = (1 - t) * (1 - t) * p0.x + 2 * (1 - t) * t * p1.x + t * t * p2.x;
				y = (1 - t) * (1 - t) * p0.y + 2 * (1 - t) * t * p1.y + t * t * p2.y;
				z = (1 - t) * (1 - t) * p0.z + 2 * (1 - t) * t * p1.z + t * t * p2.z;
				pts.push_back(v3{x, y, z});
				t += 0.05f;
			}
		}
	}
	std::vector<v3> cam_dir;
	std::vector<v3> pts;
	std::vector<v3> cps;
};

struct Level
{
	std::vector<std::unique_ptr<mesh>> meshes;
	std::vector<std::unique_ptr<Entity>> objects;
	std::vector<unsigned int> vaos;
	std::vector<float> raw_data;
	std::vector<gxb::PathPt> path;
	std::string name;

	mesh *GetMesh(const size_t hashCode) const
	{
		auto is_match = [&hashCode](const auto &m) { return m->hash_code == hashCode; };

		if (auto result = std::find_if(meshes.begin(), meshes.end(), is_match); result != meshes.end())
		{
			return result->get();
		}
		return nullptr;
	}
};

inline Entity* ObjectIdToEntity(const Level* const l, unsigned id) {
		auto is_match = [&id](const auto &e) { return e->id == id; };
		const auto & entities = l->objects;
		const auto & end_it = entities.end();
		if (auto result = std::find_if(entities.begin(), entities.end(), is_match); result != end_it)
		{
			return result->get();
		}
		return nullptr;
}

inline std::unique_ptr<std::string> LevelPath(const std::string name, const char * const ext)
{
	auto path = std::make_unique<std::string>(level_root + name + "." + ext);
	return path;
}

inline std::unique_ptr<std::string> ModelPath(const std::string name)
{
	auto path = std::make_unique<std::string>(model_root + name + ".obj");
	return path;
}

inline std::unique_ptr<std::string> ShaderPath(const std::string name)
{
	auto path = std::make_unique<std::string>(shader_root + name);
	return path;
}

inline std::optional<std::pair<int, int>> ExtractPairOfInts(std::string & token, const std::string &delim)
{
	size_t pos = 0;
	pos = token.find(delim);
	if (pos == std::string::npos)
	{
		return std::nullopt; // return on error
	}
	std::string firstNum;
	int face_id, normal_id;
	face_id = normal_id = -1;
	firstNum = token.substr(0, pos);
	face_id = (unsigned int)atoi(firstNum.c_str());
	token = token.erase(0, pos + delim.length());
	normal_id = (unsigned int)atoi(token.c_str());
	return std::optional<std::pair<int, int>>{std::make_pair(face_id, normal_id)};
}

// v  float float float
// vn float float float
// f  1// 1 22//22 9//9
//
inline std::unique_ptr<mesh> LoadMeshFromDisk(const char * const name)
{
	using std::string;
	using std::stringstream;
	stringstream file_data{};
	file_data = slurp::GetFileContents(ModelPath(name)->c_str());

	// add error checking and return null
	auto m = std::make_unique<mesh>();
	m->name = name;

	unsigned int line_num = 0;
	string line{}, first_tok{};

	while (1)
	{
		line_num++;
		getline(file_data, line, '\n');

		if (file_data.fail())
			break;

		stringstream line_stream{line};
		line_stream >> first_tok;

		if (first_tok == "v")
		{
			// read vertex
			v3 pos = {0.f, 0.f, 0.f};
			line_stream >> pos.x >> pos.y >> pos.z;
			if (line_stream.fail())
			{
				Log::LogErr(__FILE__, __LINE__, "trouble reading position data");
			}
			m->vertices.push_back(pos);
		}

		if (first_tok == "vn")
		{
			// read normal
			v3 normal;
			line_stream >> normal.x >> normal.y >> normal.z;
			if (line_stream.fail())
			{
				Log::PrintLn("error reading normal coords");
			}
			m->normals.push_back(normal);
		}

		if (first_tok == "f")
		{
			string token;
			glm::u32vec3 faces;

			for (int i = 0; i < 3; i++)
			{
				line_stream >> token; // example 1//1
				std::string delimiter = "//";
				auto result_pair = ExtractPairOfInts(token, delimiter);
				if (!result_pair)
				{
					Log::LogErr("ERROR:: < file line # , model name > ::", line_num, name);
				}
				auto [face_id, normal_id] = *result_pair;
				faces[i] = face_id;
			}
			m->faces.push_back(faces);
		}
	}

	return m;
}

inline std::vector<PathPt> LoadCamPath(const std::string level_name)
{
	using std::vector, std::ifstream, std::make_unique;
	bool fileExist = slurp::CheckFileExist(level_root, level_name, "cmp");
	if (!fileExist)
	{
		Log::PrintLn("CamPath file for level", level_name, " was not found.\n");
		return vector<PathPt>{};
	}

	// load camPath
	vector<PathPt> pts;
	auto path_data = slurp::GetFileContents(LevelPath(level_name, "cmp")->c_str());

	while (path_data.good())
	{
		float x{}, y{}, z{};
		path_data >> x >> y >> z;
		if (path_data.good())
			pts.push_back(PathPt{v3{x, y, -z}, 0.0f});
	}
	Log::PrintLn(pts.size(), "points loaded from", level_name, ".cmp");

	Log::PrintLn("CamPath file for ", level_name, " loaded");

	return std::move(pts);
}

inline float CalculateMeshSphericalDiameter(const mesh *const m)
{
	const auto &verts = m->vertices;
	const auto sz = verts.size();

	float max_distance{0.f}; // max dist between any 2 vertices
	// loop over all distinct pairs of mesh vertices
	for (int i = 0; i < sz - 1; i++)
	{
		for (int j = 1; j < sz; j++)
		{
			const auto tmp_dist = glm::distance(verts[i], verts[j]);
			max_distance = (max_distance < tmp_dist) ? tmp_dist : max_distance;
		}
	}
	return max_distance;
}

// format of level
// 							 pos   rot
// entity_type  model_name  x y z x y z
inline std::unique_ptr<Level> LoadLevelMeshesAndCamPath(const std::string level_name)
{
	auto l = std::make_unique<Level>();
	std::string line, entity_name = "";

	bool levelExist = slurp::CheckFileExist(level_root, level_name, "txt");

	if (levelExist)
	{
		auto levelData = slurp::GetFileContents(LevelPath(level_name, "txt")->c_str());
		Log::PrintLn("SUCCESS:: level", level_name, "slurped from disk");

		int line_num = 0;

		Log::PrintLn("mesh         	  v        n        f        hash			spherical diam");

		while (levelData.good())
		{
			line_num++;
			getline(levelData, line,
					'\n'); // getline sets stream bits on error

			std::stringstream line_stream{line};
			line_stream >> entity_name;
			auto type = str_to_type.count(entity_name) ? str_to_type[entity_name] : EntityType::unknown;
			if (type == EntityType::unknown)
			{
				Log::LogErr(level_name, line_num, "Entity type unknown");
				continue;
			}

			std::string mesh_name;
			v3 pos{};
			v3 rot{};
			line_stream >> mesh_name;
			line_stream >> pos.x >> pos.y >> pos.z;
			line_stream >> rot.x >> rot.y >> rot.z;

			const bool line_stream_bad = line_stream.fail();
			if (line_stream_bad && levelData.eof())
			{
				break;
			}

			if (line_stream_bad)
			{
				Log::PrintLn("ERROR: wrong values on line <", line_num, ">", "level:", level_name);
				continue;
			}

			// load model file into level struct
			auto mesh_ptr = std::make_unique<mesh>();
			auto entity_ptr = std::make_unique<Entity>();

			// set mesh_ptr properties
			size_t mesh_hash_code = str_hasher(mesh_name);

			entity_ptr->type = str_to_type[entity_name];
			entity_ptr->pos = pos;
			entity_ptr->pos_last = pos;
			entity_ptr->pos_start = pos;
			entity_ptr->rot = rot;
			entity_ptr->mesh_hash = mesh_hash_code;

			bool model_exist = slurp::CheckFileExist(model_root, mesh_name, "obj");
			bool mesh_already_loaded = (l->GetMesh(mesh_hash_code) == nullptr) ? false : true;

			if (!model_exist)
			{
				// can only reach this line if model file was not found
				Log::PrintLn("ERROR::missing mesh file:", mesh_name, "while loading level:", level_name);
				return nullptr;
			}

			// This does a few unnecesary copies to load up mesh_ptr with a copy of the proper mesh, but not a big deal
			if (mesh_already_loaded)
			{
				mesh *mesh_ = l->GetMesh(mesh_hash_code);
				std::copy(mesh_->vertices.begin(), mesh_->vertices.end(), std::back_inserter(mesh_ptr->vertices));
				std::copy(mesh_->faces.begin(), mesh_->faces.end(), std::back_inserter(mesh_ptr->faces));
				std::copy(mesh_->normals.begin(), mesh_->normals.end(), std::back_inserter(mesh_ptr->normals));
				// set mesh_ptr properties
				mesh_ptr->name = mesh_name;
				mesh_ptr->hash_code = mesh_hash_code;
			}

			// create one giant raw_data array on the level to hold all model
			// triangles
			// @Note: vertices are in raw data in the order that model is in the
			// meshes vector
			if (!mesh_already_loaded)
			{

				// read in vertices, normals, and faces from disk; deletes existing mesh_ptr data
				mesh_ptr = LoadMeshFromDisk(mesh_name.c_str());
				// set mesh_ptr properties
				mesh_ptr->name = mesh_name;
				mesh_ptr->hash_code = mesh_hash_code;
				// calculate diameter of a spherical bounding box around the mesh
				mesh_ptr->spherical_diameter = CalculateMeshSphericalDiameter(mesh_ptr.get());

				int face_added_to_raw = 0;
				auto &v = mesh_ptr->vertices;
				mesh_ptr->pos_first_vert = l->raw_data.size() / 3;
				for (const auto &face : mesh_ptr->faces)
				{
					// push a float onto vertexarray
					// @NOTE: faces integers in object file start at 1 instead
					// of 0
					l->raw_data.push_back(v[(size_t)face.x - 1].x);
					l->raw_data.push_back(v[(size_t)face.x - 1].y);
					l->raw_data.push_back(v[(size_t)face.x - 1].z);
					l->raw_data.push_back(v[(size_t)face.y - 1].x);
					l->raw_data.push_back(v[(size_t)face.y - 1].y);
					l->raw_data.push_back(v[(size_t)face.y - 1].z);
					l->raw_data.push_back(v[(size_t)face.z - 1].x);
					l->raw_data.push_back(v[(size_t)face.z - 1].y);
					l->raw_data.push_back(v[(size_t)face.z - 1].z);
					face_added_to_raw++;
				}
				const std::string spacer(15 - mesh_name.length(), ' ');
				Log::PrintLn(mesh_name, spacer, mesh_ptr->vertices.size(), mesh_ptr->normals.size(),
						   mesh_ptr->faces.size(), mesh_ptr->hash_code, spacer, mesh_ptr->spherical_diameter);

				if constexpr (Flags::USE_ASSERTIONS)
					assert(mesh_ptr->hash_code != 0);

				l->meshes.push_back(std::move(mesh_ptr));
			}

			l->objects.push_back(std::move(entity_ptr));

		} // end while

		// Load campath
		l->path = LoadCamPath(level_name);
		Log::PrintLn("objects created:", l->objects.size());
		Log::PrintLn("meshes loaded from disk:", l->meshes.size());
		l->name = level_name;
		return std::move(l);
	}
	return nullptr;
} // function LoadLevelMeshesAndCamPath

} // namespace gxb


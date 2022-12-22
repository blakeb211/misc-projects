/* Goal: draw an octree */

#define OLC_PGE_APPLICATION
#include <cmath>
#include <vector>
#include "..\include\glm.h"
#include "olcPixelGameEngine.h"
#define PI 3.1415926535
using std::vector;

using v3 = glm::vec3;
using v2 = glm::vec2;
using m44 = glm::mat4;
using v4 = glm::vec4;
/*******************************************/
// GLOBALS
/*******************************************/
constexpr auto g_WIDTH = 650;
constexpr auto g_HEIGHT = 650;
int xang = 0, yang = 0, zang = 0;



void handle_user_input(olc::PixelGameEngine* engine) {
    auto xposKeyState = engine->GetKey(olc::Key::A);
    auto xnegKeyState = engine->GetKey(olc::Key::Z);

    auto yposKeyState = engine->GetKey(olc::Key::S);
    auto ynegKeyState = engine->GetKey(olc::Key::X);

    auto zposKeyState = engine->GetKey(olc::Key::D);
    auto znegKeyState = engine->GetKey(olc::Key::C);

	if (xposKeyState.bHeld)
		xang += 1;
	if (xnegKeyState.bHeld)
		xang -= 1;
	
	if (yposKeyState.bHeld)
		yang += 1;
	if (ynegKeyState.bHeld)
		yang -= 1;

	if (zposKeyState.bHeld)
		zang += 1;
	if (znegKeyState.bHeld)
		zang -= 1;
}


// Override base class with your custom functionality
class Example : public olc::PixelGameEngine {
   public:
    Example() {
	// Name your application
	sAppName = "Example";
    }

   public:
    bool OnUserCreate() override {
	// Called once at the start, so create things here
	projected_points = vector<v2>(points.size());
	return true;
    }

    bool OnUserUpdate(float fElapsedTime) override {
	totElapsedTime += fElapsedTime;
	Clear(olc::BLACK);
	handle_user_input(this);
	// create projection
	auto points_sz = points.size();
	for (int i = 0; i < points_sz; i++) {
	    const auto& pt = points[i];
	    model = glm::mat4(1.0f);
	    model = glm::rotate(model, glm::radians(xang*1.0f),
				glm::vec3(1.0, 0.0, 0.0));
	    model = glm::rotate(model, glm::radians(yang*1.0f),
				glm::vec3(0.0, 1.0, 0.0));
	    model = glm::rotate(model, glm::radians(zang*1.0f),
				glm::vec3(0.0, 0.0, 1.0));
	    const auto tmp_v4 =
		projection * view * model * v4(pt.x, pt.y, pt.z, 1.0);
	    projected_points[i].x = tmp_v4.x;
	    projected_points[i].y = tmp_v4.y;
	    // shift and scale
	    projected_points[i].x *= 120.0f;
	    projected_points[i].y *= 120.0f;
	    projected_points[i].x += g_WIDTH / 2.0f;
	    projected_points[i].y += g_HEIGHT / 2.0f;
	}

	// Called once per frame, draws random coloured pixels
	for (int i = 0; i < points_sz - 1; i += 2) {
	    const auto& pt = projected_points[i];
	    const auto& pt2 = projected_points[i + 1];
	    DrawLine(pt.x, pt.y, pt2.x, pt2.y, olc::WHITE);
	}
	return true;
    }
    /* GAME VARS */
    float totElapsedTime;
    vector<v3> points{};
    vector<v2> projected_points;
    m44 projection;
    m44 view;
    m44 model;
};


int main() {
    Example demo;
    demo.model = glm::mat4(1.0f);
    demo.projection = glm::mat4(1.0f);
    demo.view = glm::mat4(1.0f);
    demo.projection = glm::perspective(
	(float)PI / 2.0f, (float)g_WIDTH / (float)g_HEIGHT, 0.1f, 100.0f);

    // 					y +1
    //					y
    //					y
    //					y //					y //					y
    // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
    //-1			    y +1
    //					y
    //					y
    //					y
    //					y
    //					y
    //					y -1

    // build a grid
    // draw lines parallel to y axis
    for (float k = -1.f; k <= 1.f; k += 0.50f)
	for (float i = -1.f; i <= 1.f; i += 0.50f) {
	    demo.points.push_back(glm::vec3{i, -1, k});
	    demo.points.push_back(glm::vec3{i, 1, k});
	}
    // draw lines parallel to x axis
    for (float k = -1.f; k <= 1.f; k += 0.50f)
	for (float j = -1.f; j <= 1.f; j += 0.50f) {
	    demo.points.push_back(glm::vec3{-1, j, k});
	    demo.points.push_back(glm::vec3{1, j, k});
	}
    // draw lines parallel to z axis
    for (float i = -1.f; i <= 1.f; i += 0.50f)
	for (float j = -1.f; j <= 1.f; j += 0.50f) {
	    demo.points.push_back(glm::vec3{i, j, -1});
	    demo.points.push_back(glm::vec3{i, j, 1});
	}

    if (demo.Construct(g_WIDTH, g_HEIGHT, 1, 1, false, true))
	demo.Start();

    return 0;
}

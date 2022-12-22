#include <glad\glad.h>
//
#include <GLFW\glfw3.h>
//
#include "framerater.h"
#include "gamelib.h"
#include "glm.h"
#include "headers.h"
#include "octree.h"
#include "render.h"
#include "shader.h"
#include <magic_enum.h>
//#include <reactphysics3d/reactphysics3d.h> 
// -------------------------------------------
// TYPEDEFS
// -------------------------------------------
using VecPP = std::vector<gxb::PathPt>;
using v3 = glm::vec3;
using iv3 = glm::ivec3;

// -------------------------------------------
// FORWARD DECLARATIONS
// -------------------------------------------

// -------------------------------------------
// GLOBALS
// -------------------------------------------
gxb::Camera camera{};
float last_x = gxb::SCR_WIDTH / 2, lastY = gxb::SCR_HEIGHT / 2;
bool first_mouse = true;
gxb::Level* curr_level = nullptr;
std::vector<std::unique_ptr<gxb::Level>> levels;
constexpr auto CAM_MOVE_SPEED = 0.001f;

void TerminateProgram(int exit_code)
{
  glfwTerminate();
  Log::CloseLog();
  std::exit(exit_code);
}
// process all input: move player only
// ---------------------------------------------------------------------------------------------------------
void ProcessInputPlayerOnly(GLFWwindow* window, float delta_time)
{
  const auto playerSpeed = 0.017f;
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
  auto& pos = curr_level->objects[0]->pos;
  // save last position before changing the position
  curr_level->objects[0]->pos_last = pos;

  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
  {
    pos.x += playerSpeed * delta_time;
  }

  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
  {
    pos.x -= playerSpeed * delta_time;
  }

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
  {
    pos.z -= playerSpeed * delta_time;
  }

  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
  {
    pos.z += playerSpeed * delta_time;
  }

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
  {
    pos.y += playerSpeed * delta_time;
  }

  if (glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
  {
    pos.y -= playerSpeed * delta_time;
  }

  if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    __noop;

  if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    __noop;
}

void ProcessInputLoadingScreen(GLFWwindow* window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}
// process all input: move camera only
// ---------------------------------------------------------------------------------------------------------
void ProcessInputCamOnly(GLFWwindow* window, gxb::Camera& cam, float delta_time)
{
  const float cameraSpeed = 0.16f; // adjust accordingly
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    cam.ProcessKeyboard(gxb::Camera_Movement::RIGHT, delta_time, cameraSpeed);

  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    cam.ProcessKeyboard(gxb::Camera_Movement::LEFT, delta_time, cameraSpeed);

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    cam.ProcessKeyboard(gxb::Camera_Movement::FORWARD, delta_time, cameraSpeed);

  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    cam.ProcessKeyboard(gxb::Camera_Movement::BACKWARD, delta_time, cameraSpeed);

  if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    __noop;

  if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    __noop;
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void FrameBufSizeCallback(GLFWwindow* window, int width, int height)
{
  glViewport(0, 0, width, height);
}

void MouseCallback(GLFWwindow* window, double x_pos, double y_pos)
{
  if (first_mouse)
  {
    last_x = (float)x_pos;
    lastY = (float)y_pos;
    first_mouse = false;
  }

  float xoffset = (float)x_pos - last_x;
  float yoffset = lastY - (float)y_pos; // reversed since y-coordinates go from bottom to top

  last_x = (float)x_pos;
  lastY = (float)y_pos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

void MouseCallbackNull(GLFWwindow* window, double xpos, double ypos)
{
  __noop;
  return;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll((float)yoffset);
}

GLFWwindow* InitGlfw(unsigned int w, unsigned int h, const char* title, GLFWframebuffersizefun fun)
{
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  // glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(w, h, title, NULL, NULL);
  if (window == NULL)
  {
    Log::PrintLn("Failed to create GLFW window");
    glfwTerminate();
    return nullptr;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, fun);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    Log::PrintLn("Failed to initialize GLAD");
    return nullptr;
  }

  if (Flags::FREE_MOVE)
    glfwSetCursorPosCallback(window, MouseCallback);
  else
    glfwSetCursorPosCallback(window, MouseCallbackNull);

  glfwSetScrollCallback(window, ScrollCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (Flags::VSYNC)
    glfwSwapInterval(1); // vsync on
  else
    glfwSwapInterval(0); // vsync off

  glEnable(GL_MULTISAMPLE);
  render::logOpenGLInfo();

  render::clearScreen();

  return window;
}

v3 SelectNextCamPoint(const gxb::Level* const l, gxb::Camera& cam)
{
  const auto heroPos = l->objects[0]->pos;
  const auto negZvec = v3{ 0.f, 0.f, -1.f };

  size_t path_sz = l->path.size();
  std::vector<std::pair<float, float>> dist_ang(path_sz);

  v3 pathPos{}, cam2hero{};
  float dist{}, angle{};

  for (int i = 0; i < path_sz; i++)
  {
    pathPos = l->path[i].pos;
    cam2hero = v3{ glm::normalize(heroPos - pathPos) };
    dist = glm::distance(heroPos, l->path[i].pos);
    angle = glm::acos(glm::dot(negZvec, cam2hero));
    dist_ang[i].first = dist;
    dist_ang[i].second = angle;
  }

  // find first pathPt that has distance < 20 && angle < 40
  using pair = std::pair<float, float>;
  // set camera to new position
  constexpr auto distCutoff = 30;
  constexpr auto angCutoff = 20;

  auto check = [distCutoff, angCutoff](const pair& dap) {
    return (dap.first < distCutoff&& dap.second < angCutoff);
  };

  for (int i = 0; i < path_sz; i++)
  {
    if (check(dist_ang[i]))
    {
      return l->path[i].pos;
    }
  }
  return v3{}; // if no valid campath found, cam goes to 0,0,0
}

void AddCamPathToRawData(gxb::Level* l)
{
  auto func = [l](const gxb::PathPt pp) {
    for (int i = 0; i < 3; i++)
    {
      l->raw_data.push_back(pp.pos[i]);
    }
  };
  std::for_each(l->path.begin(), l->path.end(), func);
}

void CamGoalSeek(float delta_time)
{
  auto new_cam_goal_pos = SelectNextCamPoint(curr_level, camera);
  // smoothly move cam towards goal offset
  if (camera.Position != new_cam_goal_pos)
  {
    const auto camDp = new_cam_goal_pos - camera.Position;
    camera.moveTo(camera.Position + delta_time * CAM_MOVE_SPEED * camDp);
    camera.Front = curr_level->objects[0]->pos - camera.Position; // look at hero
  }
}

void TestNaiveCollision()
{
  const auto ocnt = curr_level->objects.size();
  int num_checks{ 0 };
  for (int i = 0; i < ocnt - 1; i++)
  {
    for (int j = i + 1; j < ocnt; j++)
    {
      num_checks++;
    }
  }
  Log::PrintLn("NAIVE: num of collision checks:", num_checks);
}

void TestSpatialGridCollision()
{
  size_t num_checks{ 0 };
  for (const auto& o : curr_level->objects)
  {
    auto neighbors = SpatialGrid::FindNearestNeighbors(o.get());
    num_checks += neighbors.size();
  }
  Log::PrintLn("Spatial Grid: num of collision checks:", num_checks);
}

void ShowLevelLoading(GLFWwindow* window, unsigned int vao_loading, const Shader& prog_one, std::atomic<bool>& done)
{
  using namespace std::chrono_literals;
  float rot{ 0.f }, offset{ -0.2f }, scale{ 0.3f };
  bool dir_flag{ false };
  int dot_count = 0;
  const int dot_count_max = 40;
  for (;;)
  {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    ProcessInputLoadingScreen(window);

    if (dot_count == dot_count_max)
    {
      dot_count = 0;
    }
    dot_count++;
    auto str = std::string("Loading") + std::string(dot_count / 2, '.');
    glfwSetWindowTitle(window, str.c_str());

    // Update
    constexpr float dY = 0.014f;
    if (offset > -(1.f - 0.5f * scale) && !dir_flag)
    {
      offset -= dY;
    }
    if (offset < -(1.f - 0.5f * scale))
      dir_flag = !dir_flag;
    if (offset > (1.f - 0.5f * scale))
      dir_flag = !dir_flag;
    if (dir_flag)
    {
      offset += dY;
    }
    // render
    render::clearScreen();
    render::DrawLoadingScreen(vao_loading, prog_one, offset, rot, scale);

    glfwSwapBuffers(window);
    glfwPollEvents();
    if (glfwWindowShouldClose(window))
    {
      TerminateProgram(0);
    }
    if (done)
    {
      break;
    }
  } // end loading screen loop
}

void TestReactPhysics3d() {
////  // ReactPhysics3D namespace
 // using namespace reactphysics3d;

 // // First you need to create the PhysicsCommon object.
 // // This is a factory module that you can use to create physics
 // // world and other objects. It is also responsible for
 // // logging and memory management
 // PhysicsCommon physicsCommon;

 // // Create a physics world
 // PhysicsWorld* world = physicsCommon.createPhysicsWorld();

 // // Create a rigid body in the world
 // Vector3 position(0, 20, 0);
 // Quaternion orientation = Quaternion::identity();
 // Transform transform(position, orientation);
 // RigidBody* body = world->createRigidBody(transform);

}

void UpdatePlatformPos(std::unique_ptr<gxb::Entity>& o, const v3& pos_dir, const float& frame_time, float speedup = 1.0)
{
  v3 facing = o->pos - o->pos_last;
  facing = glm::normalize(facing);
  o->pos_last = o->pos;
  if (magic_enum::enum_name(o->state_machine.current) == "pos")
  {
    o->pos += pos_dir * 0.001f * frame_time * speedup;
  }
  else
  {
    o->pos += -1.0f * pos_dir * 0.001f * frame_time * speedup;
  }
  o->state_machine.check_transition(o->pos - o->pos_start, facing, 4, pos_dir);
}

int main()
{
  gxb::initTypeToStrMap(); // creates str_to_type
  FrameRater fr{};
  Log::SetLogFile(gxb::app_root + "log.txt");

  const auto& w = gxb::SCR_WIDTH;
  const auto& h = gxb::SCR_HEIGHT;
  GLFWwindow* window = InitGlfw(w, h, "Learn OpenGL ", FrameBufSizeCallback);
  render::SetGlFlags();

  //@TODO: add ability to switch levels while game running
  // Load Level
  auto level_name = "test";

  //@TODO: Not sure if there is a point in doing this in another thread unless I'm
  // going to save it to a future_ptr, do some other work and call .get() on it later
  // Possibly it is preventing the screen from freezing?
  auto prog_loading = Shader(*gxb::ShaderPath("3pos_no_projection.vs"), *gxb::ShaderPath("colorFromVertex.fs"));
  auto prog_one = Shader(*gxb::ShaderPath("3pos3color.vs"), *gxb::ShaderPath("colorFromVertex.fs"));
  std::atomic<bool> level_loaded{ false };
  using namespace std::chrono_literals;
  {
    auto LoadLevel = [&]() {
      levels.push_back(gxb::LoadLevelMeshesAndCamPath(level_name));
      curr_level = levels.back().get();
      SpatialGrid::SetupOctree(curr_level);
      // add camPath points to level raw_data so I can draw them for debug
      AddCamPathToRawData(curr_level);
      std::this_thread::sleep_for(1000ms);
      level_loaded = true;
    };

    std::thread loading_thread{ LoadLevel };
    unsigned vao_loading = render::BuildLoadingScreenVao();
    prog_loading.Use();
    ShowLevelLoading(window, vao_loading, prog_loading, level_loaded);
    loading_thread.join();
  }

  auto view = glm::mat4(1.0);
  auto projection = glm::mat4(1.0);

  unsigned vao_spatial_grid = render::BuildSpatialGridVao(SpatialGrid::GetVertBufGridLinesRef());
  unsigned vao_models = render::BuildLevelVao(curr_level);


  TestReactPhysics3d();
  // Game loop
  // -----------
  while (!glfwWindowShouldClose(window))
  {

    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    fr.UpdateTimes();
    float delta_time = fr.lastTimeInMs();

    if (Flags::FREE_MOVE)
      ProcessInputCamOnly(window, camera, delta_time);
    else
    {
      ProcessInputPlayerOnly(window, delta_time);
      CamGoalSeek(delta_time);
    }

    // update window title with player position
    const glm::vec3& pos = curr_level->objects[0]->pos;
    const glm::ivec3 grid_coords{ SpatialGrid::PosToGridCoords((v3&)pos) };
    auto str = std::string(glm::to_string(pos) + " " + glm::to_string(grid_coords) +
      " fr: " + std::to_string(static_cast<int>(fr.most_recent_frame_rate)));
    glfwSetWindowTitle(window, str.c_str());

    prog_one.Use();

    // update objects
    // movement is moderated by the elapsed time in case we change the framerate later
    for (auto& o : curr_level->objects)
    {
      auto const elapsed = fr.lastTimeInMs();
      v3 pos_dir;
      switch (o->type)
      {
      case gxb::EntityType::moving_ground_x:
        pos_dir = v3(1.f, 0.f, 0.f);
        UpdatePlatformPos(o, pos_dir, elapsed, 1.3f);
        break;
      case gxb::EntityType::moving_ground_y:
        pos_dir = v3(0.f, 1.f, 0.f);
        UpdatePlatformPos(o, pos_dir, elapsed, 2.0f);
        break;
      case gxb::EntityType::moving_ground_z:
        pos_dir = v3(0.f, 0.f, 1.f);
        UpdatePlatformPos(o, pos_dir, elapsed, 5.0f);
        break;
      }
    }
    // update spatial grid
    if (fr.frame_count % 1 == 0)
    {
      SpatialGrid::ClearGrid();
      for (auto& o : curr_level->objects)
      {
        SpatialGrid::UpdateGrid(o.get());
      }
      const auto near_neighbors = SpatialGrid::FindNearestNeighbors(curr_level->objects[0].get());
      render::highlighted_entities.resize(near_neighbors.size());
      std::copy(near_neighbors.begin(), near_neighbors.end(), render::highlighted_entities.begin());
    }

    // test for collisions
    // @NOTE: Once outer loop is only over dynamic objects this will be faster
    const auto sz = curr_level->objects.size();
    for (int i = 0; i < sz; i++)
    {
      const auto& curr_object_ptr = curr_level->objects[i].get();
      const auto near_neighbors = SpatialGrid::FindNearestNeighbors(curr_object_ptr);
      const auto nn_sz = near_neighbors.size();
      gxb::Entity* ea = gxb::ObjectIdToEntity(curr_level, curr_object_ptr->id);
      for (int j = 0; j < nn_sz; j++)
      {
        gxb::Entity* eb = gxb::ObjectIdToEntity(curr_level, near_neighbors[j]);
        v3 collision_response_vec{};
        SpatialGrid::FineGrainCollisionCheck(ea, eb, collision_response_vec);
      }
    }

    // set transformations
    view = camera.GetViewMatrix();
    prog_one.SetMat4("view", view);

    projection = glm::perspective(glm::radians(camera.Zoom), 800.0f / 600.0f, 0.1f, 100.0f);
    prog_one.SetMat4("projection", projection);

    // render
    // ------
    render::clearScreen();
    render::DrawLevel(vao_models, prog_one, vao_spatial_grid, curr_level);
    glfwSwapBuffers(window);
    glfwPollEvents();
    fr.printFrameRateIfFreqHasBeenReached();
  } // end game loop

  TerminateProgram(0);
  return 0;
}


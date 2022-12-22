// USAGE: This program reads in a ppm or campath file, lets users modify the
// campath with the mouse, and then saves the campath.
//
#define OLC_PGE_APPLICATION
#include "..\include\gamelib.h"
#include "..\include\glm.h"
#include "..\include\headers.h"
#include "olcPixelGameEngine.h"
#include "ppm_reader.h"

constexpr auto COORD_SCALE_FACTOR = 3;
constexpr auto BLUE_SCALE_FACTOR = 3;

// @NOTE: campath editor operates in world coordinators

using std::make_pair, std::make_unique, std::string;
using std::unique_ptr, std::tie, std::to_string, std::max_element;
using std::vector, std::pair, std::cout, std::setw, glm::vec3, std::endl;
using gxb::CamPath;
namespace fs = std::filesystem;

/************************************************
 *					GLOBALS
 *
 ************************************************/
size_t frameCnt = 0;
fs::path inFilePath{};
auto WORLD_Y_MAX = 30.0f;
auto WORLD_X_MAX = 50.0f;

float z_data_max{100};
float y_data_max{100};
float x_data_max{100};

constexpr auto comp_zmax = [](const glm::vec3 &a, const glm::vec3 &b) -> bool {
  return a.z < b.z;
};
constexpr auto comp_ymax = [](const glm::vec3 &a, const glm::vec3 &b) -> bool {
  return a.y < b.y;
};
constexpr auto comp_xmax = [](const glm::vec3 &a, const glm::vec3 &b) -> bool {
  return a.x < b.x;
};

/************************************************/
//@TODO:
// pop up pt coords
// print out CamPath points in a table
// ability to drag control points to change the campath
// ability to save CamPath

enum class View {
  ZY = 0,
  ZX = 1,
};

void save_campath_to_file(const CamPath *const campath, fs::path outPath) {
  outPath.replace_extension("cmp");
  std::ofstream out{outPath, std::ios::out};
  for (const auto &p : campath->pts) {
    out << setw(8) << p.x << " " << setw(8) << p.y;
    out << " " << setw(8) << p.z << endl;
  }
}

pair<float, float> world_to_screen(const olc::PixelGameEngine *gm,
                                   const View vw, const glm::vec3 in) {
  float y_offset{}, x_offset{};
  float y_scale{}, x_scale{};
  float x_coord{}, y_coord{};
  switch (vw) {
  case View::ZY:
    // Y is still Y but shifted
    // Z is shown on the X-Axis
    x_offset = 10;
    x_scale = gm->ScreenWidth() / (z_data_max + x_offset);
    x_coord = x_offset + in.z * x_scale;

    y_offset = gm->ScreenHeight() / 2.f;
    y_scale = -1.f * gm->ScreenHeight() / (2.f * WORLD_Y_MAX);
    y_coord = y_offset + in.y * y_scale;

    break;
  case View::ZX:
    // X data is shown on the Y-Axis
    // Z is shown on the X-Axis
    x_offset = 10;
    x_scale = gm->ScreenWidth() / (z_data_max + x_offset);
    x_coord = x_offset + in.z * x_scale;

    y_offset = gm->ScreenHeight() / 2.f;
    y_scale = -1.f * gm->ScreenHeight() / (2.f * WORLD_X_MAX);
    y_coord = y_offset + in.x * y_scale;
    break;
  };
  return make_pair(x_coord, y_coord);
}

class Example : public olc::PixelGameEngine {
public:
  View currView{View::ZX};
  unique_ptr<CamPath> path;
  vector<vec3> cps;

public:
  Example() = delete;
  Example(vector<vec3> cps) {
    sAppName = "Example";
    this->cps = cps;
  }
  bool OnUserCreate() override {
    // Called once at the start, so create things here
    this->path = make_unique<CamPath>(cps);
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
    frameCnt++;
    this->Clear(olc::Pixel(olc::DARK_GREY));

    auto viewKeyState = GetKey(olc::Key::V);
    if (viewKeyState.bReleased) {
      if (currView == View::ZY) {
        currView = View::ZX;
      } else {
        currView = View::ZY;
      }
    }

    auto saveKeyState = GetKey(olc::Key::S);
    if (saveKeyState.bReleased) {
      save_campath_to_file(path.get(), inFilePath);
    }
    // draw axes
    // ZY
    // Z = 0 					X = ScreenHeight / 2
    // Z = WorldZMax 			X = ScreenWidth
    // Y = 0 					ScreenHeight / 2
    // Y = WorldYMax 			0
    //

    if (frameCnt % 30 == 0) {
      const auto z_data_max_it =
          max_element(path->cps.begin(), path->cps.end(), comp_zmax);
      const auto y_data_max_it =
          max_element(path->cps.begin(), path->cps.end(), comp_ymax);
      const auto x_data_max_it =
          max_element(path->cps.begin(), path->cps.end(), comp_xmax);

      z_data_max = (*z_data_max_it).z;
      y_data_max = (*y_data_max_it).y;
      x_data_max = (*x_data_max_it).x;
      WORLD_Y_MAX = y_data_max + 5;
      WORLD_X_MAX = x_data_max + 5;

      path->pts.clear();
      path->createPathFromCps();
    }

    // draw axes
    int x0{}, y0{}, xmax{}, ymax{};
    switch (currView) {
    case View::ZY:
      // y = y + ScreenHeight/2
      tie(x0, y0) = world_to_screen(this, View::ZY, glm::vec3{0.f, 0.f, 0.f});
      tie(xmax, ymax) = world_to_screen(
          this, View::ZY, glm::vec3{0.f, WORLD_Y_MAX, z_data_max});
      DrawLine(x0, y0, xmax, y0, olc::WHITE);
      DrawLine(xmax / 2, ymax, xmax / 2, y0, olc::WHITE);
      break;
    case View::ZX:
      tie(x0, y0) = world_to_screen(this, View::ZY, glm::vec3{0.f, 0.f, 0.f});
      tie(xmax, ymax) = world_to_screen(
          this, View::ZY, glm::vec3{0.f, WORLD_X_MAX, z_data_max});
      DrawLine(x0, y0, xmax, y0, olc::WHITE);
      DrawLine(xmax / 2, ymax, xmax / 2, y0, olc::WHITE);
      break;
    };

    // draw campath
    // draw control points
    switch (currView) {
    case View::ZY:

      for (const auto &pt : path->pts) {
        // draw circles around control pts
        auto [x, y] = world_to_screen(this, View::ZY, pt);
        Draw(x, y, olc::RED);
        DrawCircle(x, y, 2, olc::RED);
      }

      for (const auto &pt : path->cps) {
        // draw circles around control pts
        auto [x, y] = world_to_screen(this, View::ZY, pt);
        Draw(x, y, olc::GREEN);
        DrawCircle(x, y, 3, olc::GREEN);
      }

      break;
    case View::ZX:

      for (const auto &pt : path->pts) {
        // draw circles around control pts
        auto [x, y] = world_to_screen(this, View::ZX, pt);
        Draw(x, y, olc::RED);
        DrawCircle(x, y, 2, olc::RED);
      }

      for (const auto &pt : path->cps) {
        // draw circles around control pts
        auto [x, y] = world_to_screen(this, View::ZX, pt);
        Draw(x, y, olc::GREEN);
        DrawCircle(x, y, 3, olc::GREEN);
      }

      break;
    };

    // draw axis labels
    switch (currView) {
      {
      case View::ZY:
        const string z_label_left = "Z(0)";
        const string y_label_top = "Y(" + to_string((int)WORLD_Y_MAX) + ")";
        float x = 2.0f, y = ScreenHeight() / 2.f + 5.f;
        DrawString(x, y, z_label_left, olc::WHITE, 1);
        tie(x, y) = world_to_screen(
            this, View::ZY,
            glm::vec3{0.f, WORLD_Y_MAX, (z_data_max + 2) / 2.f});
        DrawString(x, y, y_label_top, olc::WHITE, 1);
        break;
      }
    case View::ZX: {
      const string z_label_left = "Z(0)";
      const string y_label_top = "X(" + to_string((int)WORLD_X_MAX) + ")";
      float x = 2.0f, y = ScreenHeight() / 2.f + 5.f;
      DrawString(x, y, z_label_left, olc::WHITE, 1);
      tie(x, y) = world_to_screen(
          this, View::ZX, glm::vec3{WORLD_X_MAX, 0.f, (z_data_max + 2) / 2.f});
      DrawString(x, y, y_label_top, olc::WHITE, 1);
      break;
    }
    };
    return true;
  }
};

bool is_ppm_file(fs::path path) {
  if (path.extension() == ".ppm" && fs::exists(path))
    return true;
  return false;
}

bool is_level_file(string fName) { return true; }

// get control points from ppm images
vector<vec3> get_cps_from_ppm(fs::path path) {
  auto img = read_img_from_ppm(path.string());
  const auto &w = img->w;
  const auto &h = img->h;

  cout << "width:" << w << " height:" << h << endl;
  vector<vec3> cps{};

  vector<int> ints(100);
  std::iota(ints.begin(), ints.end(), 0);

  vector<int> valid_cps_sizes{};
  for (const auto &i : ints) {
    valid_cps_sizes.push_back(3 + 2 * i);
  }

  int SMPL_WIDTH = 15;

  while (1) {
    for (int i = 0; i < w; i++)
      for (int j = 0; j < h; j++) {
        const auto rgb = img->data[i + j * w];
        if (i == 0 || i % SMPL_WIDTH == 0)
          if (rgb.b > 0) {
            cps.push_back(vec3{j, rgb.b, i});
            j = h;
          }
      }

    const auto end_it = valid_cps_sizes.end();
    const auto count = cps.size();

    if (end_it !=
        std::find(valid_cps_sizes.begin(), valid_cps_sizes.end(), count)) {
      break; // break out of while loop
    }
    cps.clear();
    SMPL_WIDTH--;
  }

  cout << cps.size() << " control points created from SMP_WIDTH of "
       << SMPL_WIDTH << endl;
  return cps;
};

int main(int argc, char **argv) {
  if (argc != 2) {
    cout << endl << argv[0];
    cout << ": Must give an argument to run.\n";
    cout << "A) a ppm file OR\n";
    cout << "B) a campath file\n";
    return -1;
  }
  setLogFile("log.txt");

  // read level name
  inFilePath = fs::path{argv[1]};
  cout << "file name entered: " << inFilePath.filename() << endl;

  vector<vec3> cps{};
  // if this is a ppm file, create a campath from the ppm
  if (is_ppm_file(inFilePath)) {
    cout << "valid ppm filename given" << endl;
    cps = get_cps_from_ppm(inFilePath);
  } else if (is_level_file) {
    // if this is a valid level file, load campath and level
    // cps = get_cps_from_campath(fName);
  } else {
    cout << "Not a ppm file or a campath file. Exiting.\n";
    return -1;
  }

  // construct an camPath object inside the engine using cps as the source of
  // the control points

  Example demo{cps};

  // this matches a screen dimension of roughly 1000x700
  if (demo.Construct(500, 350, 2, 2, false, true, false)) {
    demo.Start();
  }

  return 0;
}

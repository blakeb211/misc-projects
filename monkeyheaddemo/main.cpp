#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>  // perspective, lookAt
#include <glm/gtx/string_cast.hpp>       // to_string
// glm start
using mat4 = glm::mat4;
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
constexpr float PI = glm::pi<float>();
constexpr auto deg2rad = glm::radians<float>;
// glm end
#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>
using std::vector, std::cout;
using u64 = std::uint64_t;
#include "vendor/tinyobjloader.h"
// tinyobj start
using Shape = tinyobj::shape_t;
using Material = tinyobj::material_t;
using Attrib = tinyobj::attrib_t;
using ShapeVec = std::vector<Shape>;
using MaterialVec = std::vector<Material>;
// tinyobj end
#include "loadtexture.cpp"
#include "loadmodel.cpp"  // relies on tinyobjloader
#include "loadshader.cpp"

// global start
GLFWwindow* window{nullptr};
int width = 1280;
int height = 720;
double lasttime = glfwGetTime();
u64 frames = 0;

vec3 position        =glm::vec3(0,0,5);
float horizontalAngle=PI; // rot around y toward -Z
float verticalAngle  =0.0f; // rot around x 
// global end
#include "getinput.cpp"

auto initwin() -> int {
  glewExperimental = true;
  if (!glfwInit()) {
    fprintf(stderr, "Failed to init GLFW\n");
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  window = glfwCreateWindow(width, height, "render", NULL, NULL);
  if (window == NULL) {
    fprintf(stderr, "Failed to open GLFW window\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glewExperimental = true;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to init GLEW\n");
    return -1;
  }

  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetCursorPos( window, width/2, height/2 );
  return 0;
}

auto main() -> int {
  printf("c++ version %ld\n", __cplusplus);
  if (initwin() != 0) {
    exit(-1);
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDepthFunc(GL_LESS);
 
  vector<vec3> vert, norm;
  vector<vec2> uv; 

  load_model("res/suzanne.obj", vert, uv, norm);
  //load_model("res/cube.obj", vert, uv, norm);
  cout << "model loaded with:\n" <<
    vert.size() << " vec3 verts\n" << 
    uv.size() << " vec2 uvs\n" << 
    norm.size() << " vec3 norms\n";

  GLuint VertexArray1D;
  glGenVertexArrays(1, &VertexArray1D);
  glBindVertexArray(VertexArray1D);
  printf("vertex array %d created and bound\n", VertexArray1D);

  GLuint vertexbuffer;
  glGenBuffers(1, &vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
  printf("vertex buffer %d created and bound\n", vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, vert.size()*sizeof(glm::vec3),
               vert.data(), GL_STATIC_DRAW);

  GLuint uvbuffer;
  glGenBuffers(1, &uvbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
  printf("uv buffer %d created and bound\n", uvbuffer);
  glBufferData(GL_ARRAY_BUFFER, uv.size()*sizeof(glm::vec2),
               uv.data(), GL_STATIC_DRAW);

  GLuint normalbuffer;
  glGenBuffers(1, &normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, norm.size() * sizeof(glm::vec3), &norm[0], GL_STATIC_DRAW);

  GLuint programID =
      LoadShaders("simplevertex.shader", "simplefragment.shader");

  {  // unused
    mat4 transMat = glm::translate(mat4(1.0), vec3(0.0, 0.0, 0.0));
    mat4 scaleMat = glm::scale(mat4(1.0), vec3(1, 1, 1));
    mat4 rotMat = glm::rotate(mat4(1.0), deg2rad(90.0), vec3(0.0, 0.0, 1.0));
    vec4 myVec(0.0, 1.0, 0.0, 1.0);
    mat4 modelMat = transMat * rotMat * scaleMat;
    vec4 transformedVec = modelMat * myVec;
    std::cout << "transformedVec " << glm::to_string(transformedVec) << "\n";
  }

  // rotation is CCW
  //          +y
  //          |
  //---------+z--------- +x

  // glm prints matrices as columns
  // index them by [column][row]
  // Projection matrix: 45° Field of View, 4:3 ratio, display range: 0.1 unit
  // <-> 100 units
  mat4 Projection = mat4(1.0);

  // Or, for an ortho camera:
  // mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f);
  // // In world coordinates

  // Camera matrix
  mat4 View = mat4(1.0f);

  mat4 Model = mat4(1.0f);
  mat4 mvp = Projection * View *
             Model;  // Remember, matrix multiplication is the other way around

  GLuint MatrixID = glGetUniformLocation(programID, "MVP");
  double targetDt = 1.0 / 60.0;

  
  auto Tex0 = loadBMP_custom("res/MonkeyColor2.bmp");
  // auto Tex0 = loadDDS("res/MonkeyColor.dds");


  do {
    double dt = glfwGetTime() - lasttime;
    if (dt >= targetDt) {
      frames++;
      lasttime += targetDt;
    } else {
      compute_view_projection_from_inputs(dt, View, Projection);
      glfwPollEvents();
      continue;
    }
    compute_view_projection_from_inputs(dt, View, Projection);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // DRAW FIRST THING
    glUseProgram(programID);
    // Model matrix: an identity matrix (model will be at the origin)
    Model = mat4(
        1);  

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0,         // attrib 0, must match layout in shader
                          3,         // size   // num floats per vertex
                          GL_FLOAT,  // type
                          GL_FALSE,  // normalized
                          0,         // stride
                          (void*)0   // array buffer offset
    );

    // 2nd attribute buffer : uv
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);

    glVertexAttribPointer(1,  // attribute. No particular reason for 1, but must
                              // match the layout in the shader.
                          2,  // size
                          GL_FLOAT,  // type
                          GL_FALSE,  // normalized?
                          0,         // stride
                          (void*)0   // array buffer offset
    );

    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
    glVertexAttribPointer(
        2,                                // attribute
        3,                                // size
        GL_FLOAT,                         // type
        GL_FALSE,                         // normalized?
        0,                                // stride
        (void*)0                          // array buffer offset
    );

    auto circ_pos = [](float rad, vec3 init_pos, float init_angle, u64 frames) -> vec3 {
      float start_theta = init_angle; 
      //float dtheta = 0.25/2.f*deg2rad((108-rad)/7*(frames % 2880));
      float dtheta = 0.25/2.f*deg2rad((std::abs(76-rad))/7*(frames % 2880));
      auto dx = rad*glm::cos(start_theta + dtheta);
      auto dy = rad*glm::sin(start_theta + dtheta);
      return vec3(dx,dy,init_pos.z);
    };

    float z = -220.0;
    int draw_calls = 0;
    for (int radius = 75; radius < 101; radius +=8)
    for (float t = 0; t < 2*PI; t += 2*PI/40) {
      float init_radius (radius); 
      vec3 init_pos ( init_radius*glm::cos(t), init_radius*glm::sin(t), z );
      mat4 rotMat = glm::rotate(mat4(1.0), deg2rad( (frames % 1440) ) - radius/100, vec3(1, -1, 1));
      double scaleFactor = glm::pow(99u, std::log10((double)radius)) / 2200.f;  
      mat4 scaleMat = glm::scale(mat4(1.0),vec3(scaleFactor,scaleFactor,scaleFactor));
      Model = glm::translate( mat4(1.0f),  circ_pos(radius, init_pos, t, frames) ) * rotMat * scaleMat;
      // Our ModelViewProjection: multiplication of our 3 matrices
      mvp = Projection * View *
            Model;  // Remember, matrix multiplication is the other way around
      glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

      glDrawArrays(GL_TRIANGLES, 0,
                   vert.size()*3 );  // num floats
      draw_calls++;

    }
    if (frames % 1000 == 0) {
      cout << "Draw calls: " << draw_calls << std::endl;
    }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    // DRAW SECOND THING

    glfwSwapBuffers(window);
    glfwPollEvents();
  } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0);

  return 0;
}

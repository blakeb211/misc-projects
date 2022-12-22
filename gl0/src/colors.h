#pragma once
#include "glm.h"
#include <vector>
// colors
namespace col {
inline glm::vec3 red { 1.0, 0.0, 0.0 };
inline glm::vec3 green { 0.0, 1.0, 0.0 };
inline glm::vec3 blue { 0.0, 0.0, 1.0 };
inline glm::vec3 purple { 1.0, 0.0, 1.0 };
inline glm::vec3 yellow { 0.0, 1.0, 1.0 };
inline glm::vec3 darkgrey = { 0.4, 0.4, 0.4 };
inline glm::vec3 cyan = { 0.0, 1.0, 1.0 };
inline glm::vec3 magenta = { 1.0, 0.0, 1.0 };
inline glm::vec3 white = { 1.0, 1.0, 1.0 };
inline glm::vec3 black = { 0.0, 0.0, 0.0 };
inline glm::vec3 darkred = { 0.5, 0.0, 0.0 };
inline glm::vec3 darkgreen = { 0.0, 0.5, 0.0 };
inline glm::vec3 darkblue = { 0.0, 0.0, 0.5 };
inline glm::vec3 darkcyan = { 0.0, 0.5, 0.5 };
inline glm::vec3 darkmagenta = { 0.5, 0.0, 0.5 };
inline glm::vec3 darkyellow = { 0.5, 0.5, 0.0 };
inline glm::vec3 lightgrey = { 0.8, 0.8, 0.8 };
inline std::vector<glm::vec3> list { red, green, blue, purple, yellow,
    darkgrey, cyan, magenta, white,
    black, darkred, darkgreen, darkblue, darkcyan,
    darkmagenta, darkyellow, lightgrey };
};

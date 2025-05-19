
void compute_view_projection_from_inputs(float deltaTime, mat4& ViewMatrix, mat4&ProjectionMatrix)
{
  float initialFoV     =55.0f;

  float speed          =3.0f; // 3 units / sec
  float mouseSpeed     =0.019f;                            

  // first recompute position to FoV from inputs, then
  // compute view nad projection matrices from those.
  double xpos,ypos;
  glfwGetCursorPos( window, &xpos, &ypos );
  glfwSetCursorPos( window, width/2, height/2 );

  horizontalAngle += mouseSpeed * deltaTime * float(width/2 - xpos);
  verticalAngle += mouseSpeed * deltaTime * float(height/2 - ypos);

  // Direction : Spherical coordinates to Cartesian coordinates conversion
  // dx, dy, dz

  vec3 direction(
      cos(verticalAngle) * sin(horizontalAngle),
      sin(verticalAngle),
      cos(verticalAngle) * cos(horizontalAngle)
  );
  
  // Right vector
  vec3 right = glm::vec3(
      sin(horizontalAngle - 3.14f/2.0f),
      0,
      cos(horizontalAngle - 3.14f/2.0f)
  );

  // Up vector : perpendicular to the front facing direction vector and Right
  vec3 up = glm::cross( right, direction );

  // Move forward
  if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
      position += direction * deltaTime * speed;
  }
  // Move backward
  if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
      position -= direction * deltaTime * speed;
  }
  // Strafe right
  if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
      position += right * deltaTime * speed;
  }
  // Strafe left
  if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
      position -= right * deltaTime * speed;
  }

  float FoV = initialFoV; //- 5 * glfwGetMouseWheel();

  // Projection matrix : 45&deg; Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 500.0f);
  // mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f);
  //                              left   right  bot   top   zNear zFar

  // Camera matrix
  ViewMatrix       = glm::lookAt(
      position,           // Camera is here
      position+direction, // and looks here : at the same position, plus "direction"
      up                  // Head is up (set to 0,-1,0 to look upside-down)
  );
}

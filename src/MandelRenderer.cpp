#include "MandelRenderer.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>


  
GLuint MandelRenderer::VertexArrayUnitPlane;
GLuint MandelRenderer::VertexBufferUnitPlane;

void MandelRenderer::init()
{
  glGenVertexArrays(1, &VertexArrayUnitPlane);
  glBindVertexArray(VertexArrayUnitPlane);

  //generate vertex buffer to hand off to OGL
  glGenBuffers(1, &VertexBufferUnitPlane);
  //set the current state to focus on our vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, VertexBufferUnitPlane);

  GLfloat *ver = new GLfloat[6 * 3];
  // front
  int verc = 0;

  ver[verc++] = 0.0, ver[verc++] = 0.0, ver[verc++] = 0.0;
  ver[verc++] = 1.0, ver[verc++] = 1.0, ver[verc++] = 0.0;
  ver[verc++] = 0.0, ver[verc++] = 1.0, ver[verc++] = 0.0;
  ver[verc++] = 0.0, ver[verc++] = 0.0, ver[verc++] = 0.0;
  ver[verc++] = 1.0, ver[verc++] = 0.0, ver[verc++] = 0.0;
  ver[verc++] = 1.0, ver[verc++] = 1.0, ver[verc++] = 0.0;

  //actually memcopy the data - only do this once
  glBufferData(GL_ARRAY_BUFFER, 6 * 3 * sizeof(float), ver, GL_STATIC_DRAW);
  //we need to set up the vertex array
  glEnableVertexAttribArray(0);
  //key function to get up how many elements to pull out at a time (3)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void MandelRenderer::render(std::shared_ptr<Program> prog, float zoomLevel, glm::vec2 size, bool exhaust)
{
  // this is probably real inefficient oops
  RenderData d2 = data;
  d2.zoom_level = zoomLevel;
  d2.exhaust = exhaust;
  render_internal(prog, size, d2);
}

void MandelRenderer::render_internal(std::shared_ptr<Program> prog, glm::vec2 size, RenderData &dat)
{
  glViewport(0, 0, size.x, size.y);
  glClearColor(0.f, 0.f, 0.f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  
  // We might be able to do a uniform buffer struct, but that limits
  // free edits/experimentation to the data passed for now, and it
  // doesn't seem to be a bottleneck right now.
  glUniform2f(prog->getUniform("resolution"), static_cast<float>(size.x), static_cast<float>(size.y));
  glUniform1i(prog->getUniform("intersectStepCount"), dat.intersect_step_count);
  glUniform3fv(prog->getUniform("clearColor"), 1, (float*)&dat.clear_color);
  glUniform3fv(prog->getUniform("yColor"), 1, (float*)&dat.y_color);
  glUniform3fv(prog->getUniform("zColor"), 1, (float*)&dat.z_color);
  glUniform3fv(prog->getUniform("wColor"), 1, (float*)&dat.w_color);
  glUniform3fv(prog->getUniform("diffc1"), 1, (float*)&dat.diff1);
  glUniform3fv(prog->getUniform("diffc2"), 1, (float*)&dat.diff2);
  glUniform3fv(prog->getUniform("diffc3"), 1, (float*)&dat.diff3);
  glUniform1f(prog->getUniform("viewscale"), dat.zoom_level);
  glUniform1i(prog->getUniform("modulo"), dat.modulo);
  glUniform1i(prog->getUniform("iTest"), dat.iTest);
  glUniform1f(prog->getUniform("juliaFactor"), dat.juliaFactor);
  glUniform3fv(prog->getUniform("juliaPoint"), 1, (float*)&dat.juliaPoint);
  glUniform1i(prog->getUniform("mapIterCount"), dat.map_iter_count);
  glUniform1i(prog->getUniform("exhaust"), dat.exhaust);
  
  glBindVertexArray(VertexArrayUnitPlane);
  glDrawArrays(GL_TRIANGLES, 0, 6);
}

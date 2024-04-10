#ifndef RECTS_HPP
#define RECTS_HPP
#include "celerityui.h"
#include "vao.hpp"
#include "shader.hpp"
#include <vector>
#define MAX_BATCH_ELEMENTS 1024
class RectRenderer {
  std::vector<Vao> vaos;
	ShaderProgram program;
  void recalculate_indexing();
public:
  // batches of rectangles, each batch has at most MAX_BATCH_ELEMENTS elements
  std::vector<std::vector<CelRect*>> rectangles;
	RectRenderer();
	void render_opaque();
	void render_transparent(int to_index);
};
#endif

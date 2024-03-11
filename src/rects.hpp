#ifndef RECTS_HPP
#define RECTS_HPP
#include "celerityui.h"
#include "vao.hpp"
#include "shader.hpp"
#include <vector>
class RectRenderer {
	Vao rectangle;
	ShaderProgram program;
public:
	std::vector<CelRect*> rectangles;
	RectRenderer();
	void renderOpaque();
	void renderTransparent(int to_index);
};
#endif

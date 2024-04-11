#include "rects.hpp"
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <iostream>
#include <unordered_map>
#include "src/celerityui.h"
#include "src/internal.hpp"
std::unordered_map<CelWin *, RectRenderer *> renderer;
CelRect *cel_create_rectangle(CelWin *win, float x, float y, float width,
							  float height, CelPaint color) {
	{
		using namespace std;
		const lock_guard<mutex> lk(Internal::gl_lock);
		glfwMakeContextCurrent(win->window);
		if (!renderer.contains(win))
			renderer.insert({win, new RectRenderer()});
		glfwMakeContextCurrent(nullptr);
	}
	CelRect *rect = new CelRect();
	rect->color = color;
	rect->x = x;
	rect->y = y;
	rect->width = width;
	rect->height = height;
	if (renderer[win]->rectangles.empty() ||
		renderer[win]->rectangles.back().size() >= MAX_BATCH_ELEMENTS)
		renderer[win]->rectangles.push_back({});
	renderer[win]->rectangles.back().push_back(rect);
	return rect;
}
void cel_delete_rectangle(CelWin *win, CelRect *rect) {
	using namespace std;
	vector<vector<CelRect *>> &batches = renderer[win]->rectangles;
	for (vector<CelRect *> &rects : batches) {
		for (auto it = rects.begin(); it != rects.end(); it++) {
			if (*it == rect) {
				rects.erase(it);
				break;
			}
		}
	}
	delete rect;
}

void cel_render_rectangles(CelWin *win) {
	if (renderer.contains(win))
		renderer[win]->render_opaque();
}
const std::string rect_vertex = R"(
#version 400
#define MAX_BATCH_ELEMENTS 1024
layout (location = 0) in vec2 pos;
uniform vec2 positions[MAX_BATCH_ELEMENTS];
uniform float rotations[MAX_BATCH_ELEMENTS];
uniform vec2 scales[MAX_BATCH_ELEMENTS];
uniform vec4 colors[MAX_BATCH_ELEMENTS];
out vec4 out_color;
void main() {
  // scale
  vec2 final = pos * scales[gl_InstanceID];
  // rotate
  float cosr = cos(rotations[gl_InstanceID]);
  float sinr = sin(rotations[gl_InstanceID]);
  vec2 centered = final - vec2(1, -1) * scales[gl_InstanceID] / 2;
  final = vec2(centered.x * cosr - centered.y * sinr, centered.x * sinr + centered.y * cosr);
  final += vec2(1, -1) * scales[gl_InstanceID] / 2;
  // translate
  final += positions[gl_InstanceID];
  // pass through
  out_color = colors[gl_InstanceID];
  gl_Position = vec4(final, 0.0, 1.0);
}
)";
const std::string rect_frag = R"(
#version 400
in vec4 out_color;
out vec4 final_color;
void main() {
  final_color = out_color;
}
)";
const static float vertices[] = {0, 0, 0, -1, 1, 0, 1, -1};
const static unsigned int indices[] = {0, 1, 2, 2, 1, 3};
RectRenderer::RectRenderer() : program(rect_vertex, rect_frag) {}

void RectRenderer::recalculate_indexing() {
	while (vaos.size() < rectangles.size()) {
		vaos.emplace_back();
		vaos.back().add_index_buffer(indices, 6);
		vaos.back().add_vertex_buffer(2, vertices, 8);
	}
	for (int i = 0; i < vaos.size(); i++) {
		vaos[i].set_instance_count(rectangles[i].size());
	}
}
void RectRenderer::render_opaque() {
	using namespace glm;
	program.start();
	recalculate_indexing();
	for (int i = 0; i < vaos.size(); i++) {
		vaos[i].bind();
    vaos[i].set_instance_count(rectangles[i].size());
		for (int j = 0; j < rectangles[i].size(); j++) {
			CelRect *rect = rectangles[i][j];
			program.load("positions[" + std::to_string(j) + "]",
						 vec2(rect->x, rect->y));
			program.load("rotations[" + std::to_string(j) + "]",
						 rect->rotation);
			program.load("scales[" + std::to_string(j) + "]",
						 vec2(rect->width, rect->height));
			program.load("colors[" + std::to_string(j) + "]",
						 vec4(rect->color.color.r, rect->color.color.g,
							  rect->color.color.b, rect->color.color.a));
		}
		vaos[i].draw();
	}
	program.stop();
}
void RectRenderer::render_transparent(int to_index) {}

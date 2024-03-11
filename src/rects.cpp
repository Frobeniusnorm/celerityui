#include "rects.hpp"
#include <GLFW/glfw3.h>
#include <unordered_map>
#include "src/celerityui.h"
std::unordered_map<CelWin *, RectRenderer> renderer;
CelRect *cel_create_rectangle(CelWin *win, float x, float y, float width,
							  float height, CelPaint color) {
	glfwMakeContextCurrent(win->window);
	if (!renderer.contains(win))
		renderer.emplace(win, RectRenderer());
	CelRect *rect = new CelRect();
	rect->color = color;
	rect->x = x;
	rect->y = y;
	rect->width = width;
	rect->height = height;
	renderer[win].rectangles.push_back(rect);
	return rect;
}
void cel_delete_rectangle(CelWin * win, CelRect * rect) {
	using namespace std;
	vector<CelRect*>& rects = renderer[win].rectangles;
	for (auto it = rects.begin(); it != rects.end(); it++) {
		if (*it == rect) {
			rects.erase(it);
			break;
		}
	}
	delete rect;
}

/* CelerityUI - A fast, portable, OpenGL based GUI Framework
 * Copyright (C) 2024 David Schwarzbeck
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. */

#include <unistd.h>
#include <cstdlib>
#include <thread>
#include <vector>
#include "celerityui.h"
#include <GLFW/glfw3.h>

int main() {
	CelWin *a = cel_create_window("Test Window Creation 1", 600, 400);
	using namespace std;
	vector<CelRect *> rects(100);
	vector<float> rotation_speed(100);
	for (int i = 0; i < 100; i++) {
		float x = (rand() % 1000) / 500.0 - 1;
		float y = (rand() % 1000) / 500.0 - 1;
		float r = (rand() % 1000) / 1000.0;
		float g = (rand() % 1000) / 1000.0;
		float b = (rand() % 1000) / 1000.0;
		rects[i] = cel_create_rectangle(a, x, y, 0.5, 0.5, {{r, g, b, 1}, 0});
		rotation_speed[i] = (rand() % 1000) / 1000.0;
	}
	while (!glfwWindowShouldClose(a->window)) {
		for (int i = 0; i < 100; i++) {
			rects[i]->rotation += rotation_speed[i] * 0.01;
		}
		this_thread::sleep_for(chrono::milliseconds(14));
	}
	cel_wait_for_window(a);
}

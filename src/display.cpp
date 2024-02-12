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

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <ostream>
#include <thread>
#include <unordered_map>
#include "celerityui.h"
using namespace std;
static unordered_map<GLFWwindow *, CelWin *> assoc_wins;
static unordered_map<CelWin *, thread *> assoc_threads;
static void error_callback(int, const char *error) {
	std::cout << error << std::endl;
}
static void window_size_callback(GLFWwindow *win, int width, int height) {}
static void window_pos_callback(GLFWwindow *win, int x, int y) {}
static void window_routine(CelWin *win) {
	GLFWwindow *window =
		glfwCreateWindow(win->width, win->height, win->name, nullptr, nullptr);
	win->window = window;
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetWindowPosCallback(window, window_pos_callback);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	int glew_stat = glewInit();
	if (glew_stat != GLEW_OK) {
		cout << "Could not init glew " << glew_stat << endl;
		return;
	}
	assoc_wins.insert({window, win});
	glClearColor(1, 1, 1, 1);
	glViewport(0, 0, win->width, win->height);
	while (!glfwWindowShouldClose(window) && win->closed == 0) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		std::cout << "update" << std::endl;
		glfwWaitEvents();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

CelWin *cel_create_window(const char *title, int width, int height) {
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		cout << "Could not init glfw" << endl;
		return nullptr;
	}
	CelWin *res = new CelWin();
	res->name = title;
	res->width = width;
	res->height = height;
	res->closed = 0;
	assoc_threads.insert({res, new thread(window_routine, res)});
	return res;
}
void cel_wait_for_window(CelWin *win) {
	assoc_threads[win]->join();
}
void cel_destroy_window(CelWin *win) {
	assoc_wins.erase(win->window);
	win->closed = 1;
	assoc_threads[win]->join();
	delete assoc_threads[win];
	assoc_threads.erase(win);
	glfwDestroyWindow(win->window);
	delete win;
}

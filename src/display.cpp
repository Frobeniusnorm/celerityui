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
#include <vector>
#include <semaphore>
#include "celerityui.h"

#include "internal.hpp"

using namespace std;
static bool glfw_initialized = false;
static volatile atomic<bool> glew_initialized = false;
static unordered_map<GLFWwindow *, CelWin *> assoc_wins;
static unordered_map<CelWin *, thread *> assoc_threads;
static unordered_map<CelWin *, binary_semaphore*> wait_for_create;
static void error_callback(int, const char *error) {
	std::cout << error << std::endl;
}
static unordered_map<CelWin *, vector<void (*)(CelWin *)>> resize_callbacks;
static void window_size_callback(GLFWwindow *win, int width, int height) {
	CelWin *cw = assoc_wins[win];
	cw->width = width;
	cw->height = height;
	for (const auto cb : resize_callbacks[cw])
		cb(cw);
}
void cel_add_resize_callback(CelWin *win, void (*cb)(CelWin *)) {
	auto &callbacks = resize_callbacks;
	if (!callbacks.contains(win))
		callbacks.insert({win, {cb}});
	else
		callbacks[win].push_back(cb);
}
void cel_remove_resize_callback(CelWin *win, void (*cb)(CelWin *)) {
	auto &callbacks = resize_callbacks;
	for (auto oc = callbacks[win].begin(); oc != callbacks[win].end(); oc++) {
		if (*oc == cb) {
			callbacks[win].erase(oc);
			return;
		}
	}
}
static unordered_map<CelWin *, vector<void (*)(CelWin *)>> position_callbacks;
static void window_pos_callback(GLFWwindow *win, int x, int y) {
	CelWin *cw = assoc_wins[win];
	cw->x = x;
	cw->y = y;
	for (const auto cb : position_callbacks[cw])
		cb(cw);
}
void cel_add_position_callback(CelWin *win, void (*cb)(CelWin *)) {
	auto &callbacks = position_callbacks;
	if (!callbacks.contains(win))
		callbacks.insert({win, {cb}});
	else
		callbacks[win].push_back(cb);
}
void cel_remove_position_callback(CelWin *win, void (*cb)(CelWin *)) {
	auto &callbacks = resize_callbacks;
	for (auto oc = callbacks[win].begin(); oc != callbacks[win].end(); oc++) {
		if (*oc == cb) {
			callbacks[win].erase(oc);
			return;
		}
	}
}
static unordered_map<CelWin *, vector<void (*)(CelWin *, int focus)>>
	focus_callbacks;
static void window_focus_callback(GLFWwindow *win, int focus) {
	CelWin *cw = assoc_wins[win];
	for (const auto cb : focus_callbacks[cw])
		cb(cw, focus);
}
void cel_add_focus_callback(CelWin *win, void (*cb)(CelWin *, int focus)) {
	auto &callbacks = focus_callbacks;
	if (!callbacks.contains(win))
		callbacks.insert({win, {cb}});
	else
		callbacks[win].push_back(cb);
}
void cel_remove_focus_callback(CelWin *win, void (*cb)(CelWin *, int focus)) {
	auto &callbacks = focus_callbacks;
	for (auto oc = callbacks[win].begin(); oc != callbacks[win].end(); oc++) {
		if (*oc == cb) {
			callbacks[win].erase(oc);
			return;
		}
	}
}
static unordered_map<CelWin *, vector<void (*)(CelWin *, double x, double y)>>
	cursor_callbacks;
static void window_cursor_callback(GLFWwindow *win, double x, double y) {
	CelWin *cw = assoc_wins[win];
	for (const auto cb : cursor_callbacks[cw])
		cb(cw, x, y);
}
void cel_add_cursor_callback(CelWin *win,
							 void (*cb)(CelWin *, double x, double y)) {
	auto &callbacks = cursor_callbacks;
	if (!callbacks.contains(win))
		callbacks.insert({win, {cb}});
	else
		callbacks[win].push_back(cb);
}
void cel_remove_cursor_callback(CelWin *win,
								void (*cb)(CelWin *, double x, double y)) {
	auto &callbacks = cursor_callbacks;
	for (auto oc = callbacks[win].begin(); oc != callbacks[win].end(); oc++) {
		if (*oc == cb) {
			callbacks[win].erase(oc);
			return;
		}
	}
}
static unordered_map<CelWin *, vector<void (*)(CelWin *, int, int, int)>>
	mouse_callbacks;
static void window_mouse_callback(GLFWwindow *win, int button, int action,
								  int mods) {
	CelWin *cw = assoc_wins[win];
	for (const auto cb : mouse_callbacks[cw])
		cb(cw, button, action, mods);
}
void cel_add_mouse_callback(CelWin *win, void (*cb)(CelWin *, int, int, int)) {
	auto &callbacks = mouse_callbacks;
	if (!callbacks.contains(win))
		callbacks.insert({win, {cb}});
	else
		callbacks[win].push_back(cb);
}
void cel_remove_mouse_callback(CelWin *win,
							   void (*cb)(CelWin *, int, int, int)) {
	auto &callbacks = mouse_callbacks;
	for (auto oc = callbacks[win].begin(); oc != callbacks[win].end(); oc++) {
		if (*oc == cb) {
			callbacks[win].erase(oc);
			return;
		}
	}
}
static unordered_map<CelWin *, vector<void (*)(CelWin *, double, double)>>
	scroll_callbacks;
static void window_scroll_callback(GLFWwindow *win, double x, double y) {
	CelWin *cw = assoc_wins[win];
	for (const auto cb : scroll_callbacks[cw])
		cb(cw, x, y);
}
void cel_add_scroll_callback(CelWin *win,
							 void (*cb)(CelWin *, double, double)) {
	auto &callbacks = scroll_callbacks;
	if (!callbacks.contains(win))
		callbacks.insert({win, {cb}});
	else
		callbacks[win].push_back(cb);
}
void cel_remove_scroll_callback(CelWin *win,
								void (*cb)(CelWin *, double, double)) {
	auto &callbacks = scroll_callbacks;
	for (auto oc = callbacks[win].begin(); oc != callbacks[win].end(); oc++) {
		if (*oc == cb) {
			callbacks[win].erase(oc);
			return;
		}
	}
}
static void window_routine(CelWin *win) {
	GLFWwindow *window =
		glfwCreateWindow(win->width, win->height, win->name, nullptr, nullptr);
	win->window = window;
	assoc_wins.insert({window, win});
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetWindowPosCallback(window, window_pos_callback);
	glfwSetWindowFocusCallback(window, window_focus_callback);
	glfwSetCursorPosCallback(window, window_cursor_callback);
	glfwSetMouseButtonCallback(window, window_mouse_callback);
	glfwSetScrollCallback(window, window_scroll_callback);
	{
		const lock_guard<mutex> lk(Internal::gl_lock);
		glfwMakeContextCurrent(window);
		if (!glew_initialized.exchange(true)) {
			int glew_stat = glewInit();
			if (glew_stat != GLEW_OK) {
        std::cerr << "GLEW error!" << std::endl;
				return;
			}
      std::cout << "glew init" << std::endl;
		}
		glClearColor(1, 1, 1, 1);
		glViewport(0, 0, win->width, win->height);
		glfwMakeContextCurrent(nullptr);
	}
  wait_for_create[win]->release();
	int oldwidth = win->width;
	int oldheight = win->height;
	while (!glfwWindowShouldClose(window)) {
		bool dont_sleep = false;
		{
			const lock_guard<mutex> lk(Internal::gl_lock);
			if (glfwGetCurrentContext() != window)
				glfwMakeContextCurrent(window);
			if (win->width != oldwidth || win->height != oldheight) {
				oldwidth = win->width;
				oldheight = win->height;
				glViewport(0, 0, oldwidth, oldheight);
			}
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			cel_render_rectangles(win);
			glfwSwapBuffers(window);
			glfwMakeContextCurrent(nullptr);
		}
		if (!dont_sleep) {
			glfwWaitEvents();
			glfwPollEvents();
		}
	}
	glfwHideWindow(window);
	glfwDestroyWindow(window);
}

CelWin *cel_create_window(const char *title, int width, int height) {
	glfwSetErrorCallback(error_callback);
	if (!glfw_initialized && !glfwInit()) {
		return nullptr;
	}
	glfw_initialized = true;
	CelWin *res = new CelWin();
	res->name = title;
	res->width = width;
	res->height = height;
  wait_for_create.insert({res, new std::binary_semaphore(0)});
	assoc_threads.insert({res, new thread(window_routine, res)});
  wait_for_create[res]->acquire();
  delete wait_for_create[res];
  wait_for_create.erase(res);
	return res;
}
void cel_wait_for_window(CelWin *win) {
	assoc_threads[win]->join();
}
void cel_destroy_window(CelWin *win) {
	glfwSetWindowShouldClose(win->window, 1);
	glfwPostEmptyEvent();
	assoc_threads[win]->join();
	assoc_wins.erase(win->window);
	delete assoc_threads[win];
	assoc_threads.erase(win);
	scroll_callbacks.erase(win);
	mouse_callbacks.erase(win);
	cursor_callbacks.erase(win);
	resize_callbacks.erase(win);
	position_callbacks.erase(win);
	focus_callbacks.erase(win);
	delete win;
}
void cel_resize_window(CelWin *win, int x, int y) {
	glfwSetWindowSize(win->window, x, y);
}
void cel_move_window(CelWin *win, int x, int y) {
	glfwSetWindowPos(win->window, x, y);
}
void cel_rename_window(CelWin *win, const char *title) {
	glfwSetWindowTitle(win->window, title);
}

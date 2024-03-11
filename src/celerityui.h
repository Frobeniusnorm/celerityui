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

#ifndef CELERITYUI
#define CELERITYUI
#include <GL/glew.h>
#include <GLFW/glfw3.h>
/* Color Definitions */
typedef struct CelWin {
	const char *name;
	GLFWwindow *window;
	int x, y;
	int width;
	int height;
} CelWin;
typedef struct CelColorRGBA {
	float r;
	float g;
	float b;
	float a;
} CelColorRGBA;
typedef struct CelColorGradient {
	CelColorRGBA a;
	CelColorRGBA b;
	float center;
	float angle;
} CelColorGradient;
typedef union CelColor {
	CelColorRGBA color;
	CelColorGradient gradient;
} CelColor;
typedef struct CelPaint {
	CelColor color;
	float blur;
} CelPaint;
typedef struct CelRect {
	CelPaint color;
	float x, y, width, height;
	CelWin *origin;
} CelRect;
/* Window Management Functions */
CelWin *cel_create_window(const char *title, int width, int height);
void cel_destroy_window(CelWin *);
void cel_wait_for_window(CelWin *);
void cel_resize_window(CelWin *, int, int);
void cel_move_window(CelWin *, int, int);
void cel_rename_window(CelWin *, const char *);
void cel_add_resize_callback(CelWin *, void (*)(CelWin *));
void cel_remove_resize_callback(CelWin *, void (*)(CelWin *));
void cel_add_position_callback(CelWin *, void (*)(CelWin *));
void cel_remove_position_callback(CelWin *, void (*)(CelWin *));
void cel_add_focus_callback(CelWin *, void (*)(CelWin *, int focus));
void cel_remove_focus_callback(CelWin *, void (*)(CelWin *, int focus));
void cel_add_cursor_callback(CelWin *, void (*)(CelWin *, double x, double y));
void cel_remove_cursor_callback(CelWin *,
								void (*)(CelWin *, double x, double y));
void cel_add_mouse_callback(CelWin *, void (*)(CelWin *, int, int, int));
void cel_remove_mouse_callback(CelWin *, void (*)(CelWin *, int, int, int));
void cel_add_scroll_callback(CelWin *, void (*)(CelWin *, double, double));
void cel_remove_scroll_callback(CelWin *, void (*)(CelWin *, double, double));
/* Rectangles */
CelRect *cel_create_rectangle(CelWin *, float x, float y, float width,
							  float height, CelPaint color);
void cel_delete_rectangle(CelWin *, CelRect *);
#endif

/*
 * Main Loop. Creates and OpenGL context and a window to render into, also 
 * handles input. Needs to be linked against SDL2, OpenGL and GLEW.  
 * Copyright (C) 2014 Arno in Wolde Luebke 
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MAINLOOP_H
#define MAINLOOP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "Keycodes.h"

struct
{
    float windowWidth;
    float windowHeight;
}
FFMainLoopContext;

/*
** Creates a main loop. Creates a window and an OpenGL context (currently 3.2)
** also initializes GLEW.
**
** The function takes a filename to a config file. The config file takes the 
** json format and looks for example like this:
**
** 		{
** 			"windowTitle" : "Hello, World",
**          "windowWidth" : 800,
**          "windowHeight" : 600,
**  		"windowPosX" : 100,
**          "windowPosY" : 100
**		}
**
*/  
void FFMainLoopCreate(const char* configFileName);
void FFMainLoopRun();
void FFMainLoopDestroy();

void FFMainLoopSetKeyDownFunc(void (*func)(int));
void FFMainLoopSetKeyUpFunc(void (*func)(int));
void FFMainLoopSetMouseMoveFunc(void (*func)(int, int, int, int));
void FFMainLoopSetMouseLeftButtonDownFunc(void (*func)(int, int));
void FFMainLoopSetMouseLeftButtonUpFunc(void (*func)(int, int));
void FFMainLoopSetMouseRightButtonDownFunc(void (*func)(int, int));
void FFMainLoopSetMouseRightButtonUpFunc(void (*func)(int, int));

#ifdef __cplusplus
}
#endif

#endif /* end of include guard: MAINLOOP_H */

#include "MainLoop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "../External/parson.h"

#define ERR_MSG(X) printf("In file: %s line: %d\n\t%s\n", __FILE__, __LINE__, X);

static char windowTitle[1024]; 
static int windowWidth = 800;
static int windowHeight = 600;
static int windowPosX = 0;
static int windowPosY = 0;

/* sdl stuff */
static SDL_Window* window;
static SDL_GLContext context;
static float elapsed;

extern void FFInit();
extern void FFFinalize();
extern int FFUpdate(float dt);

/* event stuff */
static void DefaultKeyDownFunc(int kcode)
{

}

static void DefaultKeyUpFunc(int kcode)
{

}

static void DefaultMouseMoveFunc(int x, int y, int relx, int rely)
{

}

static void DefaultMouseLeftButtonDown(int x, int y)
{

}

static void DefaultMouseRightButtonDown(int x, int y)
{

}

static void DefaultMouseLeftButtonUp(int x, int y)
{

}

static void DefaultMouseRightButtonUp(int x, int y)
{

}

static void (*keyDownFunc)(int) = DefaultKeyDownFunc;
static void (*keyUpFunc)(int) = DefaultKeyUpFunc;
static void (*mouseMoveFunc)(int, int, int, int) = DefaultMouseMoveFunc;
static void (*mouseLeftButtonDownFunc)(int, int) = DefaultMouseLeftButtonDown;
static void (*mouseRightButtonDownFunc)(int, int) = DefaultMouseRightButtonDown;
static void (*mouseLeftButtonUpFunc)(int, int) = DefaultMouseLeftButtonUp;
static void (*mouseRightButtonUpFunc)(int, int) = DefaultMouseRightButtonUp;

/*
** sets the global variables according to the config file
*/ 
static int ProcessConfigFile(const char* configFileName)
{
	JSON_Value* root;
	JSON_Object* object;
	
	root = json_parse_file_with_comments(configFileName);

	if (!root) 
	{
	   	return 0; 
	}

	object = json_value_get_object(root); 

	if (!object) 
	{
	    return 0;
	}

	strcpy(windowTitle, json_object_get_string(object, "windowTitle"));
	windowWidth = json_object_get_number(object, "windowWidth");	
	windowHeight = json_object_get_number(object, "windowHeight"); 
	windowPosX = json_object_get_number(object, "windowPosX"); 
	windowPosY = json_object_get_number(object, "windowPosY"); 
    FFMainLoopContext.windowWidth = windowWidth;
    FFMainLoopContext.windowHeight = windowHeight;

	json_value_free(root);

	return 1;
}

static void HandleEvent(SDL_Event* Event)
{
    switch(Event->type)
    {
 
        case SDL_KEYDOWN: 

            if (Event->key.repeat == 1)
            {
                return;
            }

            (*keyDownFunc)(Event->key.keysym.sym);
            break;
        
        case SDL_KEYUP: 
            (*keyUpFunc)(Event->key.keysym.sym);
            break;
            
        case SDL_MOUSEMOTION:
        {
            (*mouseMoveFunc)(
                Event->motion.x,
                Event->motion.y,
                Event->motion.xrel,
                Event->motion.yrel
            );
            
            break;
        }
        
        case SDL_MOUSEBUTTONDOWN:
        {
            switch(Event->button.button)
            {
                case SDL_BUTTON_LEFT:
                {
                    (*mouseLeftButtonDownFunc)(Event->button.x,Event->button.y);
                    break;
                }
                case SDL_BUTTON_RIGHT:
                {
                    (*mouseRightButtonDownFunc)(Event->button.x,Event->button.y);
                    break;
                }
            }
            break;
        }
 
        case SDL_MOUSEBUTTONUP:
        {
            switch(Event->button.button)
             {
                case SDL_BUTTON_LEFT:
                {
                    (*mouseLeftButtonUpFunc)(Event->button.x,Event->button.y);
                    break;
                }
                case SDL_BUTTON_RIGHT:
                {
                    (*mouseRightButtonUpFunc)(Event->button.x,Event->button.y);
                    break;
                }
            }
            break;
        }
        
        default:
            break;
    }
}

void FFMainLoopCreate(const char* configFileName)
{
	if (!ProcessConfigFile(configFileName))
    {
        puts("Warning: parsing the config file failed. Using default configuration.");
    }

	/* init SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        ERR_MSG("Could not initialize SDL2");
        exit(0);
    }
    
    /* Set OpenGL context attributes */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK,
        SDL_GL_CONTEXT_PROFILE_CORE
    );
    
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 24);
    
    /* create a new window */
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    
/*    if (fullscreen)
    {
        flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_FULLSCREEN;
    } */
    
    window = SDL_CreateWindow(
           	windowTitle, 
            windowPosX,
            windowPosY,
            windowWidth,
            windowHeight,
            flags
        );
    
    if (window == NULL)
    {
        SDL_Quit();
        ERR_MSG("Could not create SDL window");
		exit(0);
    }

    /* create an OpenGL context */
    context = SDL_GL_CreateContext(window);
    
    if (context == NULL)
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        ERR_MSG("Could not create SDL_GL context");
        exit(0);
    }
    
	/* init glew */
	glewExperimental = GL_TRUE; 

    if (GLEW_OK != glewInit())
    {
        SDL_DestroyWindow(window);
        SDL_Quit();
        ERR_MSG("Failed Initializing Glew");
        exit(0);
    }

    glGetError();
}

void FFMainLoopDestroy()
{
	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
    SDL_Quit();
}

void FFMainLoopRun()
{
    SDL_Event event; 
    int done = 0; 

    Uint32 currTime = SDL_GetTicks();

    FFInit();

    /* event handling */
    while (!done) 
    {
        /* compute the time */
        elapsed = (float)(SDL_GetTicks() - currTime);
        currTime = SDL_GetTicks();
 
        /* handle events */
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT || event.type == SDL_QUIT)
            {
                done = 1;
            }
            
            HandleEvent(&event);
        }
        
        if (FFUpdate(elapsed))
        {
            done = 1;
        }
             
        SDL_GL_SwapWindow(window);
    }
    
    FFFinalize();
}

void FFMainLoopSetKeyDownFunc(void (*func)(int))
{
    keyDownFunc = func;
}

void FFMainLoopSetKeyUpFunc(void (*func)(int))
{
    keyUpFunc = func;
}

void FFMainLoopSetMouseMoveFunc(void (*func)(int, int, int, int))
{
    mouseMoveFunc = func;
}

void FFMainLoopSetMouseLeftButtonDownFunc(void (*func)(int, int))
{
    mouseLeftButtonDownFunc = func;
}

void FFMainLoopSetMouseLeftButtonUpFunc(void (*func)(int, int))
{
    mouseLeftButtonUpFunc = func;
}

void FFMainLoopSetMouseRightButtonDownFunc(void (*func)(int, int))
{
    mouseRightButtonDownFunc = func;
}

void FFMainLoopSetMouseRightButtonUpFunc(void (*func)(int, int))
{
    mouseRightButtonUpFunc = func;
}


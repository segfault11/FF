//
//  Impl.c
//  ObjRendererTest
//
//  Created by Arno in Wolde Luebke on 06.04.14.
//  Copyright (c) 2014 Arno in Wolde Luebke. All rights reserved.
//

#include <stdio.h>
#include <Fxs/OpenGL/Program.h>
#include "ObjRendererMesh.h"
#include "FFObjRenderer.h"


void Init()
{
//    ObjRendererMesh* mesh = ObjRendererMeshCreateWithFile("LighthouseColored.obj");

    FFObjRendererCreate();
    FFObjRendererLoad("LighthouseColored.obj");

//    if (!mesh)
//    {
//        puts("Failed to load mesh");
//    }

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_DEPTH_TEST);
    
    puts("Hallo, Welt");
}

int Update(float dt)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    FFObjRendererRender();
    
    return 0;
}
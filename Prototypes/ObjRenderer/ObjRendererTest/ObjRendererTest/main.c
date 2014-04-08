//
//  main.c
//  ObjRendererTest
//
//  Created by Arno in Wolde Luebke on 06.04.14.
//  Copyright (c) 2014 Arno in Wolde Luebke. All rights reserved.
//

#include <stdio.h>
#include <FF/MainLoop/MainLoop.h>
#include "Impl.h"

int main(int argc, const char * argv[])
{
    FFMainLoopCreate("Config.json");
    FFMainLoopSetInitFunc(Init);
    FFMainLoopSetUpdateFunc(Update);
    FFMainLoopRun();
    FFMainLoopDestroy();

    return 0;
}


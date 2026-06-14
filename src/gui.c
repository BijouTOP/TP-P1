// src/gui.c
#include "raylib.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#pragma GCC diagnostic pop

unsigned int *GetGuiIcons(void)
{
    return guiIcons;
}
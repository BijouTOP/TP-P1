#include "utils.h"

#include "raylib.h"

// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raygui.h"

#pragma GCC diagnostic pop

int closeWindow()
{
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
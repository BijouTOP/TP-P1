#include "raylib.h"

// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raygui.h"

#pragma GCC diagnostic pop

void drawInventory(float fontSize, float iconScale, int AddIconId, int MinusIconId, int UploadIconId, int DownloadIconId, Rectangle AddIconRect, Rectangle DownloadIconRect, Rectangle UploadIconRect, Rectangle bounds);
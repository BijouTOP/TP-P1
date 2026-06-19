#include "raylib.h"

// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raygui.h"

#pragma GCC diagnostic pop

typedef struct ScrollVars
{
    Vector2 scroll;
    Rectangle view;
} ScrollVars;

int closeWindow();
void drawTextBoxWithPlaceholder(Rectangle bounds, char *text, int textSize, bool *editMode, const char *placeholder);
bool drawIconWcollisions(int id, int size, Rectangle collision);
void drawDisabledButton(Rectangle ButtonBounds, char *text);
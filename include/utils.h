#include "raylib.h"
#include "raygui.h"

typedef struct ScrollVars
{
    Vector2 scroll;
    Rectangle view;
} ScrollVars;

int closeWindow();
void drawTextBoxWithPlaceholder(Rectangle bounds, char *text, int textSize, bool *editMode, const char *placeholder);
bool drawIconWcollisions(int id, int size, Rectangle collision);
void drawDisabledButton(Rectangle ButtonBounds, char *text);
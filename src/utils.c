#include "utils.h"

#include "raylib.h"

// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raygui.h"
#include "string.h"

#pragma GCC diagnostic pop

int closeWindow()
{
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
void drawTextBoxWithPlaceholder(Rectangle bounds, char *text, int textSize, bool *editMode, const char *placeholder)
{
    if (strlen(text) == 0 && !(*editMode))
    {
        GuiLabel(bounds, placeholder);
    }
    if (GuiTextBox(bounds, text, textSize, *editMode))
    {
        *editMode = !(*editMode);
    }
}
bool drawIconWcollisions(int id, int size, Rectangle collision)
{
    GuiDrawIcon(id, collision.x, collision.y, size, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), collision) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        return true;
    }
    return false;
}
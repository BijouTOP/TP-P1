#include "utils.h"

#include "raylib.h"
#include "raygui.h"

#include "string.h"

#pragma GCC diagnostic pop

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
void drawDisabledButton(Rectangle ButtonBounds, char *text)
{
    {
        Color baseColor = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_DISABLED));
        Color textColor = GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_DISABLED));
        Color borderColor = GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_DISABLED));
        int borderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);

        DrawRectangleRec(ButtonBounds, baseColor);
        DrawRectangleLinesEx(ButtonBounds, borderWidth, borderColor);
        DrawText(text,
                 ButtonBounds.x + (ButtonBounds.width - MeasureText(text, GuiGetStyle(DEFAULT, TEXT_SIZE))) / 2,
                 ButtonBounds.y + (ButtonBounds.height - GuiGetStyle(DEFAULT, TEXT_SIZE)) / 2,
                 GuiGetStyle(DEFAULT, TEXT_SIZE),
                 textColor);
    }
}
#include "inventory.h"

#include "raylib.h"

// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raygui.h"

#pragma GCC diagnostic pop

#include "string.h"
#include "stdio.h"

typedef enum
{
    ROUTER,
    SWITCH,
    ACCESS_POINT,
    SERVER,
    NAS,
    PRINTER,
    CAMERA,
    SENSOR,
    UPS
} EquipmentType;
typedef struct
{
    int internalCode;
    char name[50];
    EquipmentType type;
    char brand[50];
    char model[50];
    char ipAddress[16];
    char macAddress[18];
    char location[100];
    char status[20];
    char lastMaintenanceDate[11];
} InventoryItem;
typedef struct
{
    InventoryItem item;
    struct Node *next;
} Node;
char itemName[50] = "";
char itemBrand[50] = "";
char itemModel[50] = "";
char itemIp[16] = "";
char itemMac[18] = "";
char itemLocation[100] = "";
char itemStatus[20] = "";
char itemLastMaintenanceDate[11] = "";
bool editModes[7] = {false};
bool showAddItemDialog = false;

Vector2 scrollItems = {0, 0};
Rectangle viewItems = {0};

Vector2 scrollDialog = {0, 0};
Rectangle viewDialog = {0};

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
void drawInventory(float fontSize, float paddingAccountingForIcon, float iconSize, float iconScale, int AddIconId, int MinusIconId)
{

    if (fontSize > 22)
    {
        GuiSetStyle(DEFAULT, TEXT_SIZE, 22);
    }
    else
    {
        GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    }

    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

    Rectangle AddIconRect = {iconSize + paddingAccountingForIcon + GetScreenWidth() - paddingAccountingForIcon * 3 + 5, paddingAccountingForIcon + iconSize + (iconScale == 1 ? 4 : -4), iconScale == 1 ? 16 : iconSize, iconScale == 1 ? 16 : iconSize};
    if (showAddItemDialog)
    {
        GuiDrawIcon(MinusIconId, AddIconRect.x, AddIconRect.y, iconScale == 1 ? 1 : 2, BLACK);
    }
    else
    {
        GuiDrawIcon(AddIconId, AddIconRect.x, AddIconRect.y, iconScale == 1 ? 1 : 2, BLACK);
    }
    if (CheckCollisionPointRec(GetMousePosition(), AddIconRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        showAddItemDialog = !showAddItemDialog;
    }

    Rectangle bounds = {iconSize + paddingAccountingForIcon, iconSize + paddingAccountingForIcon, GetScreenWidth() - paddingAccountingForIcon * 3, GetScreenHeight() - paddingAccountingForIcon * 3}; // Panel bounds
    Rectangle content = {0, 0, 500, 500};                                                                                                                                                             // Content size

    GuiScrollPanel(bounds, "Inventario", content, &scrollItems, &viewItems);
    BeginScissorMode(bounds.x, bounds.y, bounds.width, bounds.height);
    EndScissorMode();

    if (showAddItemDialog)
    {
        Rectangle dialogBounds = {GetScreenWidth() / 4, GetScreenHeight() / 4, GetScreenWidth() / 2, GetScreenHeight() / 2};
        Rectangle contentDialog = {0, 0, dialogBounds.width - 20, dialogBounds.height < 355 ? 500 : dialogBounds.height - 32};
        GuiScrollPanel(dialogBounds, "Adicionar Item", contentDialog, &scrollDialog, &viewDialog);

        BeginScissorMode(dialogBounds.x, dialogBounds.y + 24, dialogBounds.width, dialogBounds.height - 32);

        const char *placeholders[7] = {"Nome", "Marca", "Modelo", "IP", "MAC", "Localização", "Status"};

        for (int i = 0; i < 7; i++)
        {
            Rectangle textBoxBounds = {dialogBounds.x + 20, dialogBounds.y + 40 + (i * 45) + scrollDialog.y, dialogBounds.width - 60, 35};

            switch (i)
            {
            case 0:
                drawTextBoxWithPlaceholder(textBoxBounds, itemName, fontSize, &editModes[0], placeholders[i]);
                break;
            case 1:
                drawTextBoxWithPlaceholder(textBoxBounds, itemBrand, fontSize, &editModes[1], placeholders[i]);
                break;
            case 2:
                drawTextBoxWithPlaceholder(textBoxBounds, itemModel, fontSize, &editModes[2], placeholders[i]);
                break;
            case 3:
                drawTextBoxWithPlaceholder(textBoxBounds, itemIp, fontSize, &editModes[3], placeholders[i]);
                break;
            case 4:
                drawTextBoxWithPlaceholder(textBoxBounds, itemMac, fontSize, &editModes[4], placeholders[i]);
                break;
            case 5:
                drawTextBoxWithPlaceholder(textBoxBounds, itemLocation, fontSize, &editModes[5], placeholders[i]);
                break;
            case 6:
                drawTextBoxWithPlaceholder(textBoxBounds, itemStatus, fontSize, &editModes[6], placeholders[i]);
                break;
            }
        }
        EndScissorMode();
    }
}
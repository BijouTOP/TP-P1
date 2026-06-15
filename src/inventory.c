#include "inventory.h"

#include "raylib.h"

// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raygui.h"

#pragma GCC diagnostic pop

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
typedef enum
{
    OPERACIONAL,
    MANUTENCAO,
    FALHA,
    DESATIVADO
} EquipmentStatus;

typedef struct InventoryItem
{
    int internalCode;
    char name[50];
    EquipmentType type;
    char brand[50];
    char model[50];
    char ipAddress[16];
    char macAddress[18];
    char location[100];
    EquipmentStatus status;
    char lastMaintenanceDate[11];
} InventoryItem;
typedef struct Node
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
bool editModes[9] = {false};
int activeItems[2] = {0};
bool showAddItemDialog = false;
Node *inventoryList = NULL;

Vector2 scrollItems = {0, 0};
Rectangle viewItems = {0};

Vector2 scrollDialog = {0, 0};
Rectangle viewDialog = {0};
void saveItem()
{
    Node *newNode = (Node *)malloc(sizeof(Node));

    strncpy(newNode->item.name, itemName, 50);
    strncpy(newNode->item.brand, itemBrand, 50);
    strncpy(newNode->item.model, itemModel, 50);
    strncpy(newNode->item.ipAddress, itemIp, 16);
    strncpy(newNode->item.macAddress, itemMac, 18);
    strncpy(newNode->item.location, itemLocation, 100);

    newNode->item.type = (EquipmentType)activeItems[0];
    newNode->item.status = (EquipmentStatus)activeItems[1];

    newNode->next = inventoryList;
    inventoryList = newNode;

    itemName[0] = '\0';
    itemBrand[0] = '\0';
    itemModel[0] = '\0';
    itemIp[0] = '\0';
    itemMac[0] = '\0';
    itemLocation[0] = '\0';
    activeItems[0] = 0;
    activeItems[1] = 0;
    showAddItemDialog = false;
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

    Rectangle bounds = {iconSize + paddingAccountingForIcon, iconSize + paddingAccountingForIcon, GetScreenWidth() - paddingAccountingForIcon * 3, GetScreenHeight() - paddingAccountingForIcon * 3};
    Rectangle content = {0, 0, 500, 500};

    GuiScrollPanel(bounds, "Inventario", content, &scrollItems, &viewItems);

    Node *current = inventoryList;

    BeginScissorMode(bounds.x, bounds.y, bounds.width, bounds.height);

    float modificarLenght = MeasureText("Modificar", fontSize > 22 ? 22 : fontSize);
    float deletarLenght = MeasureText("Deletar", fontSize > 22 ? 22 : fontSize);
    float fontSizeForButtons = fontSize > 22 ? 22 + 5 : fontSize + 5;

    for (int i = 0; current != NULL; i++)
    {
        GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        Rectangle itemBounds = {bounds.x + 30, bounds.y + 40 + (i * 40) + scrollItems.y, bounds.width - 70 - modificarLenght - deletarLenght, fontSizeForButtons};
        Rectangle deleteButtonBounds = {bounds.x + bounds.width - 30 - deletarLenght, itemBounds.y, deletarLenght + 10, fontSizeForButtons};
        Rectangle modifyButtonBounds = {bounds.x + bounds.width - 50 - modificarLenght - deletarLenght, itemBounds.y, modificarLenght + 10, fontSizeForButtons};

        GuiLabel(itemBounds, current->item.name);

        if (GuiButton(modifyButtonBounds, "Modificar"))
        {
        }

        if (GuiButton(deleteButtonBounds, "Deletar"))
        {
            Node *temp = inventoryList;
            Node *prev = NULL;

            while (temp != NULL && temp != current)
            {
                prev = temp;
                temp = temp->next;
            }

            if (temp == NULL)
                return;

            if (prev == NULL)
                inventoryList = temp->next;
            else
                prev->next = temp->next;

            free(temp);
        }

        current = current->next;
    }
    EndScissorMode();

    if (showAddItemDialog)
    {
        Rectangle dialogBounds = {GetScreenWidth() / 4, GetScreenHeight() / 4, GetScreenWidth() / 2, GetScreenHeight() / 2};
        Rectangle contentDialog = {0, 0, dialogBounds.width - 20, dialogBounds.height < 355 ? 500 : dialogBounds.height - 32};
        GuiScrollPanel(dialogBounds, "Adicionar Item", contentDialog, &scrollDialog, &viewDialog);

        BeginScissorMode(dialogBounds.x, dialogBounds.y + 24, dialogBounds.width, dialogBounds.height - 32);

        const char *placeholders[7] = {"Nome", "Marca", "Modelo", "IP", "MAC", "Localização", "Status"};

        for (int i = 0; i < 10; i++)
        {
            Rectangle textBoxBounds = {dialogBounds.x + 20, dialogBounds.y + 40 + (i * 45) + scrollDialog.y, dialogBounds.width - 60, 35};

            switch (i)
            {
            case 0:
                drawTextBoxWithPlaceholder(textBoxBounds, itemName, 50, &editModes[0], placeholders[0]);
                break;
            case 1:
                drawTextBoxWithPlaceholder(textBoxBounds, itemBrand, 50, &editModes[1], placeholders[1]);
                break;
            case 2:
                break;
            case 3:
                if (editModes[2])
                {
                    break;
                }
                drawTextBoxWithPlaceholder(textBoxBounds, itemModel, 50, &editModes[3], placeholders[2]);
                break;
            case 4:
                if (editModes[2])
                {
                    break;
                }
                drawTextBoxWithPlaceholder(textBoxBounds, itemIp, 16, &editModes[4], placeholders[3]);
                break;
            case 5:
                if (editModes[2])
                {
                    break;
                }
                drawTextBoxWithPlaceholder(textBoxBounds, itemMac, 18, &editModes[5], placeholders[4]);
                break;
            case 6:
                if (editModes[2])
                {
                    break;
                }
                drawTextBoxWithPlaceholder(textBoxBounds, itemLocation, 100, &editModes[6], placeholders[5]);
                break;
            case 7:
                break;
            case 8:
                if (editModes[2] || editModes[7])
                {
                    break;
                }
                drawTextBoxWithPlaceholder(textBoxBounds, itemStatus, 20, &editModes[8], placeholders[6]);
                break;
            case 9:
                if (editModes[2] || editModes[7])
                {
                    break;
                }
                if (GuiLabelButton(textBoxBounds, "Adicionar Item"))
                {
                    saveItem();
                }
                break;
            }
        }
        if (editModes[2])
        {
            EndScissorMode();
        }
        Rectangle dropdownTypeBounds = {dialogBounds.x + 20, dialogBounds.y + 40 + (2 * 45) + scrollDialog.y, dialogBounds.width - 60, 35};
        if (GuiDropdownBox(dropdownTypeBounds, "Router;Switch;Access Point;Server;NAS;Printer;Camera;Sensor;UPS", &activeItems[0], editModes[2]))
        {
            editModes[2] = !editModes[2];
        }
        if (editModes[7])
        {
            EndScissorMode();
        }
        Rectangle dropdownStatusBounds = {dialogBounds.x + 20, dialogBounds.y + 40 + (7 * 45) + scrollDialog.y, dialogBounds.width - 60, 35};
        if (!editModes[2])
        {
            if (GuiDropdownBox(dropdownStatusBounds, "Operacional;Em Manutenção; Em Falha; Desativado", &activeItems[1], editModes[7]))
            {
                editModes[7] = !editModes[7];
            }
        }
        if (!editModes[2] || !editModes[7])
        {
            EndScissorMode();
        }
    }
}
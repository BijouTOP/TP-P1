#include "incidentes.h"
#include "utils.h"

#include "raylib.h"

// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raygui.h"

#pragma GCC diagnostic pop

bool showAddIncidenteDialog = false;

Vector2 scrollIncidentes = {0, 0};
Rectangle viewIncidentes = {0};

Vector2 scrollDialog = {0, 0};
Rectangle viewDialog = {0};

char searchText[50] = "";
bool searchEdit = false;

int filterType = 0;
bool filterTypeEdit = false;

void drawIncidentes(float fontSize, float iconScale, int AddIconId, int MinusIconId, int UploadIconId, int DownloadIconId, Rectangle AddIconRect, Rectangle DownloadIconRect, Rectangle UploadIconRect, Rectangle bounds)
{
    if (fontSize > 22)
    {
        GuiSetStyle(DEFAULT, TEXT_SIZE, 22);
    }
    else
    {
        GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    }

    if (showAddIncidenteDialog)
    {
        GuiDrawIcon(MinusIconId, AddIconRect.x, AddIconRect.y, iconScale == 1 ? 1 : 2, BLACK);
    }
    else if (!showAddIncidenteDialog)
    {
        GuiDrawIcon(AddIconId, AddIconRect.x, AddIconRect.y, iconScale == 1 ? 1 : 2, BLACK);
    }
    if (CheckCollisionPointRec(GetMousePosition(), AddIconRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        showAddIncidenteDialog = !showAddIncidenteDialog;
    }
    GuiDrawIcon(UploadIconId, UploadIconRect.x, UploadIconRect.y, iconScale == 1 ? 1 : 2, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), UploadIconRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        // loadInventoryFromFile("incidentes.dat");
    }
    GuiDrawIcon(DownloadIconId, DownloadIconRect.x, DownloadIconRect.y, iconScale == 1 ? 1 : 2, BLACK);
    if (CheckCollisionPointRec(GetMousePosition(), DownloadIconRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        // saveInventoryToFile("incidentes.dat");
    }

    float fontSizeForButtons = fontSize > 22 ? 22 + 5 : fontSize + 5;
    float searchLenght = MeasureText("Pesquisar...", fontSize > 22 ? 22 : fontSize);
    float statusLenght = MeasureText("CONCLUIDO", fontSize > 22 ? 22 : fontSize);

    bool isNotBullied = (bounds.width - statusLenght - 10) > searchLenght;
    float searchBullyDefender = isNotBullied ? (bounds.width - statusLenght - 10) : 0.5 * bounds.width - 10;

    Rectangle searchBounds = {bounds.x, bounds.y - fontSizeForButtons - 5, searchBullyDefender, fontSizeForButtons};
    // Rectangle statusBounds = {searchBounds.x + 5, searchBounds.y, isNotBullied ? statusLenght : 0.5 * bounds.width, fontSizeForButtons};
    Rectangle content = {0, 0, 500, 500};
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

    drawTextBoxWithPlaceholder(searchBounds, searchText, 50, &searchEdit, "Pesquisar...");

    // Node *current = inventoryList;
    GuiScrollPanel(bounds, "Inventario", content, &scrollIncidentes, &viewIncidentes);
    BeginScissorMode(bounds.x, bounds.y, bounds.width, bounds.height);

    // for (int i = 0; current != NULL; i++)
    // {
    //     bool matchesCode = strlen(searchText) > 0 && atoi(searchText) == current->item.internalCode;
    //     bool matchesSearch = strlen(searchText) == 0 || strstr(current->item.name, searchText) != NULL || strstr(current->item.ipAddress, searchText) != NULL || strstr(current->item.macAddress, searchText) != NULL || matchesCode;
    //     bool matchesType = filterType == 0 || current->item.type == (EquipmentType)(filterType - 1);
    //     bool matchesStatus = filterStatus == 0 || current->item.status == (EquipmentStatus)(filterStatus - 1);

    //     if (!(matchesSearch && matchesType && matchesStatus))
    //     {
    //         i--;
    //         current = current->next;
    //         continue;
    //     }

    //     GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
    //     Rectangle itemBounds = {bounds.x + 30, bounds.y + 40 + (i * 40) + scrollItems.y, bounds.width - 70 - modificarLenght - deletarLenght, fontSizeForButtons};
    //     Rectangle deleteButtonBounds = {bounds.x + bounds.width - 30 - deletarLenght, itemBounds.y, deletarLenght + 10, fontSizeForButtons};
    //     Rectangle modifyButtonBounds = {bounds.x + bounds.width - 50 - modificarLenght - deletarLenght, itemBounds.y, modificarLenght + 10, fontSizeForButtons};

    //     char displayText[100];

    //     snprintf(displayText, sizeof(displayText),
    //              "%d - %s",
    //              current->item.internalCode,
    //              current->item.name);

    //     GuiLabel(itemBounds, displayText);

    //     current = current->next;
    // }
    EndScissorMode();

    // if (GuiDropdownBox(statusBounds, "Todos;Operacional;Em Manutenção;Em Falha;Desativado", &filterStatus, filterStatusEdit))
    //     filterStatusEdit = !filterStatusEdit;

    // if (showAddIncidenteDialog && !isEditing)
    // {
    //     showModal(itemName, itemBrand, itemModel, itemIp, itemMac, itemLocation, itemLastMaintenanceDate, &activeType, &activeStatus);
    // }
}
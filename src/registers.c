#include "registers.h"
#include "utils.h"
#include "incidentes.h"

#include "raylib.h"

// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raygui.h"

#pragma GCC diagnostic pop

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const RegisterItem emptyRegister = {
    .equipmentCode = "",
    .configType = "",
    .previousValue = "",
    .newValue = "",
    .dateTime = "",
    .technician = ""};

static RegisterNode *registerList = NULL;
static RegisterNode *selectedRegisterNode = NULL;

static ScrollVars itemsScroll = {
    .scroll = {0, 0},
    .view = {0}};

static char searchText[50] = "";
static bool searchEdit = false;

static int spinnerValue = 0;
static bool spinnerEdit = false;

static void clearRegisters()
{
    RegisterNode *current = registerList;
    while (current != NULL)
    {
        RegisterNode *next = current->next;
        free(current);
        current = next;
    }
    registerList = NULL;
}
static void saveRegistersToFile(const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
        return;

    int count = 0;
    RegisterNode *curr = registerList;
    while (curr)
    {
        count++;
        curr = curr->next;
    }

    RegisterItem *items = malloc(sizeof(RegisterItem) * count);
    curr = registerList;
    for (int i = count - 1; i >= 0; i--)
    { // Fill backwards
        items[i] = curr->registerItem;
        curr = curr->next;
    }

    fwrite(items, sizeof(RegisterItem), count, file);

    free(items);
    fclose(file);
}

static void loadRegistersFromFile(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
        return;

    clearRegisters();

    RegisterItem tempItem;

    while (fread(&tempItem, sizeof(RegisterItem), 1, file) == 1)
    {
        bool exists = false;
        clearRegisters();
        if (!exists)
        {
            RegisterNode *newNode = malloc(sizeof(RegisterNode));
            if (newNode == NULL)
                break;

            newNode->registerItem = tempItem;
            newNode->next = registerList;
            registerList = newNode;
        }
    }

    fclose(file);
}
// static void copy(RegisterNode *node, RegisterItem *log)
// {
//     strncpy(node->registerItem.equipmentCode, log->equipmentCode, 50);
//     strncpy(node->registerItem.configType, log->configType, 50);
//     strncpy(node->registerItem.previousValue, log->previousValue, 100);
//     strncpy(node->registerItem.newValue, log->newValue, 100);
//     strncpy(node->registerItem.dateTime, log->dateTime, 17);
//     strncpy(node->registerItem.technician, log->technician, 50);
// }
// static void saveItem(RegisterItem *item)
// {

//     RegisterNode *newNode = (RegisterNode *)malloc(sizeof(RegisterNode));

//     copy(newNode, item);

//     newNode->next = registerList;
//     registerList = newNode;

//     *item = emptyRegister;
// }

void drawRegisters(float fontSize, float iconScale, int UndoIconId, int RedoIconId, int UploadIconId, int DownloadIconId, int InfoIconId, Rectangle UndoIconRect, Rectangle RedoIconRect, Rectangle DownloadIconRect, Rectangle UploadIconRect, Rectangle bounds)
{

    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize > 22 ? 22 : fontSize);

    if (drawIconWcollisions(UndoIconId, iconScale == 1 ? 1 : 2, UndoIconRect))
    {
        //
    }
    if (drawIconWcollisions(RedoIconId, iconScale == 1 ? 1 : 2, RedoIconRect))
    {
        //
    }
    if (drawIconWcollisions(UploadIconId, iconScale == 1 ? 1 : 2, UploadIconRect))
    {
        loadRegistersFromFile("data/configuracoes.dat");
    }
    if (drawIconWcollisions(DownloadIconId, iconScale == 1 ? 1 : 2, DownloadIconRect))
    {
        saveRegistersToFile("data/configuracoes.dat");
    }

    float fontSizeForButtons = fontSize > 22 ? 22 + 5 : fontSize + 5;

    Rectangle searchBounds = {bounds.x, bounds.y - fontSizeForButtons - 5, 0.34 * bounds.width - 10, fontSizeForButtons};
    Rectangle valueBounds = {searchBounds.x + searchBounds.width + 5, searchBounds.y, 0.33 * bounds.width, fontSizeForButtons};
    Rectangle clearBounds = {valueBounds.x + valueBounds.width + 5, searchBounds.y, 0.33 * bounds.width, fontSizeForButtons};
    Rectangle content = {0, 0, 500, 500};
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

    drawTextBoxWithPlaceholder(searchBounds, searchText, 50, &searchEdit, "Pesquisar...");
    if (GuiSpinner(valueBounds, NULL, &spinnerValue, 0, 100, spinnerEdit))
        spinnerEdit = !spinnerEdit;
    if (GuiLabelButton(clearBounds, "Limpar"))
    {
        clearRegisters();
    }
    RegisterNode *current = registerList;

    GuiScrollPanel(bounds, "Registers", content, &itemsScroll.scroll, &itemsScroll.view);
    // panel title 24
    BeginScissorMode(bounds.x, bounds.y + 24, bounds.width, bounds.height - 24);

    for (int i = 0; current != NULL; i++)
    {
        bool matchesCode = strlen(searchText) > 0 && (atoi(searchText) == atoi(current->registerItem.equipmentCode) || strstr(current->registerItem.equipmentCode, searchText) != NULL);

        bool matchesSearch = strlen(searchText) == 0 ||
                             strstr(current->registerItem.configType, searchText) != NULL ||
                             strstr(current->registerItem.previousValue, searchText) != NULL ||
                             strstr(current->registerItem.newValue, searchText) != NULL ||
                             strstr(current->registerItem.technician, searchText) != NULL ||
                             matchesCode;

        if (!matchesSearch || (spinnerValue > 0 && i >= spinnerValue))
        {
            i--;
            current = current->next;
            continue;
        }
        GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        Rectangle itemBounds = {bounds.x + 30, bounds.y + 40 + (i * 40) + itemsScroll.scroll.y, bounds.width - 26, fontSizeForButtons};
        Rectangle showinfo = {bounds.x + bounds.width - 16, itemBounds.y, 16, fontSizeForButtons};

        char displayText[150];

        snprintf(displayText, sizeof(displayText),
                 "%s - %s",
                 current->registerItem.equipmentCode,
                 current->registerItem.configType);

        GuiLabel(itemBounds, displayText);

        if (GuiButton(showinfo, TextFormat("#%d#", InfoIconId)))
            selectedRegisterNode = (selectedRegisterNode == current) ? NULL : current;

        current = current->next;
    }
    EndScissorMode();
}
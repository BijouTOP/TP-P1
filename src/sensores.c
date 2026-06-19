#include "sensores.h"
#include "utils.h"

#include "raylib.h"

// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raygui.h"

#pragma GCC diagnostic pop

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static SensorNode *selectedSensorNode = NULL;

static SensorNode *sensorsList = NULL;

static ScrollVars sensorsScroll = {
    .scroll = {0, 0},
    .view = {0}};

static char searchText[50] = "";
static bool searchEdit = false;

static int filterStatus = 0;
static bool filterStatusEdit = false;

void drawSensors(float fontSize, int InfoIconId, Rectangle bounds)
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize > 22 ? 22 : fontSize);

    float fontSizeForButtons = fontSize > 22 ? 22 + 5 : fontSize + 5;
    float statusLenght = MeasureText("FALHA_REDE", fontSize > 22 ? 22 : fontSize);

    Rectangle searchBounds = {bounds.x, bounds.y - fontSizeForButtons - 5, bounds.width - statusLenght - 5, fontSizeForButtons};
    Rectangle statusBounds = {searchBounds.x + searchBounds.width + 5, searchBounds.y, statusLenght, fontSizeForButtons};

    Rectangle content = {0, 0, 500, 500};
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    drawTextBoxWithPlaceholder(searchBounds, searchText, 50, &searchEdit, "Pesquisar...");
    SensorNode *current = sensorsList;

    GuiScrollPanel(bounds, "Sensores", content, &sensorsScroll.scroll, &sensorsScroll.view);
    BeginScissorMode(bounds.x, bounds.y + 24, bounds.width, bounds.height - 24);

    for (int i = 0; current != NULL; i++)
    {
        bool matchesSearch = strlen(searchText) == 0 ||
                             strstr(current->sensor.equipmentCode, searchText) != NULL ||
                             strstr(current->sensor.type, searchText) != NULL;
        bool matchesStatus = filterStatus == 0 ||
                             current->sensor.status == (sensorStatus)(filterStatus - 1);

        if (!(matchesSearch && matchesStatus))
        {
            i--;
            current = current->next;
            continue;
        }

        GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        Rectangle itemBounds = {bounds.x + 30, bounds.y + 40 + (i * 40) + sensorsScroll.scroll.y, bounds.width - 26, fontSizeForButtons};
        Rectangle showinfo = {bounds.x + bounds.width - 46, itemBounds.y, 16, fontSizeForButtons};

        char displayText[100];
        snprintf(displayText, sizeof(displayText), "%s | %s", current->sensor.equipmentCode, current->sensor.type);
        GuiLabel(itemBounds, displayText);

        if (GuiButton(showinfo, TextFormat("#%d#", InfoIconId)))
        {
            selectedSensorNode = (selectedSensorNode == current) ? NULL : current;
        }

        current = current->next;
    }

    EndScissorMode();
    if (selectedSensorNode != NULL)
    {
        const char *statusStr[] = {"NORMAL", "AVISO", "CRITICO", "FALHA_REDE"};

        char infoText[400];
        snprintf(infoText, sizeof(infoText),
                 "Código: %s\nTipo: %s\nValor Atual: %.2f %s\nEstado: %s",
                 selectedSensorNode->sensor.equipmentCode,
                 selectedSensorNode->sensor.type,
                 selectedSensorNode->sensor.value,
                 selectedSensorNode->sensor.unit,
                 statusStr[selectedSensorNode->sensor.status]);

        int result = GuiMessageBox(
            (Rectangle){GetScreenWidth() / 4, GetScreenHeight() / 4, GetScreenWidth() / 2, GetScreenHeight() / 2},
            "#140#Detalhes do Sensor",
            infoText,
            "Fechar");

        if (result >= 0)
        {
            selectedSensorNode = NULL;
        }
    }
    if (GuiDropdownBox(statusBounds, "Todos;Falha de Rede;Crítico;Aviso;Normal", &filterStatus, filterStatusEdit))
        filterStatusEdit = !filterStatusEdit;
}
// same scroll bug/provavelmente o modal passa clicks mas we have no time ;(

#include "incidentes.h"
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
#include <time.h>

static int nextInternalCode = 1;

static int showInfoForId = -1;

static DropdownVarIncidentes newDrop;

const Incident emptyIncident = {
    .id = 0,
    .equipmentCode = "",
    .type = "",
    .description = "",
    .priority = 0,
    .createdAt = "",
    .concludedAt = "",
    .technician = "",
    .status = 0};

static Incident newIncident;

static bool editModes[6] = {false};

static bool showAddIncidenteDialog = false;

static IncidentNode *incidentList = NULL;

static ScrollVars incidentsScroll = {
    .scroll = {0, 0},
    .view = {0}};

static ScrollVars dialogScroll = {
    .scroll = {0, 0},
    .view = {0}};

static bool isEditing = false;

static char searchText[50] = "";
static bool searchEdit = false;

static int filterStatus = 0;
static bool filterStatusEdit = false;

static int filterPriority = 0;
static bool filterPriorityEdit = false;

static void clearIncidentes()
{
    IncidentNode *current = incidentList;

    while (current != NULL)
    {
        IncidentNode *next = current->next;

        free(current);

        current = next;
    }

    incidentList = NULL;
}
void saveIncidentsToFile(const char *filename)
{
    FILE *f = fopen(filename, "wb");

    if (!f)
        return;

    IncidentNode *current = incidentList;

    while (current)
    {
        fwrite(
            &current->incident,
            sizeof(Incident),
            1,
            f);

        current = current->next;
    }

    fclose(f);
}
void loadIncidentsFromFile(const char *filename)
{
    clearIncidentes();

    nextInternalCode = 1;
    showInfoForId = -1;
    FILE *f = fopen(filename, "rb");

    if (!f)
        return;

    Incident temp;

    while (fread(&temp, sizeof(Incident), 1, f))
    {
        IncidentNode *node =
            malloc(sizeof(IncidentNode));

        node->incident = temp;
        node->next = NULL;

        if (!incidentList)
        {
            incidentList = node;
        }
        else
        {
            IncidentNode *tail = incidentList;

            while (tail->next)
                tail = tail->next;

            tail->next = node;
        }

        if (temp.id >= nextInternalCode)
            nextInternalCode = temp.id + 1;
    }

    fclose(f);
}
static void copy(IncidentNode *node, Incident *item)
{
    strncpy(node->incident.equipmentCode, item->equipmentCode, 50);
    strncpy(node->incident.type, item->type, 50);
    strncpy(node->incident.description, item->description, 50);
    strncpy(node->incident.technician, item->technician, 50);
    strncpy(node->incident.createdAt, item->createdAt, 17);
    strncpy(node->incident.concludedAt, item->concludedAt, 17);

    node->incident.status = item->status;
    node->incident.priority = item->priority;
}
static void saveItem(Incident *item, DropdownVarIncidentes *drop)
{
    item->status = (IncidentStatus)drop->status;
    item->priority = (IncidentPriority)drop->priority;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    strftime(item->createdAt, sizeof(item->createdAt), "%d-%m-%Y %H:%M", &tm);

    if (item->status == INCIDENTE_CONCLUIDO)
        strncpy(item->concludedAt, item->createdAt, 17);
    else
        item->concludedAt[0] = '\0';

    IncidentNode *newNode = (IncidentNode *)malloc(sizeof(IncidentNode));

    newNode->incident.id = nextInternalCode++;

    copy(newNode, item);

    newNode->next = NULL;
    if (incidentList == NULL)
    {
        incidentList = newNode;
    }
    else
    {
        IncidentNode *tail = incidentList;
        while (tail->next != NULL)
            tail = tail->next;
        tail->next = newNode;
    }

    showAddIncidenteDialog = false;

    *item = emptyIncident;
}
static void showModal(Incident *item, DropdownVarIncidentes *drop)
{
    Rectangle dialogBounds = {GetScreenWidth() / 4, GetScreenHeight() / 4, GetScreenWidth() / 2, GetScreenHeight() / 2};
    Rectangle contentDialog = {0, 0, dialogBounds.width - 20, dialogBounds.height < 355 ? 500 : dialogBounds.height - 32};
    GuiScrollPanel(dialogBounds, isEditing ? "Modificar Incident" : "Adicionar Incidente", contentDialog, &dialogScroll.scroll, &dialogScroll.view);

    BeginScissorMode(dialogBounds.x, dialogBounds.y + 24, dialogBounds.width, dialogBounds.height - 32);

    const char *placeholders[4] = {"Código", "Tipo", "Descrição", "Técnico"};

    for (int i = 0; i < 7; i++)
    {
        Rectangle textBoxBounds = {dialogBounds.x + 20, dialogBounds.y + 40 + (i * 45) + dialogScroll.scroll.y, dialogBounds.width - 60, 35};

        switch (i)
        {
        case 0:
            drawTextBoxWithPlaceholder(textBoxBounds, item->equipmentCode, 50, &editModes[0], placeholders[0]);
            break;
        case 1:
            drawTextBoxWithPlaceholder(textBoxBounds, item->type, 50, &editModes[1], placeholders[1]);
            break;
        case 2:
            drawTextBoxWithPlaceholder(textBoxBounds, item->description, 50, &editModes[2], placeholders[2]);
            break;
        case 3:
            break;
        case 4:
            if (editModes[3])
            {
                break;
            }
            drawTextBoxWithPlaceholder(textBoxBounds, item->technician, 50, &editModes[4], placeholders[3]);
            break;
        case 5:
            break;
        case 6:
            if (editModes[3] || editModes[5])
            {
                break;
            }
            if (GuiLabelButton(textBoxBounds, isEditing ? "Modificar Item" : "Adicionar Item"))
            {
                saveItem(item, drop);
            }
            break;
        }
    }
    if (editModes[3])
    {
        EndScissorMode();
    }
    Rectangle dropdownStatusBounds = {dialogBounds.x + 20, dialogBounds.y + 40 + (3 * 45) + dialogScroll.scroll.y, dialogBounds.width - 60, 35};
    if (GuiDropdownBox(dropdownStatusBounds, "Pendente;Em Curso;Concluido", &drop->status, editModes[3]))
    {
        editModes[3] = !editModes[3];
    }
    if (editModes[5])
    {
        EndScissorMode();
    }
    Rectangle dropdownPriorityBounds = {dialogBounds.x + 20, dialogBounds.y + 40 + (5 * 45) + dialogScroll.scroll.y, dialogBounds.width - 60, 35};
    if (!editModes[3])
    {
        if (GuiDropdownBox(dropdownPriorityBounds, "Baixa;Média;Alta", &drop->priority, editModes[5]))
        {
            editModes[5] = !editModes[5];
        }
    }
    if (!editModes[3] || !editModes[5])
    {
        EndScissorMode();
    }
}
void drawIncidentes(float fontSize, float iconScale, int AddIconId, int MinusIconId, int UploadIconId, int DownloadIconId, int InfoIconId, Rectangle AddIconRect, Rectangle DownloadIconRect, Rectangle UploadIconRect, Rectangle bounds)
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize > 22 ? 22 : fontSize);
    // usar funçao utils?
    if (showAddIncidenteDialog && !isEditing)
    {
        GuiDrawIcon(MinusIconId, AddIconRect.x, AddIconRect.y, iconScale == 1 ? 1 : 2, BLACK);
    }
    else if (!showAddIncidenteDialog)
    {
        newDrop.status = 0;
        newDrop.priority = 0;

        GuiDrawIcon(AddIconId, AddIconRect.x, AddIconRect.y, iconScale == 1 ? 1 : 2, BLACK);
    }
    if (CheckCollisionPointRec(GetMousePosition(), AddIconRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isEditing)
    {
        showAddIncidenteDialog = !showAddIncidenteDialog;
    }
    if (drawIconWcollisions(UploadIconId, iconScale == 1 ? 1 : 2, UploadIconRect))
    {
        loadIncidentsFromFile("data/incidentes.dat");
    }
    if (drawIconWcollisions(DownloadIconId, iconScale == 1 ? 1 : 2, DownloadIconRect))
    {
        saveIncidentsToFile("data/incidentes.dat");
    }

    float fontSizeForButtons = fontSize > 22 ? 22 + 5 : fontSize + 5;
    float searchLenght = MeasureText("Pesquisar...", fontSize > 22 ? 22 : fontSize);
    float statusLenght = MeasureText("INCIDENTE_CONCLUIDO", fontSize > 22 ? 22 : fontSize);
    float priorityLenght = MeasureText("PRIORIDADE_BAIXA", fontSize > 22 ? 22 : fontSize);
    bool isNotBullied = (bounds.width - priorityLenght - statusLenght - 10) > searchLenght;
    float searchBullyDefender = isNotBullied ? bounds.width - priorityLenght - statusLenght - 10 : 0.34 * bounds.width - 10;

    Rectangle searchBounds = {bounds.x, bounds.y - fontSizeForButtons - 5, searchBullyDefender, fontSizeForButtons};
    Rectangle statusBounds = {searchBounds.x + searchBounds.width + 5, searchBounds.y, isNotBullied ? statusLenght : 0.33 * bounds.width, fontSizeForButtons};
    Rectangle priorityBounds = {statusBounds.x + statusBounds.width + 5, searchBounds.y, isNotBullied ? statusLenght : 0.33 * bounds.width, fontSizeForButtons};
    Rectangle content = {0, 0, 500, 500};
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

    drawTextBoxWithPlaceholder(searchBounds, searchText, 50, &searchEdit, "Pesquisar...");

    IncidentNode *current = incidentList;
    GuiScrollPanel(bounds, "Incidentes", content, &incidentsScroll.scroll, &incidentsScroll.view);
    BeginScissorMode(bounds.x, bounds.y + 24, bounds.width, bounds.height - 24);

    float processarLenght = MeasureText("Processar", fontSize > 22 ? 22 : fontSize);
    float concluirLenght = MeasureText("Concluir", fontSize > 22 ? 22 : fontSize);

    for (int i = 0; current != NULL; i++)
    {
        bool matchesSearch = strlen(searchText) == 0 ||
                             strstr(current->incident.technician, searchText) != NULL ||
                             strstr(current->incident.equipmentCode, searchText) != NULL ||
                             (atoi(searchText) != 0 && atoi(searchText) == current->incident.id);
        bool matchesStatus = filterStatus == 0 ||
                             current->incident.status == (IncidentStatus)(filterStatus - 1);

        bool matchesPriority = filterPriority == 0 ||
                               current->incident.priority == (IncidentPriority)(filterPriority - 1);

        if (!(matchesSearch && matchesStatus && matchesPriority))
        {
            i--;
            current = current->next;
            continue;
        }
        GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        Rectangle itemBounds = {bounds.x + 30, bounds.y + 40 + (i * 40) + incidentsScroll.scroll.y, bounds.width - 70 - processarLenght - concluirLenght - fontSizeForButtons - 10, fontSizeForButtons};
        Rectangle processarButtonBounds = {bounds.x + bounds.width - 50 - processarLenght - concluirLenght, itemBounds.y, processarLenght + 10, fontSizeForButtons};
        Rectangle concluirButtonBounds = {bounds.x + bounds.width - 30 - concluirLenght, itemBounds.y, concluirLenght + 10, fontSizeForButtons};
        Rectangle showinfo = {bounds.x + bounds.width - 76 - processarLenght - concluirLenght, itemBounds.y, 16, fontSizeForButtons};

        char displayText[100];

        snprintf(displayText, sizeof(displayText),
                 "%d | %s",
                 current->incident.id,
                 current->incident.technician);

        GuiLabel(itemBounds, displayText);

        if (GuiButton(showinfo, TextFormat("#%d#", InfoIconId)))
            showInfoForId = (showInfoForId == current->incident.id) ? -1 : current->incident.id;
        if (current->incident.status == INCIDENTE_EM_CURSO || current->incident.status == INCIDENTE_CONCLUIDO)
        {
            Color baseColor = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_DISABLED));
            Color textColor = GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_DISABLED));
            Color borderColor = GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_DISABLED));
            int borderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
            DrawRectangleRec(processarButtonBounds, baseColor);
            DrawRectangleLinesEx(processarButtonBounds, borderWidth, borderColor);
            DrawText("Processar",
                     processarButtonBounds.x + (processarButtonBounds.width - MeasureText("Processar", GuiGetStyle(DEFAULT, TEXT_SIZE))) / 2,
                     processarButtonBounds.y + (processarButtonBounds.height - GuiGetStyle(DEFAULT, TEXT_SIZE)) / 2,
                     GuiGetStyle(DEFAULT, TEXT_SIZE), textColor);
        }
        else if (GuiButton(processarButtonBounds, "Processar") && !showAddIncidenteDialog && !filterStatusEdit)
        {
            current->incident.status = INCIDENTE_EM_CURSO;
        }
        if (current->incident.status == INCIDENTE_CONCLUIDO)
        {
            Color baseColor = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_DISABLED));
            Color textColor = GetColor(GuiGetStyle(BUTTON, TEXT_COLOR_DISABLED));
            Color borderColor = GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_DISABLED));
            int borderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);

            DrawRectangleRec(concluirButtonBounds, baseColor);
            DrawRectangleLinesEx(concluirButtonBounds, borderWidth, borderColor);
            DrawText("Concluir",
                     concluirButtonBounds.x + (concluirButtonBounds.width - MeasureText("Concluir", GuiGetStyle(DEFAULT, TEXT_SIZE))) / 2,
                     concluirButtonBounds.y + (concluirButtonBounds.height - GuiGetStyle(DEFAULT, TEXT_SIZE)) / 2,
                     GuiGetStyle(DEFAULT, TEXT_SIZE),
                     textColor);
        }
        else if (GuiButton(concluirButtonBounds, "Concluir") && !showAddIncidenteDialog && !filterStatusEdit)
        {
            current->incident.status = INCIDENTE_CONCLUIDO;

            time_t t = time(NULL);
            struct tm tm = *localtime(&t);

            strftime(current->incident.concludedAt, sizeof(current->incident.concludedAt), "%d-%m-%Y %H:%M", &tm);
        }

        current = current->next;
    }
    EndScissorMode();
    if (showInfoForId >= 0)
    {
        IncidentNode *info = incidentList;
        while (info != NULL && info->incident.id != showInfoForId)
            info = info->next;

        if (info != NULL)
        {
            const char *statusStr[] = {"Pendente", "Em Curso", "Concluido"};
            const char *priorityStr[] = {"Baixa", "Média", "Alta"};

            char infoText[400];
            snprintf(infoText, sizeof(infoText),
                     "ID: %d  |  Tecnico: %s\nTipo: %s  |  Estado: %s\nPrioridade: %s\nEquipamento: %s\nCriado: %s\nConcluido: %s",
                     info->incident.id,
                     info->incident.technician,
                     info->incident.type,
                     statusStr[info->incident.status],
                     priorityStr[info->incident.priority],
                     info->incident.equipmentCode,
                     info->incident.createdAt,
                     strlen(info->incident.concludedAt) > 0 ? info->incident.concludedAt : "XX");

            int result = GuiMessageBox(
                (Rectangle){GetScreenWidth() / 4, GetScreenHeight() / 4, GetScreenWidth() / 2, GetScreenHeight() / 2},
                "#140#Detalhes do Incidente",
                infoText,
                "Fechar");

            if (result >= 0)
                showInfoForId = -1;
        }
    }
    if (GuiDropdownBox(statusBounds, "Todos;Pendente;Em Curso;Concluido", &filterStatus, filterStatusEdit))
        filterStatusEdit = !filterStatusEdit;

    if (GuiDropdownBox(priorityBounds, "Todos;Baixa;Média;Alta", &filterPriority, filterPriorityEdit))
        filterPriorityEdit = !filterPriorityEdit;

    if (showAddIncidenteDialog && !isEditing)
    {

        showModal(&newIncident, &newDrop);
    }
}
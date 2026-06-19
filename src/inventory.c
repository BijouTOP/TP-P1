// Falta fazer verificaçoes modal / pequeno bug dropdown can hover modificar / REFACTORAR CODE 😭

#include "inventory.h"
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

int nextInternalCode = 1;

DropdownVar newDrop;
DropdownVar modDrop;

const InventoryItem emptyState = {
    .internalCode = 1,
    .name = "",
    .type = 0,
    .brand = "",
    .model = "",
    .ipAddress = "",
    .macAddress = "",
    .location = "",
    .status = 0,
    .lastMaintenanceDate = ""};

static InventoryItem newItem;
static InventoryItem modifiableItem;

static bool editModes[9] = {false};

static bool showAddItemDialog = false;
static Node *inventoryList = NULL;

static ScrollVars itemsScroll = {
    .scroll = {0, 0},
    .view = {0}};

static ScrollVars dialogScroll = {
    .scroll = {0, 0},
    .view = {0}};

static bool isEditing = false;
static Node *nodeBeingEdited = NULL;

static char searchText[50] = "";
static bool searchEdit = false;

static int filterType = 0;
static bool filterTypeEdit = false;

static int filterStatus = 0;
static bool filterStatusEdit = false;

void linkIncidenteEquipamento(int equipmentId)
{

    Node *current = inventoryList;

    while (current != NULL)
    {
        if (current->item.internalCode == equipmentId)
        {
            current->item.incidents = false;
            current->item.status = OPERACIONAL;
            break;
        }
        current = current->next;
    }
}
static bool analisarResultadoPing(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
        return false;

    char linha[256];
    bool respondeu = false;

    while (fgets(linha, sizeof(linha), file))
    {
        if (strstr(linha, "ttl=") != NULL || strstr(linha, "TTL=") != NULL)
        {
            respondeu = true;
            break;
        }
    }

    fclose(file);
    return respondeu;
}

static void exePingItem(Node *node)
{
    if (node == NULL || strlen(node->item.ipAddress) == 0)
        return;

    char comando[256];
    const char *tempFile = "data/resultado_ping.txt";

#if defined(_WIN32)
    snprintf(comando, sizeof(comando), "ping -n 4 %s > %s", node->item.ipAddress, tempFile);
#else
    snprintf(comando, sizeof(comando), "ping -c 4 %s > %s", node->item.ipAddress, tempFile);
#endif

    system(comando);

    bool respondeu = analisarResultadoPing(tempFile);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    strftime(node->item.lastMaintenanceDate, sizeof(node->item.lastMaintenanceDate), "%d/%m/%Y", &tm);

    if (!respondeu)
    {
        node->item.status = 2;
        node->item.incidents = true;

        autoIncidentinventory(node->item.name, node->item.internalCode);
    }

    FILE *logFile = fopen("data/log_monitorizacao.txt", "a");
    if (logFile != NULL)
    {
        fprintf(logFile, "[%02d/%02d/%04d %02d:%02d:%02d] ID: %d | IP: %s | Estado: %s\n",
                tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900,
                tm.tm_hour, tm.tm_min, tm.tm_sec,
                node->item.internalCode,
                node->item.ipAddress,
                respondeu ? "SUCESSO" : "FALHA (Estado alterado para Em Falha)");
        fclose(logFile);
    }
}
void exePingGeral()
{
    if (inventoryList == NULL)
    {
        return;
    }

    Node *current = inventoryList;

    while (current != NULL)
    {

        exePingItem(current);

        current = current->next; // Avança para o próximo equipamento
    }
}
static void clearInventory()
{
    Node *current = inventoryList;
    while (current != NULL)
    {
        Node *next = current->next;
        free(current);
        current = next;
    }
    inventoryList = NULL;
}
static void saveInventoryToFile(const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL)
        return;

    int count = 0;
    Node *curr = inventoryList;
    while (curr)
    {
        count++;
        curr = curr->next;
    }

    InventoryItem *items = malloc(sizeof(InventoryItem) * count);
    curr = inventoryList;
    for (int i = count - 1; i >= 0; i--)
    { // Fill backwards
        items[i] = curr->item;
        curr = curr->next;
    }

    fwrite(items, sizeof(InventoryItem), count, file);

    free(items);
    fclose(file);
}

static void loadInventoryFromFile(const char *filename)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
        return;

    clearInventory();
    nextInternalCode = 1;

    InventoryItem tempItem;

    while (fread(&tempItem, sizeof(InventoryItem), 1, file) == 1)
    {
        bool exists = false;
        Node *current = inventoryList;
        while (current != NULL)
        {
            if (current->item.internalCode == tempItem.internalCode)
            {
                exists = true;
                break;
            }
            current = current->next;
        }

        if (!exists)
        {
            Node *newNode = malloc(sizeof(Node));
            if (newNode == NULL)
                break;

            newNode->item = tempItem;
            newNode->next = inventoryList;
            inventoryList = newNode;

            if (tempItem.internalCode >= nextInternalCode)
            {
                nextInternalCode = tempItem.internalCode + 1;
            }
        }
    }

    fclose(file);
}
static void copy(Node *node, InventoryItem *item)
{
    strncpy(node->item.name, item->name, 50);
    strncpy(node->item.brand, item->brand, 50);
    strncpy(node->item.model, item->model, 50);
    strncpy(node->item.ipAddress, item->ipAddress, 16);
    strncpy(node->item.macAddress, item->macAddress, 18);
    strncpy(node->item.location, item->location, 100);
    strncpy(node->item.lastMaintenanceDate, item->lastMaintenanceDate, 11);

    node->item.type = item->type;
    node->item.status = item->status;
}
static void saveItem(InventoryItem *item, DropdownVar *drop)
{
    item->type = (EquipmentType)drop->type;
    item->status = (EquipmentStatus)drop->status;
    if (isEditing && nodeBeingEdited != NULL)
    {
        copy(nodeBeingEdited, item);
        isEditing = false;
        nodeBeingEdited = NULL;
    }
    else
    {
        Node *newNode = (Node *)malloc(sizeof(Node));

        newNode->item.internalCode = nextInternalCode++;

        copy(newNode, item);

        newNode->next = inventoryList;
        inventoryList = newNode;

        showAddItemDialog = false;

        *item = emptyState;
    }
}

static void showModal(InventoryItem *item, DropdownVar *drop)
{
    Rectangle dialogBounds = {GetScreenWidth() / 4, GetScreenHeight() / 4, GetScreenWidth() / 2, GetScreenHeight() / 2};
    Rectangle contentDialog = {0, 0, dialogBounds.width - 20, dialogBounds.height < 355 ? 500 : dialogBounds.height - 32};
    GuiScrollPanel(dialogBounds, isEditing ? "Modificar Item" : "Adicionar Item", contentDialog, &dialogScroll.scroll, &dialogScroll.view);

    BeginScissorMode(dialogBounds.x, dialogBounds.y + 24, dialogBounds.width, dialogBounds.height - 32);

    const char *placeholders[7] = {"Nome", "Marca", "Modelo", "IP", "MAC", "Localização", "Ultima Verificação"};

    for (int i = 0; i < 10; i++)
    {
        Rectangle textBoxBounds = {dialogBounds.x + 20, dialogBounds.y + 40 + (i * 45) + dialogScroll.scroll.y, dialogBounds.width - 60, 35};

        switch (i)
        {
        case 0:
            drawTextBoxWithPlaceholder(textBoxBounds, item->name, 50, &editModes[0], placeholders[0]);
            break;
        case 1:
            drawTextBoxWithPlaceholder(textBoxBounds, item->brand, 50, &editModes[1], placeholders[1]);
            break;
        case 2:
            break;
        case 3:
            if (editModes[2])
            {
                break;
            }
            drawTextBoxWithPlaceholder(textBoxBounds, item->model, 50, &editModes[3], placeholders[2]);
            break;
        case 4:
            if (editModes[2])
            {
                break;
            }
            drawTextBoxWithPlaceholder(textBoxBounds, item->ipAddress, 16, &editModes[4], placeholders[3]);
            break;
        case 5:
            if (editModes[2])
            {
                break;
            }
            drawTextBoxWithPlaceholder(textBoxBounds, item->macAddress, 18, &editModes[5], placeholders[4]);
            break;
        case 6:
            if (editModes[2])
            {
                break;
            }
            drawTextBoxWithPlaceholder(textBoxBounds, item->location, 100, &editModes[6], placeholders[5]);
            break;
        case 7:
            break;
        case 8:
            if (editModes[2] || editModes[7])
            {
                break;
            }
            drawTextBoxWithPlaceholder(textBoxBounds, item->lastMaintenanceDate, 11, &editModes[8], placeholders[6]);
            break;
        case 9:
            if (editModes[2] || editModes[7])
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
    if (editModes[2])
    {
        EndScissorMode();
    }
    Rectangle dropdownTypeBounds = {dialogBounds.x + 20, dialogBounds.y + 40 + (2 * 45) + dialogScroll.scroll.y, dialogBounds.width - 60, 35};
    if (GuiDropdownBox(dropdownTypeBounds, "Router;Switch;Access Point;Server;NAS;Printer;Camera;Sensor;UPS", &drop->type, editModes[2]))
    {
        editModes[2] = !editModes[2];
    }
    if (editModes[7])
    {
        EndScissorMode();
    }
    Rectangle dropdownStatusBounds = {dialogBounds.x + 20, dialogBounds.y + 40 + (7 * 45) + dialogScroll.scroll.y, dialogBounds.width - 60, 35};
    if (!editModes[2])
    {
        if (GuiDropdownBox(dropdownStatusBounds, "Operacional;Em Manutenção; Em Falha; Desativado", &drop->status, editModes[7]))
        {
            editModes[7] = !editModes[7];
        }
    }
    if (!editModes[2] || !editModes[7])
    {
        EndScissorMode();
    }
}
void drawInventory(float fontSize, float iconScale, int AddIconId, int MinusIconId, int UploadIconId, int DownloadIconId, int PingIconId, int PingNetworkIconId, Rectangle AddIconRect, Rectangle DownloadIconRect, Rectangle UploadIconRect, Rectangle PingNetworkRect, Rectangle bounds)
{

    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize > 22 ? 22 : fontSize);

    if (showAddItemDialog && !isEditing)
    {
        GuiDrawIcon(MinusIconId, AddIconRect.x, AddIconRect.y, iconScale == 1 ? 1 : 2, BLACK);
    }
    else if (!showAddItemDialog)
    {
        newDrop.type = 0;
        newDrop.status = 0;

        GuiDrawIcon(AddIconId, AddIconRect.x, AddIconRect.y, iconScale == 1 ? 1 : 2, BLACK);
    }
    if (CheckCollisionPointRec(GetMousePosition(), AddIconRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !isEditing)
    {
        showAddItemDialog = !showAddItemDialog;
    }
    if (drawIconWcollisions(UploadIconId, iconScale == 1 ? 1 : 2, UploadIconRect))
    {
        loadInventoryFromFile("data/equipamentos.dat");
    }
    if (drawIconWcollisions(DownloadIconId, iconScale == 1 ? 1 : 2, DownloadIconRect))
    {
        saveInventoryToFile("data/equipamentos.dat");
    }
    if (drawIconWcollisions(PingNetworkIconId, iconScale == 1 ? 1 : 2, PingNetworkRect))
    {
        exePingGeral();
    }

    float fontSizeForButtons = fontSize > 22 ? 22 + 5 : fontSize + 5;
    float searchLenght = MeasureText("Pesquisar...", fontSize > 22 ? 22 : fontSize);
    float typeLenght = MeasureText("ACCESS_POINT", fontSize > 22 ? 22 : fontSize);
    float statusLenght = MeasureText("OPERACIONAL", fontSize > 22 ? 22 : fontSize);
    bool isNotBullied = (bounds.width - typeLenght - statusLenght - 10) > searchLenght;
    float searchBullyDefender = isNotBullied ? bounds.width - typeLenght - statusLenght - 10 : 0.34 * bounds.width - 10;

    Rectangle searchBounds = {bounds.x, bounds.y - fontSizeForButtons - 5, searchBullyDefender, fontSizeForButtons};
    Rectangle typeBounds = {searchBounds.x + searchBounds.width + 5, searchBounds.y, isNotBullied ? typeLenght : 0.33 * bounds.width, fontSizeForButtons};
    Rectangle statusBounds = {typeBounds.x + typeBounds.width + 5, searchBounds.y, isNotBullied ? statusLenght : 0.33 * bounds.width, fontSizeForButtons};
    Rectangle content = {0, 0, 500, 500};
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

    drawTextBoxWithPlaceholder(searchBounds, searchText, 50, &searchEdit, "Pesquisar...");

    Node *current = inventoryList;

    GuiScrollPanel(bounds, "Inventario", content, &itemsScroll.scroll, &itemsScroll.view);
    // panel title 24
    BeginScissorMode(bounds.x, bounds.y + 24, bounds.width, bounds.height - 24);

    float modificarLenght = MeasureText("Modificar", fontSize > 22 ? 22 : fontSize);
    float deletarLenght = MeasureText("Deletar", fontSize > 22 ? 22 : fontSize);

    for (int i = 0; current != NULL; i++)
    {
        bool matchesCode = strlen(searchText) > 0 && atoi(searchText) == current->item.internalCode;
        bool matchesSearch = strlen(searchText) == 0 || strstr(current->item.name, searchText) != NULL || strstr(current->item.ipAddress, searchText) != NULL || strstr(current->item.macAddress, searchText) != NULL || matchesCode;
        bool matchesType = filterType == 0 || current->item.type == (EquipmentType)(filterType - 1);
        bool matchesStatus = filterStatus == 0 || current->item.status == (EquipmentStatus)(filterStatus - 1);

        if (!(matchesSearch && matchesType && matchesStatus))
        {
            i--;
            current = current->next;
            continue;
        }

        GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        Rectangle itemBounds = {bounds.x + 30, bounds.y + 40 + (i * 40) + itemsScroll.scroll.y, bounds.width - 70 - 26 - modificarLenght - deletarLenght, fontSizeForButtons};
        Rectangle pingButtonBounds = {bounds.x + bounds.width - 76 - modificarLenght - deletarLenght, itemBounds.y, 16, fontSizeForButtons};
        Rectangle modifyButtonBounds = {bounds.x + bounds.width - 50 - modificarLenght - deletarLenght, itemBounds.y, modificarLenght + 10, fontSizeForButtons};
        Rectangle deleteButtonBounds = {bounds.x + bounds.width - 30 - deletarLenght, itemBounds.y, deletarLenght + 10, fontSizeForButtons};

        char displayText[100];

        snprintf(displayText, sizeof(displayText),
                 "%d - %s",
                 current->item.internalCode,
                 current->item.name);

        GuiLabel(itemBounds, displayText);
        if (GuiButton(pingButtonBounds, TextFormat("#%d#", PingIconId)))
        {
            exePingItem(current);
        }

        if (GuiButton(modifyButtonBounds, "Modificar") && !showAddItemDialog && !filterStatusEdit && !filterTypeEdit)
        {
            isEditing = true;
            nodeBeingEdited = current;
            modifiableItem = current->item;

            modDrop.type = (int)current->item.type;
            modDrop.status = (int)current->item.status;
        }
        if (current->item.incidents == true)
        {
            drawDisabledButton(deleteButtonBounds, "Deletar");
        }
        else if (GuiButton(deleteButtonBounds, "Deletar") && !showAddItemDialog && !filterStatusEdit && !filterTypeEdit)
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

    if (GuiDropdownBox(typeBounds, "Todos;Router;Switch;Access Point;Server;NAS;Printer;Camera;Sensor;UPS", &filterType, filterTypeEdit))
        filterTypeEdit = !filterTypeEdit;

    if (GuiDropdownBox(statusBounds, "Todos;Operacional;Em Manutenção;Em Falha;Desativado", &filterStatus, filterStatusEdit))
        filterStatusEdit = !filterStatusEdit;

    if (showAddItemDialog && !isEditing)
    {

        showModal(&newItem, &newDrop);
    }
    if (!showAddItemDialog && isEditing)
    {
        showModal(&modifiableItem, &modDrop);
    }
}
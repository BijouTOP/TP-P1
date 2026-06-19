#include "sensores.h"
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
#include <curl/curl.h>

typedef struct
{
    char *payload;
    size_t size;
} ResponseBuffer;

static SensorNode *selectedSensorNode = NULL;
static SensorNode *sensorsList = NULL;

static ScrollVars sensorsScroll = {
    .scroll = {0, 0},
    .view = {0}};

static char searchText[50] = "";
static bool searchEdit = false;

static int filterStatus = 0;
static bool filterStatusEdit = false;

void freeSensorsList(void);
sensorStatus stringToStatus(const char *statusStr);
void processarTextoSensores(const char *textData, const char *origemLog);

void freeSensorsList(void)
{
    SensorNode *current = sensorsList;
    while (current != NULL)
    {
        SensorNode *next = current->next;
        free(current);
        current = next;
    }
    sensorsList = NULL;
}

sensorStatus stringToStatus(const char *statusStr)
{
    if (strcmp(statusStr, "FALHA_REDE") == 0)
        return FALHA_REDE;
    if (strcmp(statusStr, "CRITICO") == 0)
        return CRITICO;
    if (strcmp(statusStr, "AVISO") == 0)
        return AVISO;
    return NORMAL;
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    ResponseBuffer *mem = (ResponseBuffer *)userp;

    char *ptr = realloc(mem->payload, mem->size + realsize + 1);
    if (ptr == NULL)
        return 0;

    mem->payload = ptr;
    memcpy(&(mem->payload[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->payload[mem->size] = 0;

    return realsize;
}

void processarTextoSensores(const char *textData, const char *origemLog)
{
    if (!textData)
        return;

    freeSensorsList();

    FILE *logFile = fopen("data/log_sensores.txt", "a");
    time_t agora = time(NULL);
    char *dataHora = ctime(&agora);
    if (dataHora)
        dataHora[strlen(dataHora) - 1] = '\0';

    if (logFile)
        fprintf(logFile, "--- IMPORTACAO VIA %s EM: %s ---\n", origemLog, dataHora ? dataHora : "---");

    char *dataCopy = strdup(textData);
    if (!dataCopy)
    {
        if (logFile)
            fclose(logFile);
        return;
    }

    char *line = strtok(dataCopy, "\r\n");
    SensorNode *tail = NULL;

    while (line != NULL)
    {
        if (strlen(line) == 0 || line[0] == '#' || strstr(line, "codigo_sensor") != NULL)
        {
            line = strtok(NULL, "\r\n");
            continue;
        }

        SensorNode *newNode = (SensorNode *)malloc(sizeof(SensorNode));
        if (newNode == NULL)
            break;
        newNode->next = NULL;

        char statusStr[20] = "";
        int parsed = sscanf(line, "%15[^;];%15[^;];%f;%7[^;];%19s",
                            newNode->sensor.equipmentCode,
                            newNode->sensor.type,
                            &newNode->sensor.value,
                            newNode->sensor.unit,
                            statusStr);

        if (parsed == 5)
        {
            newNode->sensor.status = stringToStatus(statusStr);

            if (sensorsList == NULL)
                sensorsList = newNode;
            else
                tail->next = newNode;
            tail = newNode;

            if (logFile)
                fprintf(logFile, "[IMPORTADO] Code: %s | Status: %s\n", newNode->sensor.equipmentCode, statusStr);

            if (newNode->sensor.status == AVISO || newNode->sensor.status == CRITICO || newNode->sensor.status == FALHA_REDE)
            {
                autoIncidentinventory(newNode->sensor.equipmentCode, -1);
                if (logFile)
                    fprintf(logFile, " >> [INCIDENTE GERADO] Para %s devido ao estado %s\n", newNode->sensor.equipmentCode, statusStr);
            }
        }
        else
        {
            free(newNode);
        }

        line = strtok(NULL, "\r\n");
    }

    free(dataCopy);
    if (logFile)
    {
        fprintf(logFile, "--------------------------------------\n\n");
        fclose(logFile);
    }
}

bool importarSensoresFicheiro(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        TraceLog(LOG_WARNING, "Nao foi possivel abrir o ficheiro %s", filename);
        return false;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(length + 1);
    if (buffer)
    {
        size_t readBytes = fread(buffer, 1, length, file);
        buffer[readBytes] = '\0';
        processarTextoSensores(buffer, "FICHEIRO LOCAL");
        free(buffer);
    }

    fclose(file);
    return true;
}

bool importarSensoresAPI(void)
{
    CURL *curl_handle;
    CURLcode res;
    ResponseBuffer chunk = {.payload = malloc(1), .size = 0};

    if (chunk.payload == NULL)
    {
        TraceLog(LOG_ERROR, "[CURL DEBUG] Falha ao alocar memoria inicial para o buffer.");
        return false;
    }

    TraceLog(LOG_INFO, "[CURL DEBUG] A inicializar cURL...");
    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    if (!curl_handle)
    {
        TraceLog(LOG_ERROR, "[CURL DEBUG] Nao foi possivel inicializar o easy handle.");
        free(chunk.payload);
        return false;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, "https://sensorlab.innominatum.pt/v1/sensors/export/legacy");
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    TraceLog(LOG_INFO, "[CURL DEBUG] A disparar o pedido HTTP para o endpoint...");
    res = curl_easy_perform(curl_handle);

    bool sucesso = false;
    if (res == CURLE_OK)
    {
        TraceLog(LOG_INFO, "[CURL DEBUG] Pedido efetuado com sucesso! Bytes recebidos: %zu", chunk.size);

        if (chunk.size > 0)
        {
            char debugSnippet[151];
            strncpy(debugSnippet, chunk.payload, 150);
            debugSnippet[150] = '\0';
            TraceLog(LOG_INFO, "[CURL DEBUG] Inicio do payload recebido:\n%s\n[...]", debugSnippet);
        }

        processarTextoSensores(chunk.payload, "API ENDPOINT");
        sucesso = true;
    }
    else
    {
        TraceLog(LOG_ERROR, "[CURL DEBUG] O cURL falhou! Código do Erro: %d - %s", res, curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl_handle);
    free(chunk.payload);
    curl_global_cleanup();

    return sucesso;
}
void guardarSensoresFicheiro(const char *filename)
{
    if (sensorsList == NULL)
        return;

    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        TraceLog(LOG_WARNING, "Nao foi possivel abrir o ficheiro %s para escrita", filename);
        return;
    }

    const char *statusStr[] = {"FALHA_REDE", "CRITICO", "NORMAL", "AVISO"};

    SensorNode *current = sensorsList;
    while (current != NULL)
    {
        fprintf(file, "%s;%s;%.1f;%s;%s\n",
                current->sensor.equipmentCode,
                current->sensor.type,
                current->sensor.value,
                current->sensor.unit,
                statusStr[current->sensor.status]);

        current = current->next;
    }

    fclose(file);
    TraceLog(LOG_INFO, "Dados dos sensores guardados com sucesso em %s", filename);
}
void drawSensors(float fontSize, int InfoIconId, Rectangle bounds)
{
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize > 22 ? 22 : fontSize);

    float fontSizeForButtons = fontSize > 22 ? 22 + 5 : fontSize + 5;
    float statusLenght = MeasureText("FALHA_REDE", fontSize > 22 ? 22 : fontSize);

    Rectangle searchBounds = {bounds.x, bounds.y - fontSizeForButtons - 5, bounds.width - statusLenght - 5, fontSizeForButtons};
    Rectangle statusBounds = {searchBounds.x + searchBounds.width + 5, searchBounds.y, statusLenght, fontSizeForButtons};

    Rectangle content = {0, 0, bounds.width - 16, 500};
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
    drawTextBoxWithPlaceholder(searchBounds, searchText, 50, &searchEdit, "Pesquisar...");
    SensorNode *current = sensorsList;

    GuiScrollPanel(bounds, "Sensores", content, &sensorsScroll.scroll, &sensorsScroll.view);
    BeginScissorMode(bounds.x, bounds.y + 24, bounds.width, bounds.height - 24);

    int visibleIndex = 0;
    while (current != NULL)
    {
        bool matchesSearch = strlen(searchText) == 0 ||
                             strstr(current->sensor.equipmentCode, searchText) != NULL ||
                             strstr(current->sensor.type, searchText) != NULL;
        bool matchesStatus = filterStatus == 0 ||
                             current->sensor.status == (sensorStatus)(filterStatus - 1);

        if (!(matchesSearch && matchesStatus))
        {
            current = current->next;
            continue;
        }

        GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        Rectangle itemBounds = {bounds.x + 10, bounds.y + 40 + (visibleIndex * 40) + sensorsScroll.scroll.y, bounds.width - 66, fontSizeForButtons};
        Rectangle showinfo = {bounds.x + bounds.width - 46, itemBounds.y, 24, fontSizeForButtons};

        char displayText[100];
        snprintf(displayText, sizeof(displayText), "%s | %s", current->sensor.equipmentCode, current->sensor.type);
        GuiLabel(itemBounds, displayText);

        if (GuiButton(showinfo, TextFormat("#%d#", InfoIconId)))
        {
            selectedSensorNode = (selectedSensorNode == current) ? NULL : current;
        }

        visibleIndex++;
        current = current->next;
    }

    EndScissorMode();

    if (selectedSensorNode != NULL)
    {
        const char *statusStr[] = {"FALHA_REDE", "CRITICO", "NORMAL", "AVISO"};

        char infoText[400];
        snprintf(infoText, sizeof(infoText),
                 "Codigo: %s\nTipo: %s\nValor Atual: %.2f %s\nEstado: %s",
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

    if (GuiDropdownBox(statusBounds, "Todos;Falha de Rede;Critico;Normal;Aviso", &filterStatus, filterStatusEdit))
        filterStatusEdit = !filterStatusEdit;
}
// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raylib.h"
#include "raygui.h"

#pragma GCC diagnostic pop

typedef enum
{
    INCIDENTE_PENDENTE,
    INCIDENTE_EM_CURSO,
    INCIDENTE_CONCLUIDO
} IncidentStatus;

typedef enum
{
    PRIORIDADE_BAIXA,
    PRIORIDADE_MEDIA,
    PRIORIDADE_ALTA
} IncidentPriority;

typedef struct Incident
{
    int id;

    char equipmentCode[50];
    char type[50];
    char description[200];

    IncidentPriority priority;

    char createdAt[17];
    char concludedAt[17];

    char technician[50];

    IncidentStatus status;
} Incident;

typedef struct IncidentNode
{
    Incident incident;
    struct IncidentNode *next;
} IncidentNode;
typedef struct DropdownVarIncidentes
{
    int status;
    int priority;
} DropdownVarIncidentes;

void drawIncidentes(float fontSize, float iconScale, int AddIconId, int MinusIconId, int UploadIconId, int DownloadIconId, Rectangle AddIconRect, Rectangle DownloadIconRect, Rectangle UploadIconRect, Rectangle bounds);
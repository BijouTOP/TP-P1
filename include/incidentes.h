#include "raylib.h"
#include "raygui.h"

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
    int equipmentId;
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

void drawIncidentes(float fontSize, float iconScale, int AddIconId, int MinusIconId, int UploadIconId, int DownloadIconId, int InfoIconId, Rectangle AddIconRect, Rectangle DownloadIconRect, Rectangle UploadIconRect, Rectangle bounds);
void autoIncidentinventory(char *equipamento, int equipmentId);
IncidentNode *getIncidentList(void);
void loadIncidentsFromFile(const char *filename);
void saveIncidentsToFile(const char *filename);
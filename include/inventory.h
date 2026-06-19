#include "raylib.h"

// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raygui.h"

#pragma GCC diagnostic pop

typedef enum
{
    ROUTER,
    SWITCH,
    ACCESS_POINT,
    SERVER,
    NAS,
    IMPRESSORA,
    CAMARA,
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
    bool incidents;
} InventoryItem;
typedef struct Node
{
    InventoryItem item;
    struct Node *next;
} Node;
typedef struct DropdownVar
{
    int type;
    int status;
} DropdownVar;

void drawInventory(float fontSize, float iconScale, int AddIconId, int MinusIconId, int UploadIconId, int DownloadIconId, int PingiconId, int PingNetworkIconId, Rectangle AddIconRect, Rectangle DownloadIconRect, Rectangle UploadIconRect, Rectangle PingNetworkRect, Rectangle bounds);
void linkIncidenteEquipamento(int equipmentId);
Node *getInventoryList(void);
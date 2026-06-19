#include "raylib.h"
#include "raygui.h"

typedef struct RegisterItem
{
    char equipmentCode[50];
    char configType[50];
    char previousValue[50];
    char newValue[50];
    char dateTime[17];
    char technician[50];
} RegisterItem;

typedef struct RegisterNode
{
    RegisterItem registerItem;
    struct RegisterNode *next;
} RegisterNode;

void drawRegisters(float fontSize, float iconScale, int UndoIconId, int RedoIconId, int UploadIconId, int DownloadIconId, int InfoIconId, Rectangle UndoIconRect, Rectangle RedoIconRect, Rectangle DownloadIconRect, Rectangle UploadIconRect, Rectangle bounds);
void registerNewConfig(RegisterItem *item);
void clearRegisters();
void loadRegistersFromFile(const char *filename);
void saveRegistersToFile(const char *filename);
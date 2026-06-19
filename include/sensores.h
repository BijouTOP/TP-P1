#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raylib.h"
#include "raygui.h"

#pragma GCC diagnostic pop

typedef enum
{
    FALHA_REDE,
    CRITICO,
    NORMAL,
    AVISO
} sensorStatus;

typedef struct Sensor
{
    char equipmentCode[16];
    char type[16];
    float value;
    char unit[8];
    sensorStatus status;
} Sensor;
typedef struct SensorNode
{
    Sensor sensor;
    struct SensorNode *next;
} SensorNode;
void drawSensors(float fontSize, int InfoIconId, Rectangle bounds);
bool importarSensoresFicheiro(const char *filename);
bool importarSensoresAPI(void);
void guardarSensoresFicheiro(const char *filename);
void freeSensorsList(void);
#include "menus.h"
#include "reports.h"

#include "raylib.h"

// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raygui.h"

#pragma GCC diagnostic pop

int drawMainMenu(float fontSize)
{
    const float containerPadding = 0.24f;
    const float spacing = 0.08f * GetScreenHeight();

    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

    const char *menuLabels[6] = {
        "Inventário de Equipamentos da Rede",
        "Monitorização de Sensores",
        "Incidentes Técnicos",
        "Registo de Configurações",
        "Relatório Estado Da Rede",
        "Relatório Mensal"};
    for (int i = 0; i < 6; i++)
    {
        // GuiLabel((Rectangle){containerBounds.x, containerBounds.y + (i * spacing), containerBounds.width, 24}, menuLabels[i]);
        if (GuiLabelButton((Rectangle){GetScreenWidth() / 2 - ((MeasureText(menuLabels[i], fontSize)) / 2.0f), containerPadding * GetScreenHeight() + (i * spacing), MeasureText(menuLabels[i], (int)fontSize), fontSize}, menuLabels[i]))
        {
            switch (i)
            {
            case 0:
                return 1;
                break;
            case 1:
                return 2;
                break;
            case 2:
                return 3;
                break;
            case 3:
                return 4;
                break;
            case 4:
                gerarRelatorioEstadoRede();

                break;
            case 5:
                gerarRelatorioMensalIncidentes();
                break;
            default:
                return 0;
                break;
            }
        }
    }
    return 0;
}
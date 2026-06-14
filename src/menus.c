#include "menus.h"

#include "raylib.h"

// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raygui.h"

#pragma GCC diagnostic pop

int closeWindow()
{
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

int drawMainMenu(float fontSize)
{
    const float containerPadding = 0.24f;
    const float spacing = 0.08f * GetScreenHeight();

    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

    const char *menuLabels[7] = {"Inventário de Equipamentos da Rede", "Testes de Conectividade", "Monitorização de Sensores", "Incidentes Técnicos", "Registo de Configurações", "Relatórios Técnicos"};
    for (int i = 0; i < 7; i++)
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
                return 5;
                break;
            case 5:
                return 6;
                break;
            default:
                return 0;
                break;
            }
        }
    }
    return 0;
}
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main()
{
    InitWindow(800, 400, "Mini NOC");

    // Start Music
    InitAudioDevice();
    Music music = LoadMusicStream("media/music.ogg");
    SetMusicVolume(music, 0.2);
    PlayMusicStream(music);
    PauseMusicStream(music);
    int MusicState = 0; // 0 - Stopped, 1 - Playing

    SetTargetFPS(60);

    // Custom icon made with rguiicons, https://raylibtech.itch.io/rguiicons
    unsigned int AudioMutedIcon[8] = {0x00000000, 0x00a000c0, 0x00880090, 0x00820086, 0x00860082, 0x00900088, 0x00c000a0, 0x00000000};

    // Main application loop
    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        // Load custom icon into raygui internal array
        for (int i = 0; i < 8; i++)
        {
            guiIcons[123 * 8 + i] = AudioMutedIcon[i];
        }

        // Define icons rectangles
        Rectangle FullscreenIcon = {GetScreenWidth() - 64, 32, 32, 32};
        Rectangle MusicIcon = {GetScreenWidth() - 64, GetScreenHeight() - 64, 32, 32};

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        // Draw icons and check for mouse interaction
        GuiDrawIcon(107, FullscreenIcon.x, FullscreenIcon.y, 2, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), FullscreenIcon) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            ToggleFullscreen();
        }
        if (MusicState == 0)
        {
            GuiDrawIcon(123, MusicIcon.x, MusicIcon.y, 2, BLACK);
        }
        else
        {
            GuiDrawIcon(122, MusicIcon.x, MusicIcon.y, 2, BLACK);
        }
        if (CheckCollisionPointRec(GetMousePosition(), MusicIcon) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (MusicState == 0)
            {
                ResumeMusicStream(music);
                MusicState = 1;
            }
            else
            {
                PauseMusicStream(music);
                MusicState = 0;
            }
        }

        // Draw main menu

        // Default fontsize is 10
        GuiSetStyle(DEFAULT, TEXT_SIZE, 20);                   // To reset call same function with default value (10)
        GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER); // Center text in label control

        Rectangle containerBounds = {96 * 2, 32 * 3, GetScreenWidth() - 96 * 4, GetScreenHeight() - 32 * 6};
        const char *menuLabels[6] = {"Inventário de Equipamentos da Rede", "Testes de Conectividade", "Monitorização de Sensores", "Incidentes Técnicos", "Registo de Configurações", "Relatórios Técnicos"};
        for (int i = 0; i < 6; i++)
        {
            GuiLabel((Rectangle){containerBounds.x, containerBounds.y + i * 24, containerBounds.width, 24}, menuLabels[i]);
        }

        EndDrawing();
    }

    // De-Initialization
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
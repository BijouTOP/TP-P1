#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define DEFAULTWIDTH 800
#define DEFAULTHEIGHT 400

#define MINIMUMWIDTH 362
#define MINIMUMHEIGHT 200

#define MUSICPATH "media/music.ogg"

#define MUSIC_VOLUME 0.2f

#define AUDIO_UNMUTED_ICON_ID 122
#define AUDIO_MUTED_ICON_ID 123

#define FRAME_RATE 60

int closeWindow()
{
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
int main()
{
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(DEFAULTWIDTH, DEFAULTHEIGHT, "Mini NOC");
    SetWindowMinSize(MINIMUMWIDTH, MINIMUMHEIGHT);
    // Start Music
    InitAudioDevice();
    Music music = LoadMusicStream(MUSICPATH);
    SetMusicVolume(music, MUSIC_VOLUME);
    PlayMusicStream(music);
    PauseMusicStream(music);
    int MusicState = 0; // 0 - Stopped, 1 - Playing

    SetTargetFPS(FRAME_RATE);

    // Custom icon made with rguiicons, https://raylibtech.itch.io/rguiicons
    unsigned int AudioMutedIcon[8] = {0x00000000, 0x00a000c0, 0x00880090, 0x00820086, 0x00860082, 0x00900088, 0x00c000a0, 0x00000000};

    // Main application loop
    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        // Load custom icon into raygui internal array
        for (int i = 0; i < 8; i++)
        {
            guiIcons[AUDIO_MUTED_ICON_ID * 8 + i] = AudioMutedIcon[i];
        }

        // Define icons rectangles

        // Using GetScreenHeight() / 200.0f because it is the scale factor for icons.
        const int iconScale = GetScreenHeight() < GetScreenWidth() ? GetScreenHeight() / 200 : GetScreenWidth() / 200;
        const float iconSize = 16 * iconScale;
        const float paddingAccountingForIcon = 32 * iconScale;

        DrawRectangleLinesEx((Rectangle){GetScreenWidth() - paddingAccountingForIcon, iconSize, iconSize, iconSize}, 1, BLUE);
        DrawRectangleLinesEx((Rectangle){GetScreenWidth() - paddingAccountingForIcon, GetScreenHeight() - paddingAccountingForIcon, iconSize, iconSize}, 1, BLUE);
        Rectangle FullscreenIcon = {GetScreenWidth() - paddingAccountingForIcon, iconSize, iconSize, iconSize};
        Rectangle MusicIcon = {GetScreenWidth() - paddingAccountingForIcon, GetScreenHeight() - paddingAccountingForIcon, iconSize, iconSize};

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        // Draw icons and check for mouse interaction
        GuiDrawIcon(107, FullscreenIcon.x, FullscreenIcon.y, iconScale, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), FullscreenIcon) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            ToggleFullscreen();
        }
        if (MusicState == 0)
        {
            GuiDrawIcon(AUDIO_MUTED_ICON_ID, MusicIcon.x, MusicIcon.y, iconScale, BLACK);
        }
        else
        {
            GuiDrawIcon(AUDIO_UNMUTED_ICON_ID, MusicIcon.x, MusicIcon.y, iconScale, BLACK);
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

        // To reset call same function with default value (10)
        float fontSize = 0.05 * GetScreenHeight();

        // Draw main menu
        if (GetScreenWidth() > GetScreenHeight())
        {
            fontSize = 0.05 * GetScreenHeight();
        }
        else
        {
            fontSize = 0.05 * GetScreenWidth();
        }

        const float containerPadding = 0.24f;
        const float spacing = 0.08f * GetScreenHeight();

        // Default fontsize is 10

        GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
        GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        const char *menuLabels[7] = {"Inventário de Equipamentos da Rede", "Testes de Conectividade", "Monitorização de Sensores", "Incidentes Técnicos", "Registo de Configurações", "Relatórios Técnicos", "Exit"};
        for (int i = 0; i < 7; i++)
        {
            // GuiLabel((Rectangle){containerBounds.x, containerBounds.y + (i * spacing), containerBounds.width, 24}, menuLabels[i]);
            if (GuiLabelButton((Rectangle){GetScreenWidth() / 2 - ((MeasureText(menuLabels[i], fontSize)) / 2.0f), containerPadding * GetScreenHeight() + (i * spacing), MeasureText(menuLabels[i], (int)fontSize), fontSize}, menuLabels[i]))
            {
                switch (i)
                {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    break;
                case 4:
                    break;
                case 5:
                    break;
                case 6:
                    return closeWindow();
                default:
                    break;
                }
            }
        }

        EndDrawing();
    }

    // De-Initialization
    return closeWindow();
}
#include "menus.h"

#include "raylib.h"

// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raygui.h"

#pragma GCC diagnostic pop

#define DEFAULTWIDTH 800
#define DEFAULTHEIGHT 400

#define MINIMUMWIDTH 362
#define MINIMUMHEIGHT 200

#define MUSICPATH "media/music.ogg"

#define MUSIC_VOLUME 0.2f

#define AUDIO_UNMUTED_ICON_ID 122
#define AUDIO_MUTED_ICON_ID 123

#define FRAME_RATE 60

int MusicState = 0; // 0 - Stopped, 1 - Playing
int menu = 0;       // 0 - Main Menu, 1 - Inventory, 2 - Connectivity Tests, 3 - Sensor Monitoring, 4 - Technical Incidents, 5 - Configuration Records, 6 - Technical Reports

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

    SetTargetFPS(FRAME_RATE);

    // Inside main()
    unsigned int *guiIcons = GetGuiIcons();

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

        Rectangle FullscreenIcon = {GetScreenWidth() - paddingAccountingForIcon, iconSize, iconSize, iconSize};
        Rectangle ExitIcon = {GetScreenWidth() - paddingAccountingForIcon, GetScreenHeight() - paddingAccountingForIcon, iconSize, iconSize};
        Rectangle MusicIcon = {GetScreenWidth() - 1.5 * paddingAccountingForIcon, GetScreenHeight() - paddingAccountingForIcon, iconSize, iconSize};
        Rectangle HomeIcon = {paddingAccountingForIcon / 2, GetScreenHeight() - paddingAccountingForIcon, iconSize, iconSize};

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
        GuiDrawIcon(158, ExitIcon.x, ExitIcon.y, iconScale, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), ExitIcon) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            return closeWindow();
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
        GuiDrawIcon(185, HomeIcon.x, HomeIcon.y, iconScale, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), HomeIcon) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            menu = 0;
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

        switch (menu)
        {
        case 0:
            if (drawMainMenu(fontSize) != 0)
            {
                menu = drawMainMenu(fontSize);
            }
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
            break;
        case 7:
            return closeWindow();
            break;
        default:
            break;
        }
        EndDrawing();
    }

    // De-Initialization
    return closeWindow();
}

#include "menus.h"
#include "utils.h"
#include "inventory.h"
#include "incidentes.h"
#include "sensores.h"

#include "raylib.h"

// Disable unused parameter warnings specifically for raygui
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "raygui.h"

#pragma GCC diagnostic pop

#include <math.h>

#include <sys/stat.h>
#include <sys/types.h>

#if defined(_WIN32)
#include <direct.h>
#define CREATE_DIR(path) _mkdir(path)
#else
#define CREATE_DIR(path) mkdir(path, 0777)
#endif

#define DEFAULTWIDTH 1000
#define DEFAULTHEIGHT 600

#define MINIMUMWIDTH 640
#define MINIMUMHEIGHT 480

#define MUSICPATH "media/music.ogg"

#define MUSIC_VOLUME 0.2f

#define FULLSCREEN_ICON_ID 107
#define HOME_ICON_ID 185
#define AUDIO_UNMUTED_ICON_ID 122
#define AUDIO_MUTED_ICON_ID 123
#define EXIT_ICON_ID 158
#define ADD_ICON_ID 220
#define MINUS_ICON_ID 221
#define UPLOAD_ICON_ID 5
#define DOWNLOAD_ICON_ID 6
#define INFO_ICON_ID 140
#define PING_ICON_ID 124
#define PING_NETWORK_ICON_ID 172

#define FRAME_RATE 60

int MusicState = 0; // 0 - Stopped, 1 - Playing
int menu = 0;       // 0 - Main Menu, 1 - Inventory, 2 - Connectivity Tests, 3 - Sensor Monitoring, 4 - Technical Incidents, 5 - Configuration Records, 6 - Technical Reports

void garantirPastaData()
{
    struct stat st = {0};

    if (stat("data", &st) == -1)
    {
        CREATE_DIR("data");
    }
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

    SetTargetFPS(FRAME_RATE);

    garantirPastaData();

    unsigned int *guiIcons = GetGuiIcons();

    // Custom icon made with rguiicons, https://raylibtech.itch.io/rguiicons
    unsigned int AudioMutedIcon[8] = {0x00000000, 0x00a000c0, 0x00880090, 0x00820086, 0x00860082, 0x00900088, 0x00c000a0, 0x00000000};
    unsigned int AddIcon[8] = {0x01800000, 0x01800180, 0x01800180, 0x7ffe0180, 0x01807ffe, 0x01800180, 0x01800180, 0x00000180};
    unsigned int MinusIcon[8] = {0x00000000, 0x00000000, 0x00000000, 0x7ffe0000, 0x00007ffe, 0x00000000, 0x00000000, 0x00000000};

    // Main application loop
    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        // Load custom icon into raygui internal array
        for (int i = 0; i < 8; i++)
        {
            guiIcons[AUDIO_MUTED_ICON_ID * 8 + i] = AudioMutedIcon[i];
        }
        for (int i = 0; i < 8; i++)
        {
            guiIcons[ADD_ICON_ID * 8 + i] = AddIcon[i];
        }
        for (int i = 0; i < 8; i++)
        {
            guiIcons[MINUS_ICON_ID * 8 + i] = MinusIcon[i];
        }

        const int iconScale = GetScreenHeight() < GetScreenWidth() ? GetScreenHeight() / 200 : GetScreenWidth() / 200;
        const float iconSize = 16 * iconScale;
        const float paddingAccountingForIcon = 32 * iconScale;

        Rectangle FullscreenIcon = {GetScreenWidth() - paddingAccountingForIcon, iconSize, iconSize, iconSize};
        Rectangle ExitIcon = {GetScreenWidth() - paddingAccountingForIcon, GetScreenHeight() - paddingAccountingForIcon, iconSize, iconSize};
        Rectangle MusicIcon = {GetScreenWidth() - 1.5 * paddingAccountingForIcon, GetScreenHeight() - paddingAccountingForIcon, iconSize, iconSize};
        Rectangle HomeIcon = {paddingAccountingForIcon / 2, GetScreenHeight() - paddingAccountingForIcon, iconSize, iconSize};

        Rectangle AddIconRect = {iconSize + paddingAccountingForIcon + GetScreenWidth() - paddingAccountingForIcon * 3 + 5, paddingAccountingForIcon + iconSize + (iconScale == 1 ? 4 : -4), iconScale == 1 ? 16 : 16 * 2, iconScale == 1 ? 16 : 16 * 2};
        Rectangle DownloadIconRect = {iconSize + paddingAccountingForIcon + GetScreenWidth() - paddingAccountingForIcon * 3 + 5, AddIconRect.y + (iconScale == 1 ? 16 : 16 * 2) + 15, iconScale == 1 ? 16 : 16 * 2, iconScale == 1 ? 16 : 16 * 2};
        Rectangle UploadIconRect = {iconSize + paddingAccountingForIcon + GetScreenWidth() - paddingAccountingForIcon * 3 + 5, DownloadIconRect.y + (iconScale == 1 ? 16 : 16 * 2) + 15, iconScale == 1 ? 16 : 16 * 2, iconScale == 1 ? 16 : 16 * 2};
        Rectangle PingNetworkIconRect = {iconSize + paddingAccountingForIcon + GetScreenWidth() - paddingAccountingForIcon * 3 + 5, UploadIconRect.y + (iconScale == 1 ? 16 : 16 * 2) + 15, iconScale == 1 ? 16 : 16 * 2, iconScale == 1 ? 16 : 16 * 2};
        Rectangle bounds = {iconSize + paddingAccountingForIcon, iconSize + paddingAccountingForIcon, GetScreenWidth() - paddingAccountingForIcon * 3, GetScreenHeight() - paddingAccountingForIcon * 3};

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        if (drawIconWcollisions(FULLSCREEN_ICON_ID, iconScale, FullscreenIcon))
        {
            ToggleFullscreen();
        }
        if (drawIconWcollisions(EXIT_ICON_ID, iconScale, ExitIcon))
        {
            return closeWindow();
        }
        if (drawIconWcollisions(MusicState == 0 ? AUDIO_MUTED_ICON_ID : AUDIO_UNMUTED_ICON_ID, iconScale, MusicIcon))
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
        if (drawIconWcollisions(HOME_ICON_ID, iconScale, HomeIcon))
        {
            menu = 0;
        }

        float fontSize = 0.05 * GetScreenHeight();

        // Draw main menu
        // if (GetScreenWidth() > GetScreenHeight())
        // {
        //     fontSize = 0.05 * GetScreenHeight();
        // }
        // else
        // {
        //     fontSize = 0.05 * GetScreenWidth();
        // }

        // Calculate the diagonal magnitude
        float diagonal = sqrtf(GetScreenWidth() * GetScreenWidth() + GetScreenHeight() * GetScreenHeight());

        // Scale based on the diagonal (adjust the 0.035 factor to suit your needs)
        fontSize = 0.035f * diagonal;

        switch (menu)
        {
        case 0:
            if (drawMainMenu(fontSize) != 0)
            {
                menu = drawMainMenu(fontSize);
            }
            break;
        case 1:
            drawInventory(fontSize, iconScale, ADD_ICON_ID, MINUS_ICON_ID, UPLOAD_ICON_ID, DOWNLOAD_ICON_ID, PING_ICON_ID, PING_NETWORK_ICON_ID, AddIconRect, DownloadIconRect, UploadIconRect, PingNetworkIconRect, bounds);
            break;
        case 2:
            break;
        case 3:
            drawSensors(fontSize, bounds);
            break;
        case 4:
            drawIncidentes(fontSize, iconScale, ADD_ICON_ID, MINUS_ICON_ID, UPLOAD_ICON_ID, DOWNLOAD_ICON_ID, INFO_ICON_ID, AddIconRect, DownloadIconRect, UploadIconRect, bounds);
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

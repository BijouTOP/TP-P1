#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main()
{
    InitWindow(800, 400, "Mini NOC");
    InitAudioDevice();
    Music music = LoadMusicStream("media/music.ogg");
    SetMusicVolume(music, 0.2);
    PlayMusicStream(music);
    int MusicState = 1; // 0 - Stopped, 1 - Playing
    SetTargetFPS(60);
    unsigned int AudioMutedIcon[8] = {0x00000000, 0x00a000c0, 0x00880090, 0x00820086, 0x00860082, 0x00900088, 0x00c000a0, 0x00000000};

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        Rectangle FullscreenIcon = {GetScreenWidth() - 64, 32, 32, 32};
        Rectangle MusicIcon = {GetScreenWidth() - 64, GetScreenHeight() - 64, 32, 32};
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        GuiDrawIcon(107, FullscreenIcon.x, FullscreenIcon.y, 2, BLACK);
        if (CheckCollisionPointRec(GetMousePosition(), FullscreenIcon) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            ToggleFullscreen();
        }
        for (int i = 0; i < 8; i++)
        {
            guiIcons[123 * 8 + i] = AudioMutedIcon[i];
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
        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
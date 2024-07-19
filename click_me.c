/// A button you can't click
#include <raylib.h>
#include <raymath.h>
#define RAYGUI_IMPLEMENTATION
#include "include/raygui.h"

int main(void)
{
    InitWindow(500, 500, "click me");

    int mon = GetCurrentMonitor();
    int mon_w = GetMonitorWidth(mon), mon_h = GetMonitorHeight(mon);

    int win_w = mon_w / 4, win_h = mon_h / 3;
    int but_w = win_w / 2, but_h = win_h / 2;
    
    Rectangle the_button = (Rectangle){
        .x = (win_w / 2) - (but_w / 2),
        .y = (win_h / 2) - (but_h / 2),
        .width = but_w,
        .height = but_h,
    };

    Rectangle mouse = (Rectangle){
        .x = 0, .y = 0,
        .width = 50,
        .height = 50,
    };

    SetWindowSize(win_w, win_h);
    while (!WindowShouldClose())
    {
    BeginDrawing();
    ClearBackground(RAYWHITE);

        GuiButton(the_button, "Click Me");

        Vector2 mouse_pos = GetMousePosition();

        mouse.x = mouse_pos.x;
        mouse.y = mouse_pos.y;
        if (CheckCollisionRecs(the_button, mouse))
        {
            Vector2 mouse_delta = GetMouseDelta();
            Vector2 mouse_prev = Vector2Subtract(mouse_pos, mouse_delta);
            float dist = Vector2Distance(mouse_pos, mouse_prev);
            
            Vector2 move = Vector2Scale((Vector2){ .x = 10, .y = 10 }, dist);
            Vector2 to   = Vector2Subtract(mouse_pos, move);
            SetMousePosition(to.x, to.y);
        }

    EndDrawing();
    }
}
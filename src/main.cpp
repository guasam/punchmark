/**
 * Raylib Web Starter
 *
 * This project will be usedd for compiling raylib (v3.7.0) for PLATFORM_WEB and PLATFORM_DESKTOP
 * Copyright (c) 2021 Guasam (@guasam)
 **/

#include "raylib.h"
#include <random>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <string>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

#define MAX_PUNCHES 50000
#define MAX_BATCH_ELEMENTS 8192

void UpdateDrawFrame(void);
Texture2D punchTexture;
Font font;
int screenWidth = 414; // 414
int screenHeight = 686; // 736
int rotation = 0;
Vector2 textSize = { 0.0f, 0.0f };
float fontSize = 22.0f;
float fontSpacing = 2.0f; // For correct alignement?
int* fontChars;
const char* headText = "PUNCH MARK";
const char* strictText = "Copyright (c) 2021 Guasam";

typedef struct PUNCH {
    Vector2 position;
    Vector2 speed;
    Color color;
} Punch;

Punch* punches;
int punchesCount = 0;


#if defined(PLATFORM_WEB)
EM_JS(int, get_canvas_width, (), {
return available_screen_width();
    }
);
EM_JS(int, get_canvas_height, (), {
return available_screen_height();
    }
);
#endif

int get_canvas_width();
int get_canvas_height();


extern "C" 
{
    int my_add(int a, int b);
}

int my_add(int a, int b) 
{

    return a + b;
}


//// Random numbers doohickey
//std::random_device randev;
//std::default_random_engine randen(randev());
//
//
//float getRandomFloat(float min, float max)
//{
//    std::uniform_real_distribution<> distr(min, max);
//    std::stringstream ss;
//    ss << std::setprecision(2) << distr(randen);
//    return std::stof(ss.str());
//}   


/// ------------------------------------------
/// Main execution method
/// ------------------------------------------
int main(void)
{
    // Initialize random number generator


    
#if defined(PLATFORM_WEB)
    int canvasWidth = get_canvas_width();
    int canvasHeight = get_canvas_height();

    screenWidth = canvasWidth;
    screenHeight = canvasHeight;

    printf("Checking %d %d", canvasWidth, canvasHeight);
#endif

    InitWindow(screenWidth, screenHeight, "Punch Mark");

    // Load Font
    font = LoadFontEx("resources/NotoSans-Regular.ttf", (int)fontSize, 0, 250);

    // Load punch texture (must be after initializing window to get OpenGL Context)
    punchTexture = LoadTexture("resources/punch.png");

    // Punches array
    punches = (Punch*)malloc(MAX_PUNCHES * sizeof(Punch));

#if defined(PLATFORM_WEB)
    // Must be after initializing window
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else

    SetTargetFPS(60);

    // Main loop
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    free(punches);
    UnloadTexture(punchTexture);
    UnloadFont(font);

    CloseWindow();
    return 0;
}

void UpdateDrawFrame(void)
{
    // Handle inputs gestures
    if (GetGestureDetected() == GESTURE_TAP)
    {
        // Smack more punches
        for (int i = 0; i < 100; i++)
        {
            if (punchesCount < MAX_PUNCHES)
            {
                punches[punchesCount].position = GetMousePosition();
                punches[punchesCount].speed.x = (float)GetRandomValue(-300, 300) * GetFrameTime();
                punches[punchesCount].speed.y = (float)GetRandomValue(-300, 300) * GetFrameTime();
                punches[punchesCount].color = ColorFromHSV((float) GetRandomValue(0, 80), (float) GetRandomValue(0, 1), 1.0f);
                punchesCount++;
            }
        }
    }

    // Update punches
    for (int i = 0; i < punchesCount; i++)
    {
        // Apply speed to punch
        punches[i].position.x += punches[i].speed.x;
        punches[i].position.y += punches[i].speed.y;

        // Screen corner collision check
        if (punches[i].position.x > screenWidth || punches[i].position.x < 0)
            punches[i].speed.x *= -1;
        if (punches[i].position.y > screenHeight || punches[i].position.y < 0)
            punches[i].speed.y *= -1;
    }

    BeginDrawing();
    {
        // Clear canvas (OpenGL)
        ClearBackground(RAYWHITE);

        // Draw text
        textSize = MeasureTextEx(font, headText, fontSize, fontSpacing);
        DrawTextEx(font, headText, Vector2{ (screenWidth - textSize.x) / 2.0f, textSize.y + 20 }, fontSize, fontSpacing, DARKGRAY);

        // Draw text
        textSize = MeasureTextEx(font, strictText, fontSize, fontSpacing);
        DrawTextEx(font, strictText, Vector2{ (screenWidth - textSize.x) / 2.0f, screenHeight - textSize.y - 20 }, fontSize, fontSpacing, DARKGRAY);
        //DrawRectangleLines((screenWidth - textSize.x) / 2.0f, screenHeight - textSize.y - 20, textSize.x, textSize.y, RED);

        // 
        // Increment rotation
        //rotation++;

        // Scale image
        float scale = 0.15f;

        // Render punches
        for (int i = 0; i < punchesCount; i++)
        {
            DrawTexturePro(
                punchTexture,
                Rectangle{ 0, 0, (float)punchTexture.width, (float)punchTexture.height },
                Rectangle{ punches[i].position.x, punches[i].position.y, punchTexture.width * scale, punchTexture.height * scale },
                Vector2{ punchTexture.width / 2.0f * scale, punchTexture.height / 2.0f * scale },
                180,
                punches[i].color
            );

            //DrawCircle((int) punches[i].position.x, (int) punches[i].position.y, 5, MAGENTA);
            //DrawTextEx(font, FormatText("Postiion x : %d y : %d", (int) punches[i].position.x, (int) punches[i].position.y), Vector2{ 20, 20 }, fontSize, 2, RED);
            //DrawTextEx(font, FormatText("Postiion x : %d y : %d", screenWidth, screenHeight), Vector2{ 50, screenHeight - 20.0f }, fontSize, 2, RED);
        }

        DrawRectangle(0, 0, screenWidth, 30, BLACK);
        //DrawText(TextFormat("Punches: %i", punchesCount), 10, 5, 20, RED);
        DrawTextEx(font, TextFormat("Punches: %i", punchesCount), Vector2{ 10, 5 }, fontSize, fontSpacing, WHITE);
        DrawTextEx(font, TextFormat("FPS: %i", GetFPS()), Vector2{ (float) screenWidth - 75, 5 }, fontSize, fontSpacing, GREEN);

    /*    DrawFPS(screenWidth - 85, 5);
        GetFPS*/
    }

    EndDrawing();
}
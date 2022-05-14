#include "raylib.h"
#include <stdio.h>
#include <array>

struct AnimData
{
    Rectangle rec;
    Vector2 vector;
    int velocity;
    int frame;
    float updateTime;
    float runningTime = 0.0;
};

AnimData updateAnimData(AnimData data, float DeltaTime, int maxFrame)
{
    data.runningTime += DeltaTime;
    if (data.runningTime >= data.updateTime)
    {
        data.runningTime = 0.0;
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > maxFrame)
        {
            data.frame = 0;
        }
    }
    return data;
}

AnimData Move(AnimData nebula, int windowDimension[], float DeltaTime)
{
    nebula.vector.x += nebula.velocity * DeltaTime;
    // returns nebula to start position
    // if (nebula.vector.x < (0 - nebula.rec.width))
    // {
    //     nebula.vector.x = windowDimension[1];
    // }
    nebula = updateAnimData(nebula, DeltaTime, 7);
    return nebula;
}

float drawingMovingBackground(Texture2D texture, float coordinateX, float Deltatime, int velocity)
{
    coordinateX += velocity * Deltatime;
    if (coordinateX <= 0 - texture.width * 2)
    {
        coordinateX = 0.0;
    }

    DrawTextureEx(texture, {coordinateX, 0.0}, 0.0, 2.0, WHITE);
    DrawTextureEx(texture, {coordinateX + texture.width * 2, 0.0}, 0.0, 2.0, WHITE);
    DrawTextureEx(texture, {coordinateX + texture.width * 4, 0.0}, 0.0, 2.0, WHITE);
    return coordinateX;
}

bool groundCheckForScarfy(AnimData scarfyData, int windowDimension[])
{
    return scarfyData.vector.y >= windowDimension[0] - scarfyData.rec.height;
}

int main()
{
    // window dimension
    int windowDimension[2] = {380, 700};
    char *window_name = "Alex Game";
    int gravity = 1000;
    const int jump_vel = -600;
    bool jump = false;
    float DeltaTime;
    bool victory = false;
    bool loose = false;
    InitWindow(windowDimension[1], windowDimension[0], window_name);

    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    Texture2D background = LoadTexture("textures/far-buildings.png");
    Texture2D middleground = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");

    float bgX = 0.0;
    float mgX = 0.0;
    float fgX = 0.0;
    int amountOfNebulas = 2;
    AnimData nebulas[amountOfNebulas];
    int iterator = 0;
    for (AnimData nebulaEl : nebulas)
    {
        nebulas[iterator].rec.x = 0.0;
        nebulas[iterator].rec.y = 0.0;
        nebulas[iterator].rec.width = nebula.width / 8;
        nebulas[iterator].rec.height = nebula.height / 8;
        nebulas[iterator].vector.x = windowDimension[1] + 450 * iterator;
        nebulas[iterator].vector.y = windowDimension[0] - nebulas[iterator].rec.height;
        nebulas[iterator].velocity = -230;
        nebulas[iterator].frame = iterator;
        nebulas[iterator].updateTime = 1.0 / 12.0;
        iterator++;
    }

    Rectangle finishLine;
    finishLine.width = 5;
    finishLine.height = windowDimension[1];
    finishLine.x = windowDimension[1] + 450;
    finishLine.y = windowDimension[0] - finishLine.height;

    iterator = 0;
    AnimData scarfyData{{0.0, 0.0, scarfy.width / 6, scarfy.height},
                        {windowDimension[1] / 4, windowDimension[0] - scarfyData.rec.height},
                        0,
                        0,
                        1.0 / 12.0};

    SetTargetFPS(60);

    while (WindowShouldClose() == false)
    {
        DeltaTime = GetFrameTime();
        BeginDrawing();
        ClearBackground(BLACK);

        bgX = drawingMovingBackground(background, bgX, DeltaTime, -20);
        mgX = drawingMovingBackground(middleground, mgX, DeltaTime, -40);
        fgX = drawingMovingBackground(foreground, fgX, DeltaTime, -80);

        if (groundCheckForScarfy(scarfyData, windowDimension))
        {
            scarfyData.velocity = 0;
            jump = false;
            scarfyData.vector.y = windowDimension[0] - scarfyData.rec.height;
        }
        else
        {
            scarfyData.velocity = scarfyData.velocity + gravity * DeltaTime;
        }
        if (IsKeyPressed(KEY_SPACE) && !jump)
        {
            printf("I'm jumping now\n");
            jump = true;
            scarfyData.velocity = scarfyData.velocity + jump_vel;
        }

        scarfyData.vector.y = scarfyData.vector.y + scarfyData.velocity * DeltaTime;
        if (!jump)
        {
            scarfyData = updateAnimData(scarfyData, DeltaTime, 5);
        }
     if (loose)
     {
         DrawText("YOU LOSE", 30, windowDimension[0] - 350, 30, RED);
     }else if (victory)
     {
         DrawText("YOU WIN", 30, windowDimension[0] - 350, 30, GREEN);
     }else {   
        DrawTextureRec(scarfy, scarfyData.rec, scarfyData.vector, WHITE);

        for (AnimData nebEl : nebulas)
        {
            nebulas[iterator] = Move(nebEl, windowDimension, DeltaTime);
            DrawTextureRec(nebula, nebulas[iterator].rec, nebulas[iterator].vector, WHITE);
            if (CheckCollisionRecs({nebulas[iterator].vector.x + 20,
                                    nebulas[iterator].vector.y + 20,
                                    nebulas[iterator].rec.width - 40,
                                    nebulas[iterator].rec.height - 40},
                                   {scarfyData.vector.x + 20,
                                    scarfyData.vector.y + 20,
                                    scarfyData.rec.width - 40,
                                    scarfyData.rec.height - 40}))
            {
                loose = true;
            }

            iterator++;
        }
        if (CheckCollisionRecs(finishLine, {scarfyData.vector.x,
                                            scarfyData.vector.y,
                                            scarfyData.rec.width,
                                            scarfyData.rec.height}))
        {
            victory = true;
        }

        iterator = 0;
        if (nebulas[amountOfNebulas - 1].vector.x <= windowDimension[1])
        {
            finishLine.x += nebulas[amountOfNebulas - 1].velocity * DeltaTime;
            DrawRectangle(finishLine.x, finishLine.y, finishLine.width, finishLine.height, RED);
        }
    }
        EndDrawing();
    }
    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(middleground);
    UnloadTexture(foreground);

    CloseWindow();
}

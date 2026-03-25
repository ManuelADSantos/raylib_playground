#define RAYMATH_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 800
#define SAMPLE_SIZE 1000

Vector2 screen_center(Vector2 original_point)
{
    return (Vector2){(float)(original_point.x + SCREEN_WIDTH / 2.0),
                     (float)(original_point.y + SCREEN_HEIGHT / 2.0)};
}

float wave(float Amplitude, float frequency, float phase, float dt)
{
    return Amplitude*sinf(2*PI*frequency*dt + phase);
}

typedef struct
{
    Vector2 position;
    float radius;
    Color color;
} Ball;

// ------------------------------------------------------------------------------------
// Program main entry point
// ------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Sine Wave");

    Ball red_ball = {(Vector2){0.0, 0.0}, 10.0, RED};
    Ball green_ball = {(Vector2){-100.0, 0.0}, 10.0, GREEN};
    Ball blue_ball = {(Vector2){0.0, -100.0}, 10.0, BLUE};

    // SetTargetFPS(60);
    bool showFPS = true;

    TraceLog(LOG_INFO, "SCREEN_WIDTH / 2.0 = %f", SCREEN_WIDTH/2.0);
    TraceLog(LOG_INFO, "SCREEN_HEIGHT / 2.0 = %f", SCREEN_HEIGHT/2.0);
    TraceLog(LOG_INFO, "SCREEN_WIDTH / SAMPLE_SIZE = %f", (float)SCREEN_WIDTH/(float)SAMPLE_SIZE);

    float Amplitude = 75.0f;
    float frequency = 0.01f;
    float phase = 0.0f;
    Vector2 points[SAMPLE_SIZE];
    for (int i = 0; i < SAMPLE_SIZE; i++)
    {
        points[i].x = (float)(-SCREEN_WIDTH/2.0) + i*((float)(SCREEN_WIDTH) / (float)SAMPLE_SIZE);
        // TraceLog(LOG_INFO, "points[%d].x = %f", i, points[i].x);
        points[i].y = wave(Amplitude, frequency, phase, points[i].x);
    }
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_F))
            showFPS = !showFPS;

        float dt = GetTime();

        green_ball.position.x = 100.0f * cosf(dt);
        green_ball.position.y = 100.0f * sinf(dt);

        blue_ball.position.x = 100.0f * cosf(-dt + PI / 2);
        blue_ball.position.y = 100.0f * sinf(-dt + PI / 2);

        for (int i = 0; i < SAMPLE_SIZE; i++)
        {
            points[i].y = wave(Amplitude, frequency, -dt, points[i].x);
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(DARKGRAY);

        DrawCircleV(screen_center(red_ball.position), red_ball.radius, red_ball.color);
        DrawCircleV(screen_center(green_ball.position), green_ball.radius, green_ball.color);
        DrawCircleV(screen_center(blue_ball.position), blue_ball.radius, blue_ball.color);

        for (int i = 0; i < (int)(SAMPLE_SIZE) - 1; i++)
        {
            DrawPixelV(screen_center(points[i]) , RAYWHITE);
        }

        if (showFPS)
        {
            const char *fpsText = TextFormat("FPS: %i", GetFPS());
            DrawText(fpsText, 10, 10, 20, RAYWHITE);
        }

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
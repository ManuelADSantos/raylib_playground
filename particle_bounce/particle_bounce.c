#define RAYMATH_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    float radius;
    Color color;
} Particle;

#define RADIUS 10.0f
#define MAX_PARTICLES 1000

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Particle Bounce");

    Particle particles[MAX_PARTICLES];

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        Particle one_particle;
        one_particle.position = (Vector2){(int)GetRandomValue(screenWidth / 4, 3 * screenWidth / 4), (int)GetRandomValue(screenHeight / 4, 3 * screenHeight / 4)};
        one_particle.velocity = (Vector2){(float)GetRandomValue(-5, 5), (float)GetRandomValue(-5, 5)};
        one_particle.color = (Color){(int)GetRandomValue(0, 255), (int)GetRandomValue(0, 255), (int)GetRandomValue(0, 255), 255};
        one_particle.radius = RADIUS;
        particles[i] = one_particle;
    }

    SetTargetFPS(60);
    const char *fpsText = 0;

    bool paused = false;
    Vector2 last_velocities[MAX_PARTICLES];
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        for (int i = 0; i < MAX_PARTICLES; i++)
        {
            // Check for collision with walls and reverse velocity if necessary
            if (particles[i].position.x <= particles[i].radius || particles[i].position.x >= screenWidth - particles[i].radius)
            {
                particles[i].velocity.x *= -1;
            }
            if (particles[i].position.y <= particles[i].radius || particles[i].position.y >= screenHeight - particles[i].radius)
            {
                particles[i].velocity.y *= -1;
            }

            // Update particle position based on velocity
            particles[i].position.x += particles[i].velocity.x;
            particles[i].position.y += particles[i].velocity.y;
        }

        // Pause simulation and save velocities, or resume simulation and restore velocities
        if (IsKeyPressed(KEY_SPACE))
        {
            if (paused)
            {
                for (int i = 0; i < MAX_PARTICLES; i++)
                {
                    particles[i].velocity = last_velocities[i];
                }
            }
            else
            {
                for (int i = 0; i < MAX_PARTICLES; i++)
                {
                    last_velocities[i] = particles[i].velocity;
                    particles[i].velocity = Vector2Zero();
                }
            }
            paused = !paused;
        }

        // Reset particle positions and velocities
        if (IsKeyPressed(KEY_R))
        {
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                particles[i].position = (Vector2){(int)GetRandomValue(screenWidth / 4, 3 * screenWidth / 4),
                                                  (int)GetRandomValue(screenHeight / 4, 3 * screenHeight / 4)};
                particles[i].velocity.x = (float)GetRandomValue(-5, 5);
                particles[i].velocity.y = (float)GetRandomValue(-5, 5);
                last_velocities[i] = particles[i].velocity;
            }
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        fpsText = TextFormat("FPS: %i", GetFPS());

        DrawText(fpsText, 10, 10, 20, DARKGRAY);

        for (int i = 0; i < MAX_PARTICLES; i++)
            DrawCircleV(particles[i].position, particles[i].radius, particles[i].color);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow(); // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
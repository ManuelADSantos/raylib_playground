#define RAYMATH_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    float radius;
    float mass;
    Color color;
} Particle;

#define RADIUS 10.0f
#define MAX_PARTICLES 100
#define MAX_VELOCITY 3.0f
#define radius_increment 1.0f

// ------------------------------------------------------------------------------------
// Program main entry point
// ------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1600;
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
        one_particle.mass = 1.0f;
        particles[i] = one_particle;
    }

    SetTargetFPS(60);
    const char *fpsText = 0;
    bool showFPS = true;
    bool showCollisions = false;
    bool paused = false;
    Vector2 last_velocities[MAX_PARTICLES];

    float masses_play, numerator, denominator;
    Vector2 positions_play, right_part, v1_temp, v2_temp;
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        for (int i = 0; i < MAX_PARTICLES; i++)
        {
            if (particles[i].position.x <= particles[i].radius || particles[i].position.x >= screenWidth - particles[i].radius)
            {
                particles[i].velocity.x *= -1;
            }
            if (particles[i].position.y <= particles[i].radius || particles[i].position.y >= screenHeight - particles[i].radius)
            {
                particles[i].velocity.y *= -1;
            }

            // Only check collisions with particles that haven't been checked yet to avoid checking the same collision twice
            for (int j = i + 1; j < MAX_PARTICLES; j++)
            {
                // Skipt collision check with itself
                if (i == j)
                    continue;
                
                // Check for collision
                if (CheckCollisionCircles(particles[i].position, particles[i].radius, particles[j].position, particles[j].radius))
                {
                    // Log collision details if enabled and not paused
                    if (showCollisions && !paused)
                    {
                        TraceLog(LOG_INFO, "Collision detected between particle %d (x: %.2f, y: %.2f)|(v: %.2f, y: %.2f) and particle %d (x: %.2f, y: %.2f)|(vx: %.2f, vy: %.2f)",
                                 i, particles[i].position.x, particles[i].position.y, particles[i].velocity.x, particles[i].velocity.y,
                                 j, particles[j].position.x, particles[j].position.y, particles[j].velocity.x, particles[j].velocity.y);
                    }

                    // // Check if particles are moving towards each other before checking for collision
                    Vector2 delta_move = Vector2Subtract(particles[j].position, particles[i].position);
                    Vector2 relativeVelocity = Vector2Subtract(particles[j].velocity, particles[i].velocity);
                    if (Vector2DotProduct(relativeVelocity, delta_move) >= 0)
                        continue;

                    // Do position correction to avoid sinking due to fast velocities or small radii
                    Vector2 delta = Vector2Subtract(particles[i].position, particles[j].position);
                    float distance = Vector2Length(delta);
                    float min_distance = particles[i].radius + particles[j].radius;
                    if (distance < min_distance && distance > 0.0001f)
                    {
                        float overlap = min_distance - distance;
                        Vector2 correction = Vector2Scale(Vector2Normalize(delta), overlap / 2);
                        particles[i].position = Vector2Add(particles[i].position, correction);
                        particles[j].position = Vector2Subtract(particles[j].position, correction);
                    }

                    // Particle 1
                    masses_play = -1 * ((2 * particles[j].mass) / (particles[i].mass + particles[j].mass));
                    numerator = Vector2DotProduct(Vector2Subtract(particles[i].velocity, particles[j].velocity),
                                                  Vector2Subtract(particles[i].position, particles[j].position));
                    denominator = Vector2LengthSqr(Vector2Subtract(particles[i].position, particles[j].position));
                    if (denominator < 0.0001f)
                        continue;
                    positions_play = Vector2Subtract(particles[i].position, particles[j].position);
                    right_part = Vector2Scale(positions_play, masses_play * (numerator / denominator));
                    v1_temp = Vector2Add(particles[i].velocity, right_part);

                    // Particle 2
                    masses_play = -1 * ((2 * particles[i].mass) / (particles[i].mass + particles[j].mass));
                    numerator = Vector2DotProduct(Vector2Subtract(particles[j].velocity, particles[i].velocity),
                                                  Vector2Subtract(particles[j].position, particles[i].position));
                    denominator = Vector2LengthSqr(Vector2Subtract(particles[j].position, particles[i].position));
                    if (denominator < 0.0001f)
                        continue;
                    positions_play = Vector2Subtract(particles[j].position, particles[i].position);
                    right_part = Vector2Scale(positions_play, masses_play * (numerator / denominator));
                    v2_temp = Vector2Add(particles[j].velocity, right_part);
                    
                    // Resulting velocities are clamped to avoid extreme values in case of very fast particles or very close collisions
                    particles[i].velocity = (Vector2){Clamp(v1_temp.x, -MAX_VELOCITY, MAX_VELOCITY), Clamp(v1_temp.y, -MAX_VELOCITY, MAX_VELOCITY)};
                    particles[i].radius += radius_increment;
                    particles[j].velocity = (Vector2){Clamp(v2_temp.x, -MAX_VELOCITY, MAX_VELOCITY), Clamp(v2_temp.y, -MAX_VELOCITY, MAX_VELOCITY)};
                    particles[j].radius += radius_increment;
                }
            }

            // Update particle position based on velocity
            particles[i].position.x += particles[i].velocity.x;
            particles[i].position.y += particles[i].velocity.y;
        }

        // Pressing space will pause/unpause the simulation
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

        // Pressing R will randomize particle velocities
        if (IsKeyDown(KEY_R))
        {
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                particles[i].position.x = (int)GetRandomValue(screenWidth / 4, 3 * screenWidth / 4);
                particles[i].position.y = (int)GetRandomValue(screenHeight / 4, 3 * screenHeight / 4);
                particles[i].radius = RADIUS;
                if (!paused)
                {
                    particles[i].velocity.x = (float)GetRandomValue(-MAX_VELOCITY, MAX_VELOCITY);
                    particles[i].velocity.y = (float)GetRandomValue(-MAX_VELOCITY, MAX_VELOCITY);
                }
                else
                {
                    particles[i].velocity = Vector2Zero();
                    last_velocities[i] = (Vector2){(float)GetRandomValue(-MAX_VELOCITY, MAX_VELOCITY), (float)GetRandomValue(-MAX_VELOCITY, MAX_VELOCITY)};
                }
            }
        }

        // Pressing F will toggle FPS display
        if (IsKeyPressed(KEY_F))
            showFPS = !showFPS;

        // Pressing C will toggle collision logs
        if (IsKeyPressed(KEY_C))
        {
            showCollisions = !showCollisions;
            TraceLog(LOG_INFO, "Show collisions: %s", showCollisions ? "ON" : "OFF");
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        for (int i = 0; i < MAX_PARTICLES; i++)
            DrawCircleV(particles[i].position, particles[i].radius, particles[i].color);

        if (showFPS)
        {
            fpsText = TextFormat("FPS: %i", GetFPS());
            DrawText(fpsText, 10, 10, 20, DARKGRAY);
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
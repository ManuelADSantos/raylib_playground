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
#define MAX_VELOCITY 10.0f
// #define radius_increment 0.05f
#define radius_increment 0.0f

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

    // float masses_play, numerator, denominator;
    // Vector2 positions_play, right_part, v1_temp, v2_temp;
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

            for (int j = 0; j < MAX_PARTICLES; j++)
            {
                // Skipt collision check with itself
                if (i == j)
                    continue;

                // if (CheckCollisionCircles(particles[i].position, particles[i].radius, particles[j].position, particles[j].radius))
                // {
                    // TraceLog(LOG_INFO, "Collision detected between particle %d (x: %.2f, y: %.2f)|(v: %.2f, y: %.2f) and particle %d (x: %.2f, y: %.2f)|(vx: %.2f, vy: %.2f)",
                    //          i, particles[i].position.x, particles[i].position.y, particles[i].velocity.x, particles[i].velocity.y,
                    //          j, particles[j].position.x, particles[j].position.y, particles[j].velocity.x, particles[j].velocity.y);

                    // Particle 1
                    // masses_play = -1 * ((2 * particles[j].mass) / (particles[i].mass + particles[j].mass));
                    // numerator = Vector2DotProduct(Vector2Subtract(particles[i].velocity, particles[j].velocity),
                    //                               Vector2Subtract(particles[i].position, particles[j].position));
                    // denominator = Vector2LengthSqr(Vector2Subtract(particles[i].position, particles[j].position));
                    // positions_play = Vector2Subtract(particles[i].position, particles[j].position);
                    // right_part = Vector2Scale(positions_play, masses_play * (numerator / denominator));
                    // v1_temp = Vector2Add(particles[i].velocity, right_part);

                    // Particle 2
                    // masses_play = -1 * ((2 * particles[i].mass) / (particles[i].mass + particles[j].mass));
                    // numerator = Vector2DotProduct(Vector2Subtract(particles[j].velocity, particles[i].velocity),
                    //                               Vector2Subtract(particles[j].position, particles[i].position));
                    // denominator = Vector2LengthSqr(Vector2Subtract(particles[j].position, particles[i].position));
                    // positions_play = Vector2Subtract(particles[j].position, particles[i].position);
                    // right_part = Vector2Scale(positions_play, masses_play * (numerator / denominator));
                    // v2_temp = Vector2Add(particles[j].velocity, right_part);

                    // particles[i].velocity = (Vector2){Clamp(v1_temp.x, -MAX_VELOCITY, MAX_VELOCITY), Clamp(v1_temp.y, -MAX_VELOCITY, MAX_VELOCITY)};
                    // particles[i].radius += radius_increment;
                    // particles[j].velocity = (Vector2){Clamp(v2_temp.x, -MAX_VELOCITY, MAX_VELOCITY), Clamp(v2_temp.y, -MAX_VELOCITY, MAX_VELOCITY)};
                    // particles[j].radius += radius_increment;
                // }
            }

            particles[i].position.x += particles[i].velocity.x;
            particles[i].position.y += particles[i].velocity.y;
        }

        if (IsKeyDown(KEY_SPACE))
        {
            for (int i = 0; i < MAX_PARTICLES; i++)
            {
                particles[i].velocity.x = (float)GetRandomValue(-5, 5);
                particles[i].velocity.y = (float)GetRandomValue(-5, 5);
            }
        }

        if (IsKeyPressed(KEY_F)) showFPS = !showFPS;

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

// #define NUM_PARTICLES_TEST 2
// int main(void)
// {
//     // Initialization
//     //--------------------------------------------------------------------------------------
//     const int screenWidth = 800;
//     const int screenHeight = 800;

//     InitWindow(screenWidth, screenHeight, "Particle Bounce");

//     Particle particles[NUM_PARTICLES_TEST];

//     Particle particle_1;
//     particle_1.position = (Vector2){screenWidth * (1.0f / 4.0f), screenHeight * (2.0f / 4.0f)};
//     particle_1.velocity = (Vector2){1.0f, 0.0f};
//     particle_1.color = RED;
//     particle_1.radius = RADIUS;
//     particle_1.mass = 1.0f;
//     particles[0] = particle_1;

//     Particle particle_2;
//     particle_2.position = (Vector2){screenWidth * (3.0f / 4.0f), screenHeight * (2.0f / 4.0f)};
//     particle_2.velocity = (Vector2){-1.0f, 0.0f};
//     particle_2.color = BLUE;
//     particle_2.radius = RADIUS;
//     particle_2.mass = 1.0f;
//     particles[1] = particle_2;

//     SetTargetFPS(60);
//     const char *fpsText = 0;
//     bool showFPS = true;

//     float masses_play, numerator, denominator;
//     Vector2 positions_play, right_part, v1_temp, v2_temp;
//     //--------------------------------------------------------------------------------------

//     // Main game loop
//     while (!WindowShouldClose()) // Detect window close button or ESC key
//     {
//         // Update
//         //----------------------------------------------------------------------------------
//         for (int i = 0; i < NUM_PARTICLES_TEST; i++)
//         {
//             if (particles[i].position.x <= particles[i].radius || particles[i].position.x >= screenWidth - particles[i].radius)
//             {
//                 particles[i].velocity.x *= -1;
//             }
//             if (particles[i].position.y <= particles[i].radius || particles[i].position.y >= screenHeight - particles[i].radius)
//             {
//                 particles[i].velocity.y *= -1;
//             }

//             for (int j = 0; j < NUM_PARTICLES_TEST; j++)
//             {
//                 // Skipt collision check with itself
//                 if (i == j)
//                     continue;

//                 if (CheckCollisionCircles(particles[i].position, particles[i].radius, particles[j].position, particles[j].radius))
//                 {
//                     TraceLog(LOG_INFO, "Collision detected between particle %d (x: %.2f, y: %.2f)|(v: %.2f, y: %.2f) and particle %d (x: %.2f, y: %.2f)|(vx: %.2f, vy: %.2f)",
//                              i, particles[i].position.x, particles[i].position.y, particles[i].velocity.x, particles[i].velocity.y,
//                              j, particles[j].position.x, particles[j].position.y, particles[j].velocity.x, particles[j].velocity.y);

//                     // Particle 1
//                     masses_play = -1 * ((2 * particles[j].mass) / (particles[i].mass + particles[j].mass));
//                     numerator = Vector2DotProduct(Vector2Subtract(particles[i].velocity, particles[j].velocity),
//                                                   Vector2Subtract(particles[i].position, particles[j].position));
//                     denominator = Vector2LengthSqr(Vector2Subtract(particles[i].position, particles[j].position));
//                     positions_play = Vector2Subtract(particles[i].position, particles[j].position);
//                     right_part = Vector2Scale(positions_play, masses_play * (numerator / denominator));
//                     v1_temp = Vector2Add(particles[i].velocity, right_part);

//                     // Particle 2
//                     masses_play = -1 * ((2 * particles[i].mass) / (particles[i].mass + particles[j].mass));
//                     numerator = Vector2DotProduct(Vector2Subtract(particles[j].velocity, particles[i].velocity),
//                                                   Vector2Subtract(particles[j].position, particles[i].position));
//                     denominator = Vector2LengthSqr(Vector2Subtract(particles[j].position, particles[i].position));
//                     positions_play = Vector2Subtract(particles[j].position, particles[i].position);
//                     right_part = Vector2Scale(positions_play, masses_play * (numerator / denominator));
//                     v2_temp = Vector2Add(particles[j].velocity, right_part);

//                     particles[i].velocity = v1_temp;
//                     particles[j].velocity = v2_temp;
//                 }
//             }

//             particles[i].position.x += particles[i].velocity.x;
//             particles[i].position.y += particles[i].velocity.y;
//         }

//         if (IsKeyDown(KEY_SPACE))
//         {
//             for (int i = 0; i < NUM_PARTICLES_TEST; i++)
//             {
//                 particles[i].velocity.x = (float)GetRandomValue(-5, 5);
//                 particles[i].velocity.y = (float)GetRandomValue(-5, 5);
//             }
//         }

//         if (IsKeyPressed(KEY_F))
//             showFPS = !showFPS;

//         //----------------------------------------------------------------------------------

//         // Draw
//         //----------------------------------------------------------------------------------
//         BeginDrawing();

//         ClearBackground(RAYWHITE);

//         fpsText = TextFormat("FPS: %i", GetFPS());

//         if (showFPS)
//             DrawText(fpsText, 10, 10, 20, DARKGRAY);

//         for (int i = 0; i < NUM_PARTICLES_TEST; i++)
//             DrawCircleV(particles[i].position, particles[i].radius, particles[i].color);

//         EndDrawing();
//         //----------------------------------------------------------------------------------
//     }

//     // De-Initialization
//     //--------------------------------------------------------------------------------------
//     CloseWindow(); // Close window and OpenGL context
//     //--------------------------------------------------------------------------------------

//     return 0;
// }
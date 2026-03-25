#define RAYMATH_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float radius;
    float mass;
    Color color;
} Particle;

typedef struct
{
    Vector2 position;
    int width;
    int height;
} Box;

#define RADIUS 5.0f
#define MAX_PARTICLES 100
#define MAX_VELOCITY 200.0f
#define COLOR_MODES 2
#define GRAVITY_STRENGTH 0.0f

// ------------------------------------------------------------------------------------
// Program main entry point
// ------------------------------------------------------------------------------------
int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1500;
    const int screenHeight = 900;
    InitWindow(screenWidth, screenHeight, "Boids");

    // Box box = {(Vector2){screenWidth/6.0, screenHeight / 6.0}, screenWidth * (4 / 6), screenHeight * (4 / 6)};
    Box box = {(Vector2){250, 150}, 1000, 600};
    TraceLog(LOG_INFO, "Box parameters: %d x %d | Width: %d | Height: %d", box.position.x, box.position.y, box.width, box.height);

    // Initialize particles
    Particle particles[MAX_PARTICLES];
    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        Particle one_particle;
        one_particle.position = (Vector2){(int)GetRandomValue(box.position.x, box.position.x + box.width),
                                          (int)GetRandomValue(box.position.y, box.position.y + box.height)};
        one_particle.velocity = (Vector2){(float)GetRandomValue(-MAX_VELOCITY, MAX_VELOCITY), (float)GetRandomValue(-MAX_VELOCITY, MAX_VELOCITY)};
        one_particle.acceleration = (Vector2){0.0f, GRAVITY_STRENGTH};
        one_particle.color = (Color){(int)GetRandomValue(0, 255), (int)GetRandomValue(0, 255), (int)GetRandomValue(0, 255), 255};
        one_particle.radius = RADIUS;
        one_particle.mass = 1.0f;
        particles[i] = one_particle;
    }

    // Several variables
    // #TODO: Clean this up, maybe use a struct to hold all these variables
    const char *fpsText = 0;
    bool showFPS = true;
    bool paused = false;
    bool showVectors = true;
    int color_mode = 1;
    Vector2 last_velocities[MAX_PARTICLES];
    float masses_play, numerator, denominator;
    Vector2 positions_play, right_part, v1_temp, v2_temp;
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Refactor this into separate functions to clean up the main loop and optimize performance
        for (int i = 0; i < MAX_PARTICLES; i++)
        {
            // ----------> Edge collision
            bool on_edge = false;
            Vector2 edge_position = {0.0f, 0.0f};
            // Left edge
            if (particles[i].position.x <= particles[i].radius + box.position.x)
            {
                particles[i].velocity.x *= -1;
                edge_position = (Vector2){box.position.x, particles[i].position.y};
                on_edge = true;
            }
            // Right edge
            if (particles[i].position.x >= box.position.x + box.width - particles[i].radius)
            {
                particles[i].velocity.x *= -1;
                edge_position = (Vector2){box.position.x + box.width, particles[i].position.y};
                on_edge = true;
            }
            // Top edge
            if (particles[i].position.y <= particles[i].radius + box.position.y)
            {
                particles[i].velocity.y *= -1;
                edge_position = (Vector2){particles[i].position.x, box.position.y};
                on_edge = true;
            }
            // Bottom edge
            if (particles[i].position.y >= box.position.y + box.height - particles[i].radius)
            {
                particles[i].velocity.y *= -1;
                edge_position = (Vector2){particles[i].position.x, box.position.y + box.height};
                on_edge = true;
            }

            if (on_edge)
            {
                Vector2 delta = Vector2Subtract(particles[i].position, edge_position);
                float distance = Vector2Length(delta);
                float min_distance = particles[i].radius;
                if (distance < min_distance && distance > 0.0001f)
                {
                    float overlap = min_distance - distance;
                    Vector2 correction = Vector2Scale(Vector2Normalize(delta), overlap);
                    particles[i].position = Vector2Add(particles[i].position, correction);
                }
            }

            // ----------> Particle collision
            // Only check collisions with particles that haven't been checked yet to avoid checking the same collision twice
            for (int j = i + 1; j < MAX_PARTICLES; j++)
            {
                // Skipt collision check with itself
                if (i == j)
                    continue;

                // Check for particle collision
                if (CheckCollisionCircles(particles[i].position, particles[i].radius, particles[j].position, particles[j].radius))
                {
                    // Check if particles are moving towards each other before checking for collision
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
                    particles[j].velocity = (Vector2){Clamp(v2_temp.x, -MAX_VELOCITY, MAX_VELOCITY), Clamp(v2_temp.y, -MAX_VELOCITY, MAX_VELOCITY)};
                }
            }

            // ----------> Movement and color update
            float dt = GetFrameTime();
            // Update particle velocity based on acceleration
            particles[i].velocity.x += particles[i].acceleration.x * dt;
            particles[i].velocity.y += particles[i].acceleration.y * dt;
            // Update particle position based on velocity
            particles[i].position.x += particles[i].velocity.x * dt;
            particles[i].position.y += particles[i].velocity.y * dt;

            // Update particle color based on velocity
            float speed = Vector2Length(particles[i].velocity);
            float normalizedSpeed = Normalize(speed, 0.0, Vector2Length((Vector2){MAX_VELOCITY, MAX_VELOCITY})); // Normalize speed to range [0, 1] based on maximum possible speed
            // TraceLog(LOG_INFO, "Particle %d speed: %.2f, normalized: %.2f", i, speed, normalizedSpeed);

            if (color_mode == 0)
            {
                // -- Short rainbow gradient based on speed --
                float a = (1 - normalizedSpeed) / 0.25;
                int X = (int)a;
                int Y = (int)(255 * (a - X));
                switch (X)
                {
                case 0:
                    particles[i].color = (Color){255, Y, 0, 255};
                    break;
                case 1:
                    particles[i].color = (Color){255 - Y, 255, 0, 255};
                    break;
                case 2:
                    particles[i].color = (Color){0, 255, Y, 255};
                    break;
                case 3:
                    particles[i].color = (Color){0, 255 - Y, 255, 255};
                    break;
                case 4:
                    particles[i].color = (Color){0, 0, 255, 255};
                    break;
                }
            }

            if (color_mode == 1)
            {
                // -- Long rainbow gradient based on speed --
                float a = (1 - normalizedSpeed) / 0.2;
                int X = (int)a;
                int Y = (int)(255 * (a - X));
                switch (X)
                {
                case 0:
                    particles[i].color = (Color){255, Y, 0, 255};
                    break;
                case 1:
                    particles[i].color = (Color){255 - Y, 255, 0, 255};
                    break;
                case 2:
                    particles[i].color = (Color){0, 255, Y, 255};
                    break;
                case 3:
                    particles[i].color = (Color){0, 255 - Y, 255, 255};
                    break;
                case 4:
                    particles[i].color = (Color){Y, 0, 255, 255};
                    break;
                case 5:
                    particles[i].color = (Color){255, 0, 255, 255};
                    break;
                }
            }

            particles[i].color = (Color){Clamp(particles[i].color.r, 0, 255),
                                         Clamp(particles[i].color.g, 0, 255),
                                         Clamp(particles[i].color.b, 0, 255),
                                         255};
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
                    last_velocities[i] = (Vector2){(float)GetRandomValue(-MAX_VELOCITY, MAX_VELOCITY),
                                                   (float)GetRandomValue(-MAX_VELOCITY, MAX_VELOCITY)};
                }
            }
        }

        // Pressing F will toggle FPS display
        if (IsKeyPressed(KEY_F))
            showFPS = !showFPS;

        // Pressing M will toggle color scheme
        if (IsKeyPressed(KEY_M))
        {
            color_mode = (color_mode + 1) % COLOR_MODES;
        }

        // Pressing V will toggle vector display
        if (IsKeyPressed(KEY_V))
        {
            showVectors = !showVectors;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(BLACK);

        DrawRectangle(box.position.x, box.position.y,
                      box.width, box.height,
                      (Color){255, 255, 255, 50});

        for (int i = 0; i < MAX_PARTICLES; i++)
        {
            DrawCircleV(particles[i].position, particles[i].radius, particles[i].color);
            if (showVectors)
            {
                DrawLine(particles[i].position.x, particles[i].position.y,
                         particles[i].position.x + (int)(particles[i].velocity.x) / 2, particles[i].position.y + (int)(particles[i].velocity.y) / 2,
                         particles[i].color);
            }
        }

        if (showFPS)
        {
            fpsText = TextFormat("FPS: %i", GetFPS());
            DrawText(fpsText, 10, 10, 20, WHITE);
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
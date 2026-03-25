#define RAYMATH_IMPLEMENTATION
#include "raylib.h"
#include "raymath.h"
#include <math.h>

typedef struct
{
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    float radius;
    float mass;
    Color color;
} Particle;

#define MAX_PARTICLES 3000
#define RADIUS 5.0f
#define MAX_VELOCITY 200.0f
#define GRAVITY_STRENGTH 0.0f

// Spatial grid
#define CELL_SIZE (RADIUS * 2)
#define MAX_GRID_WIDTH 400
#define MAX_GRID_HEIGHT 400

typedef struct {
    int indices[MAX_PARTICLES];
    int count;
} Cell;

Cell grid[MAX_GRID_WIDTH][MAX_GRID_HEIGHT];

int screenWidth = 800;
int screenHeight = 800;

// --------------------------------------------------
// Utility
// --------------------------------------------------
static inline int ClampInt(int v, int min, int max) {
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

void ClearGrid(int gridW, int gridH)
{
    for (int x = 0; x < gridW; x++)
        for (int y = 0; y < gridH; y++)
            grid[x][y].count = 0;
}

void InsertIntoGrid(Particle *p, int index, int gridW, int gridH)
{
    int gx = ClampInt((int)(p->position.x / CELL_SIZE), 0, gridW - 1);
    int gy = ClampInt((int)(p->position.y / CELL_SIZE), 0, gridH - 1);

    Cell *cell = &grid[gx][gy];
    if (cell->count < MAX_PARTICLES)
        cell->indices[cell->count++] = index;
}

// --------------------------------------------------
// Physics
// --------------------------------------------------
void ResolveWallCollision(Particle *p)
{
    if (p->position.x <= p->radius || p->position.x >= screenWidth - p->radius)
        p->velocity.x *= -1;

    if (p->position.y <= p->radius || p->position.y >= screenHeight - p->radius)
        p->velocity.y *= -1;

    p->position.x = Clamp(p->position.x, p->radius, screenWidth - p->radius);
    p->position.y = Clamp(p->position.y, p->radius, screenHeight - p->radius);
}

void ResolveParticleCollision(Particle *a, Particle *b)
{
    Vector2 delta = Vector2Subtract(a->position, b->position);
    float distSqr = Vector2LengthSqr(delta);

    float minDist = a->radius + b->radius;

    if (distSqr >= minDist * minDist || distSqr < 0.0001f)
        return;

    float dist = sqrtf(distSqr);
    Vector2 normal = Vector2Scale(delta, 1.0f / dist);

    // Skip if moving apart
    Vector2 relativeVel = Vector2Subtract(b->velocity, a->velocity);
    if (Vector2DotProduct(relativeVel, normal) > 0)
        return;

    // Position correction
    float overlap = minDist - dist;
    Vector2 correction = Vector2Scale(normal, overlap * 0.5f);
    a->position = Vector2Add(a->position, correction);
    b->position = Vector2Subtract(b->position, correction);

    // Elastic collision
    float m1 = a->mass;
    float m2 = b->mass;

    float impulse = (2 * Vector2DotProduct(relativeVel, normal)) / (m1 + m2);

    a->velocity = Vector2Add(a->velocity, Vector2Scale(normal, impulse * m2));
    b->velocity = Vector2Subtract(b->velocity, Vector2Scale(normal, impulse * m1));

    // Clamp velocities
    a->velocity.x = Clamp(a->velocity.x, -MAX_VELOCITY, MAX_VELOCITY);
    a->velocity.y = Clamp(a->velocity.y, -MAX_VELOCITY, MAX_VELOCITY);
    b->velocity.x = Clamp(b->velocity.x, -MAX_VELOCITY, MAX_VELOCITY);
    b->velocity.y = Clamp(b->velocity.y, -MAX_VELOCITY, MAX_VELOCITY);
}

// ✅ Removed unused `count` parameter
void HandleCollisions(Particle *particles, int gridW, int gridH)
{
    for (int gx = 0; gx < gridW; gx++)
    {
        for (int gy = 0; gy < gridH; gy++)
        {
            Cell *cell = &grid[gx][gy];

            for (int i = 0; i < cell->count; i++)
            {
                int idxA = cell->indices[i];

                for (int nx = -1; nx <= 1; nx++)
                {
                    for (int ny = -1; ny <= 1; ny++)
                    {
                        int cx = gx + nx;
                        int cy = gy + ny;

                        if (cx < 0 || cy < 0 || cx >= gridW || cy >= gridH)
                            continue;

                        Cell *neighbor = &grid[cx][cy];

                        for (int j = 0; j < neighbor->count; j++)
                        {
                            int idxB = neighbor->indices[j];

                            if (idxA >= idxB) continue;

                            ResolveParticleCollision(&particles[idxA], &particles[idxB]);
                        }
                    }
                }
            }
        }
    }
}

void UpdateParticles(Particle *particles, int count, float dt)
{
    for (int i = 0; i < count; i++)
    {
        particles[i].velocity.x += particles[i].acceleration.x * dt;
        particles[i].velocity.y += particles[i].acceleration.y * dt;

        particles[i].position.x += particles[i].velocity.x * dt;
        particles[i].position.y += particles[i].velocity.y * dt;

        ResolveWallCollision(&particles[i]);
    }
}

// --------------------------------------------------
// Visuals
// --------------------------------------------------
void UpdateColor(Particle *p)
{
    float speed = Vector2Length(p->velocity);
    float t = Clamp(speed / MAX_VELOCITY, 0.0f, 1.0f);

    p->color = (Color){
        (unsigned char)(255 * t),
        50,
        (unsigned char)(255 * (1 - t)),
        255
    };
}

// --------------------------------------------------
// Main
// --------------------------------------------------
int main(void)
{
    InitWindow(screenWidth, screenHeight, "Particle Simulator (Optimized)");

    Particle particles[MAX_PARTICLES];

    for (int i = 0; i < MAX_PARTICLES; i++)
    {
        particles[i].position = (Vector2){
            GetRandomValue(100, 700),
            GetRandomValue(100, 700)
        };

        particles[i].velocity = (Vector2){
            GetRandomValue(-100, 100),
            GetRandomValue(-100, 100)
        };

        particles[i].acceleration = (Vector2){0.0f, GRAVITY_STRENGTH};
        particles[i].radius = RADIUS;
        particles[i].mass = 1.0f;
        particles[i].color = WHITE;
    }

    int gridW = screenWidth / CELL_SIZE;
    int gridH = screenHeight / CELL_SIZE;

    SetTargetFPS(120);

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        ClearGrid(gridW, gridH);
        for (int i = 0; i < MAX_PARTICLES; i++)
            InsertIntoGrid(&particles[i], i, gridW, gridH);

        HandleCollisions(particles, gridW, gridH);
        UpdateParticles(particles, MAX_PARTICLES, dt);

        for (int i = 0; i < MAX_PARTICLES; i++)
            UpdateColor(&particles[i]);

        BeginDrawing();
        ClearBackground(BLACK);

        for (int i = 0; i < MAX_PARTICLES; i++)
            DrawCircleV(particles[i].position, particles[i].radius, particles[i].color);

        DrawText(TextFormat("FPS: %i", GetFPS()), 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
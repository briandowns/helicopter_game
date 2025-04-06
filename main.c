#include <stdlib.h>

#include <raylib.h>
#include <raymath.h>

#define RAYTMX_IMPLEMENTATION
#include "raytmx.h"

#define MAX_FRAME_SPEED 15
#define MIN_FRAME_SPEED  1

#define FPS 60

#define HELI_FRAME_COUNT 4
#define HELI_SPEED 5
#define GRAVITY .3;

void
DrawTime()
{
    double now = GetTime();
    int seconds = (int)now%60;
    int minutes = (int)now/60;

    DrawText(TextFormat("Elapsed Time: m %i s %i", minutes, seconds), 0, 0, 30, WHITE);
}

int
main(void)
{
    InitWindow(1280, 900, "Helicopter");
    InitAudioDevice();

    // spritesheet
    // width: 226
    // height: 320
    // sprite: w 226 x h 80
    Texture2D helicopter_texture = LoadTexture("assets/helicopter-spritesheet.png");

    Texture2D background = LoadTexture("assets/background/Layers/sky.png");
    // Texture2D midground = LoadTexture("resources/cyberpunk_street_midground.png");
    // Texture2D foreground = LoadTexture("resources/cyberpunk_street_foreground.png");

    Sound sound = LoadSound("");
    Music music = LoadMusicStream("assets/sound/airwolf.mp3");
    // PlayMusicStream(music);

    Vector2 position = { 320.0f, 226.0f };
    Rectangle frame_rec = { 0.0f, 0.0f, (float)helicopter_texture.width, (float)helicopter_texture.height/HELI_FRAME_COUNT };
    Vector2 helicopter_velocity = (Vector2){0.0f, 0.0f};
    int current_frame = 0;

    int frames_counter = 0;
    int frames_speed = 8; 

    float scrollingBack = 0.0f;

    SetTargetFPS(FPS);

    const float panSpeed = 150.0f;

    // Camera2D camera = {0};
    // camera.target = position;
    // camera.offset = (Vector2){ 1280/2.0f, 900/2.0f };
    // camera.rotation = 0.0f;
    // camera.zoom = 1.0f;

    const char* tmx = "assets/helicopter.tmx";
    TmxMap* map = LoadTMX(tmx);

    Camera2D camera;
    camera.zoom = 6.0f;
    camera.target.x = (float)(map->width * map->tileWidth) / 2.0f;
    camera.target.y = (float)(map->height * map->tileHeight) / 2.0f;
    camera.offset.x = (float)1280 / 2.0f;
    camera.offset.y = (float)900 / 2.0f;
    camera.rotation = 0.0f;

    bool is_in_menu = true;

    while (!WindowShouldClose()) {
        // update section

        // if (is_in_menu) {
        //     if (IsKeyPressed(KEY_Q)) {
        //         is_in_menu = false;
        //     }
        // }
        // else {
        //     if (IsKeyPressed(KEY_W)) {
        //         is_in_menu = true;
        //     }
        // }

        scrollingBack -= 0.1f;

        if (scrollingBack <= -background.width*2) scrollingBack = 0;

        if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_M)) {
            // TODO: look into volume adjustment instead
            if (IsMusicStreamPlaying(music)) {
                StopMusicStream(music);
            } else {
                PlayMusicStream(music);
            }
        }

        UpdateMusicStream(music);

        if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_SPACE)) {
            helicopter_velocity.y = -HELI_SPEED;
        }

        // Control frames speed
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            helicopter_velocity.x = HELI_SPEED;
            if (frame_rec.width < 0) {
                frame_rec.width = -frame_rec.width;
            }
            camera.target.x += panSpeed * GetFrameTime();
        } else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            helicopter_velocity.x = -HELI_SPEED;
            if (frame_rec.width > 0) {
                frame_rec.width = -frame_rec.width;
            }
            camera.target.x -= panSpeed * GetFrameTime();
        } else {
            helicopter_velocity.x = 0;
        }
    
        bool helicopyter_moving = helicopter_velocity.x != 0.0f || helicopter_velocity.y != 0.0f;

        frames_counter++;

        position = Vector2Add(position, helicopter_velocity);

        helicopter_velocity.y += GRAVITY;

        if (frames_counter >= (FPS/frames_speed)) {
            frames_counter = 0;
            current_frame++;

            if (current_frame > 3) {
                current_frame = 0;
            }

            frame_rec.y = (float)current_frame*(float)helicopter_texture.height/HELI_FRAME_COUNT;
        }

        if (frames_speed > MAX_FRAME_SPEED) {
            frames_speed = MAX_FRAME_SPEED;
        } else if (frames_speed < MIN_FRAME_SPEED) {
            frames_speed = MIN_FRAME_SPEED;
        }

        camera.target = position;
        camera.offset = (Vector2){ 1280/2.0f, 900/2.0f };
        float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;

        // draw section
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawTexture(background, 1280/2 - background.width/2, 900/2 - background.height/2, WHITE);
            
            BeginMode2D(camera);
            {
                //AnimateTMX(map);
                DrawTMX(map, &camera, 0, 0, WHITE);
            }
            EndMode2D();
            DrawTime();

            DrawTextureRec(helicopter_texture, frame_rec, position, WHITE);
        EndDrawing();
    }

    UnloadTMX(map);
    UnloadTexture(background);
    UnloadTexture(helicopter_texture);
    StopMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
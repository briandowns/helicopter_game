#include <stdlib.h>

#include <raylib.h>
#include <raymath.h>

#define MAX_FRAME_SPEED 15
#define MIN_FRAME_SPEED  1

#define FPS 60

#define HELI_FRAME_COUNT 4
#define HELI_SPEED 5
#define GRAVITY .3;

void
DrawTime()
{
    double gt = GetTime();
    int seconds = (int)gt%60;
    int minutes = (int)gt/60;

    DrawText(TextFormat("Elapsed Time: m %i s %i", minutes, seconds), 0, 0, 30, WHITE);
}

int
main(void)
{
    InitWindow(1200, 900, "Helicopter");
    InitAudioDevice();

    // spritesheet
    // width: 226
    // height: 320
    // sprite: w 226 x h 80
    Texture2D helicopter_texture = LoadTexture("assets/helicopter-spritesheet.png");

    Sound sound = LoadSound("");
    Music music = LoadMusicStream("assets/sound/airwolf.mp3");
    // PlayMusicStream(music);

    Vector2 position = { 320.0f, 226.0f };
    Rectangle frame_rec = { 0.0f, 0.0f, (float)helicopter_texture.width, (float)helicopter_texture.height/HELI_FRAME_COUNT };
    Vector2 helicopter_velocity = (Vector2){0.0f, 0.0f};
    int current_frame = 0;

    int frames_counter = 0;
    int frames_speed = 8; 

    SetTargetFPS(FPS);

    Camera2D camera = {0};
    camera.target = position;
    camera.offset = (Vector2){ 1200/2.0f, 900/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

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
        } else if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            helicopter_velocity.x = -HELI_SPEED;
            if (frame_rec.width > 0) {
                frame_rec.width = -frame_rec.width;
            }
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
        camera.offset = (Vector2){ 1200/2.0f, 900/2.0f };
        float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;

        // draw section
        BeginDrawing();
            if (is_in_menu) {
                ClearBackground(RAYWHITE);
                //DrawText("Helicopter", 10, 20, 50, WHITE);
                DrawTime();
                //DrawText(TextFormat("Elapsed Time: %d s", GetFrameTime()), 10, 20, 50, WHITE);
            }
            
            ClearBackground(BLACK);

            //DrawTexture(helicopter, 80, 71, WHITE);

            //DrawRectangleLines(80, 71, helicopter.width, helicopter.height, LIME);
            //DrawRectangleLines(80 + (int)helicopter.dest_rect.x, 80 + (int)helicopter.dest_rect.y, (int)helicopter.dest_rect.width, (int)helicopter.dest_rect.height, RED);

            //DrawText("FRAME SPEED: ", 165, 210, 10, DARKGRAY);
            //DrawText(TextFormat("%02i FPS", frames_speed), 575, 210, 10, DARKGRAY);
            //DrawText("PRESS RIGHT/LEFT KEYS to CHANGE SPEED!", 290, 240, 10, DARKGRAY);

            // for (int i = 0; i < MAX_FRAME_SPEED; i++)
            // {
            //     if (i < frames_speed) DrawRectangle(250 + 21*i, 205, 20, 20, RED);
            //     DrawRectangleLines(250 + 21*i, 205, 20, 20, MAROON);
            // }

            DrawTextureRec(helicopter_texture, frame_rec, position, WHITE);  // Draw part of the texture

        EndDrawing();
    }

    UnloadTexture(helicopter_texture);
    StopMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
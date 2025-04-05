#include <stdlib.h>

#include <raylib.h>

#define MAX_FRAME_SPEED 15
#define MIN_FRAME_SPEED  1

#define FPS 60

#define LINE_THICKNESS 2.5

typedef struct {
    Texture2D texture;
    Rectangle dest_rect;
    Vector2 velocity;
} helicopter_t;

void
move_helicopter(helicopter_t *helicopter)
{
    helicopter->velocity.x = 0.0f;

    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        helicopter->velocity.x = 100;
    }
    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        helicopter->dest_rect.x = -100;
    }
}

void
apply_gravity(helicopter_t *helicopter)
{
    helicopter->velocity.y += 10.0f;

    if (helicopter->velocity.y > 200.0f) {
        helicopter->velocity.y = 200.0f;
    }
}

void
apply_velocity(helicopter_t *helicopter)
{
    helicopter->velocity.x += helicopter->velocity.x * GetFrameTime();
    helicopter->velocity.y += helicopter->velocity.y * GetFrameTime();
}

int
main(void)
{
    InitWindow(1024, 768, "Helicopter");
    InitAudioDevice();

    // spritesheet
    // width: 226
    // height: 320
    // sprite: w 226 x h 80
    Texture2D helicopter_texture = LoadTexture("assets/helicopter-spritesheet.png");
    //Texture2D sprite = LoadTexture("heli-1.png");
    Sound sound = LoadSound("");
    Music music = LoadMusicStream("assets/sound/airwolf.mp3");
    //PlayMusicStream(music);

    Vector2 position = { 320.0f, 226.0f };
    Rectangle frame_rec = { 0.0f, 0.0f, (float)helicopter_texture.width, (float)helicopter_texture.height/4 };
    int current_frame = 0;

    int frames_counter = 0;
    int frames_speed = 8; 

    SetTargetFPS(FPS);

    helicopter_t helicopter = (helicopter_t) {
      .texture = helicopter_texture,
    //   .dest_rect = (Rectangle) {
    //     .x = 10.0f,
    //     .y = 100.0f,
    //     .width = 100.0f,
    //     .height = 100.0f
    //   }
        .dest_rect = (Rectangle) {
            0.0f, 0.0f,
            (float)helicopter_texture.width,
            (float)helicopter_texture.height/4
        }
    };

    Camera2D camera = {0};
    camera.target = position;
    camera.offset = (Vector2){ 1024/2.0f, 768/2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;

    while (!WindowShouldClose()) {
        // update section

        if (IsKeyPressed(KEY_M)) {
            // TODO: look into volume adjustment instead
            if (IsMusicStreamPlaying(music)) {
                StopMusicStream(music);
            } else {
                PlayMusicStream(music);
            }
        }

        UpdateMusicStream(music);

        frames_counter++;

        if (frames_counter >= (FPS/frames_speed)) {
            frames_counter = 0;
            current_frame++;

            if (current_frame > 3) {
                current_frame = 0;
            }

            helicopter.dest_rect.y = (float)current_frame*(float)helicopter_texture.height/4;
        }

        move_helicopter(&helicopter);
        apply_gravity(&helicopter);
        apply_velocity(&helicopter);

        // Control frames speed
        if (IsKeyPressed(KEY_RIGHT)) {
            frames_speed++;
        } else if (IsKeyPressed(KEY_LEFT)) {
            frames_speed--;
        }

        if (frames_speed > MAX_FRAME_SPEED) {
            frames_speed = MAX_FRAME_SPEED;
        } else if (frames_speed < MIN_FRAME_SPEED) {
            frames_speed = MIN_FRAME_SPEED;
        }

        camera.target = position;
        camera.offset = (Vector2){ 1024/2.0f, 768/2.0f };
        float minX = 1000, minY = 1000, maxX = -1000, maxY = -1000;

        // draw section
        BeginDrawing();
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

            //DrawTextureRec(helicopter_texture, helicopter.dest_rect, position, WHITE);  // Draw part of the texture
            DrawTextureRec(helicopter.texture, helicopter.dest_rect, position, WHITE);  // Draw part of the texture
            //DrawTexturePro(helicopter.texture, (Rectangle){0, 0, 64, 64}, helicopter.dest_rect, (Vector2){80, 71}, 0, WHITE);

        EndDrawing();
    }

    UnloadTexture(helicopter_texture);
    StopMusicStream(music);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
#include <stdlib.h>

#include <raylib.h>

#include "raylib-tmx.h"

#define MAX_FRAME_SPEED 15
#define MIN_FRAME_SPEED  1

#define FPS 60

#define LINE_THICKNESS 2.5

void draw_polyline(double offset_x, double offset_y, double **points, int points_count, Color color) {
  int i;
  for (i=1; i<points_count; i++) {
    DrawLineEx((Vector2){offset_x + points[i-1][0], offset_y + points[i-1][1]},
               (Vector2){offset_x + points[i][0], offset_y + points[i][1]},
               LINE_THICKNESS, color);
  }
}

void draw_polygon(double offset_x, double offset_y, double **points, int points_count, Color color) {
  draw_polyline(offset_x, offset_y, points, points_count, color);
  if (points_count > 2) {
    DrawLineEx((Vector2){offset_x + points[0][0], offset_y + points[0][1]},
               (Vector2){offset_x + points[points_count-1][0], offset_y + points[points_count-1][1]},
               LINE_THICKNESS, color);
  }
}

void draw_objects(tmx_object_group *objgr) {
  tmx_object *head = objgr->head;
  Color color = int_to_color(objgr->color);

  while (head) {
    if (head->visible) {
      if (head->obj_type == OT_SQUARE) {
        DrawRectangleLinesEx((Rectangle){head->x, head->y, head->width, head->height}, LINE_THICKNESS, color);
      }
      else if (head->obj_type  == OT_POLYGON) {
        draw_polygon(head->x, head->y, head->content.shape->points, head->content.shape->points_len, color);
      }
      else if (head->obj_type == OT_POLYLINE) {
        draw_polyline(head->x, head->y, head->content.shape->points, head->content.shape->points_len, color);
      }
      else if (head->obj_type == OT_ELLIPSE) {
        DrawEllipseLines(head->x + head->width/2.0, head->y + head->height/2.0, head->width/2.0, head->height/2.0, color);
      }
    }
    head = head->next;
  }
}

void draw_tile(void *image, unsigned int sx, unsigned int sy, unsigned int sw, unsigned int sh,
    unsigned int dx, unsigned int dy, float opacity, unsigned int flags) {
DrawTextureRec((Texture2D)image, (Rectangle) {sx, sy, sw, sh}, (Vector2) {dx, dy}, (Color) {opacity, opacity, opacity, opacity});
}

void draw_layer(tmx_map *map, tmx_layer *layer) {
    unsigned long i, j;
    unsigned int gid, x, y, w, h, flags;
    float op;
    tmx_tileset *ts;
    tmx_image *im;
    void* image;
    op = layer->opacity;
    for (i=0; i<map->height; i++) {
      for (j=0; j<map->width; j++) {
        gid = (layer->content.gids[(i*map->width)+j]) & TMX_FLIP_BITS_REMOVAL;
        if (map->tiles[gid] != NULL) {
          ts = map->tiles[gid]->tileset;
          im = map->tiles[gid]->image;
          x  = map->tiles[gid]->ul_x;
          y  = map->tiles[gid]->ul_y;
          w  = ts->tile_width;
          h  = ts->tile_height;
          if (im) {
            image = im->resource_image;
          }
          else {
            image = ts->image->resource_image;
          }
          flags = (layer->content.gids[(i*map->width)+j]) & ~TMX_FLIP_BITS_REMOVAL;
          draw_tile(image, x, y, w, h, j*ts->tile_width, i*ts->tile_height, op, flags); // Function to be implemented
        }
      }
    }
  }

void draw_image_layer(tmx_image *image) {
    Texture2D *texture = (Texture2D*)image->resource_image;
    DrawTexture(*texture, 0, 0, WHITE);
  }

void draw_all_layers(tmx_map *map, tmx_layer *layers) {
    while (layers) {
      if (layers->visible) {
  
        if (layers->type == L_GROUP) {
          draw_all_layers(map, layers->content.group_head); // recursive call
        }
        else if (layers->type == L_OBJGR) {
          draw_objects(layers->content.objgr); // Function to be implemented
        }
        else if (layers->type == L_IMAGE) {
          draw_image_layer(layers->content.image); // Function to be implemented
        }
        else if (layers->type == L_LAYER) {
          draw_layer(map, layers); // Function to be implemented
        }
      }
      layers = layers->next;
    }
  }

  
Color int_to_color(int color) {
    tmx_col_bytes res = tmx_col_to_bytes(color);
    return *((Color*)&res);
  }
  
  void render_map(tmx_map *map) {
    ClearBackground(int_to_color(map->backgroundcolor));
    draw_all_layers(map, map->ly_head); // Function to be implemented
  }

  void* raylib_tex_loader(const char *path) {
    Texture2D *returnValue = malloc(sizeof(Texture2D));
    *returnValue = LoadTexture(path);
    return returnValue;
  }
  
  void raylib_free_tex(void *ptr)
  {
    UnloadTexture(*((Texture2D*)ptr));
    free(ptr);
  }

int
main(void)
{
    InitWindow(800, 450, "Helicopter");
    InitAudioDevice();

    tmx_map *map = tmx_load("helicopter.tmx");
    tmx_img_load_func = raylib_tex_loader;
    tmx_img_free_func = raylib_free_tex;

    // spritesheet
    // width: 226
    // height: 320
    // sprite: w 226 x h 80
    Texture2D helicopter = LoadTexture("helicopter-spritesheet.png");
    //Texture2D sprite = LoadTexture("heli-1.png");
    Sound sound = LoadSound("");
    Music music = LoadMusicStream("airwolf.mp3");
    //PlayMusicStream(music);

    Vector2 position = { 320.0f, 226.0f };
    Rectangle frame_rec = { 0.0f, 0.0f, (float)helicopter.width, (float)helicopter.height/4 };
    int current_frame = 0;

    int frames_counter = 0;
    int frames_speed = 8; 

    SetTargetFPS(FPS);

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

            frame_rec.y = (float)current_frame*(float)helicopter.height/4;
        }

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

        // draw section
        BeginDrawing();
            ClearBackground(BLACK);
            DrawTMX(map, 0, 0, WHITE);

            //DrawTexture(helicopter, 80, 71, WHITE);

            //DrawRectangleLines(80, 71, helicopter.width, helicopter.height, LIME);
            //DrawRectangleLines(80 + (int)frame_rec.x, 80 + (int)frame_rec.y, (int)frame_rec.width, (int)frame_rec.height, RED);

            //DrawText("FRAME SPEED: ", 165, 210, 10, DARKGRAY);
            //DrawText(TextFormat("%02i FPS", frames_speed), 575, 210, 10, DARKGRAY);
            //DrawText("PRESS RIGHT/LEFT KEYS to CHANGE SPEED!", 290, 240, 10, DARKGRAY);

            // for (int i = 0; i < MAX_FRAME_SPEED; i++)
            // {
            //     if (i < frames_speed) DrawRectangle(250 + 21*i, 205, 20, 20, RED);
            //     DrawRectangleLines(250 + 21*i, 205, 20, 20, MAROON);
            // }

            DrawTextureRec(helicopter, frame_rec, position, WHITE);  // Draw part of the texture

        EndDrawing();
    }

    UnloadTexture(helicopter);
    StopMusicStream(music);
    CloseAudioDevice();
    tmx_map_free(map);
    CloseWindow();

    return 0;
}
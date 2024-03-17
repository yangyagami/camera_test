#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <pthread.h>

#include <raylib.h>
#include <libuvc/libuvc.h>

#include "camera.h"
#include "button.h"

int main() {
  const int screenWidth = 800;
  const int screenHeight = 600;

  InitWindow(screenWidth, screenHeight, "Camera example");

  SetTargetFPS(60);

  camera_ptr_t camera = NULL;

  if (camera_create(&camera, 0x1187, 0x3a49) != UVC_SUCCESS) {
    goto finish;
  }
  
  camera_init(camera, 640, 480, 30);

  if (camera_open(camera) != UVC_SUCCESS) {
    goto finish;
  }

  button_t open_btn = button_create("Open", 20, screenHeight - 40, 100, 30);
  button_t close_btn = button_create("Close", 130, screenHeight - 40, 100, 30);

  while (!WindowShouldClose()) {
    Image frame;
    Texture2D frame_texture;

    if (button_is_clicked(&open_btn) && !camera_opened(camera)) {
      camera_open(camera);
    }
    if (button_is_clicked(&close_btn)) {
      camera_close(camera);
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);

    int release = 0;

    if (camera_opened(camera)) {

      pthread_mutex_lock(&camera->mutex);


      frame = LoadImageFromMemory(
          ".jpeg",
          (const unsigned char *)camera->frame.buffer,
          camera->frame.size);
      frame_texture = LoadTextureFromImage(frame);

      pthread_mutex_unlock(&camera->mutex);

      DrawTexture(frame_texture, 0, 0, WHITE);

      button_draw(&open_btn);
      button_draw(&close_btn);

      release = 1;

    }
    EndDrawing();

    if (release) {
      UnloadImage(frame);
      UnloadTexture(frame_texture);
    }

  }

finish:
  camera_close(camera);
  camera_destroy(camera);
  CloseWindow();

  return 0;
}


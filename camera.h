#ifndef CAMERA_TEST_CAMERA_H_
#define CAMERA_TEST_CAMERA_H_

#include <pthread.h>

#include <libuvc/libuvc.h>

struct camera_frame;
struct camera;

typedef struct camera_frame camera_frame_t;
typedef struct camera camera_t;
typedef camera_t* camera_ptr_t;

struct camera_frame {
  char *buffer;
  size_t size;
};

struct camera {
  uvc_context_t *ctx;
  uvc_device_t *dev;
  uvc_device_handle_t *devh;
  uvc_stream_ctrl_t ctrl;

  int vid;
  int pid;

  int width;
  int height;
  int fps;

  camera_frame_t frame;

  pthread_mutex_t mutex;
};

int camera_create(camera_ptr_t *camera, int vid, int pid);
void camera_init(camera_ptr_t camera, int width, int height, int fps);
int camera_open(camera_ptr_t camera);
int camera_opened(camera_ptr_t camera);
void camera_close(camera_ptr_t camera);
void camera_destroy(camera_ptr_t camera);

#endif  // CAMERA_TEST_CAMERA_H_

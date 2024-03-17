#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "camera.h"

// Auto return false
#define CAMERA_TEST_LIBUVC_SAFE_CALL(function, ...) \
do { \
 uvc_error_t res; \
 res = (function)(__VA_ARGS__); \
 if (res < 0) { \
   uvc_perror(res, #function); \
   return res; \
 } \
} while (0)

static void cb(uvc_frame_t *frame, void *ptr) {
  camera_ptr_t camera = (camera_ptr_t)ptr;

  pthread_mutex_lock(&camera->mutex);

  if (camera->frame.size != 0) {
    free(camera->frame.buffer); 
    camera->frame.size = 0;
  }

  camera->frame.buffer = (char *)malloc(frame->data_bytes);
  memcpy(camera->frame.buffer, frame->data, frame->data_bytes);
  camera->frame.size = frame->data_bytes;

  pthread_mutex_unlock(&camera->mutex);

  [[maybe_unused]]
  enum uvc_frame_format *frame_format = (enum uvc_frame_format *)ptr;

  printf("callback! frame_format = %d, width = %d, height = %d, length = %lu, ptr = %p\n",
      frame->frame_format, frame->width, frame->height, frame->data_bytes, ptr);

  if (frame->sequence % 30 == 0) {
    printf(" * got image %u\n",  frame->sequence);
  }
}

int camera_create(camera_ptr_t *camera, int vid, int pid) {
  camera_ptr_t obj = NULL;
  obj = (camera_ptr_t)calloc(1, sizeof(*obj));
  *camera = obj;

  CAMERA_TEST_LIBUVC_SAFE_CALL(uvc_init, &obj->ctx, NULL);

  printf("UVC initialized\n");

  obj->vid = vid;
  obj->pid = pid;

  pthread_mutex_init(&obj->mutex, NULL);

  return UVC_SUCCESS;
}

void camera_init(camera_ptr_t camera, int width, int height, int fps) {
  camera->width = width;
  camera->height = height;
  camera->fps = fps;
}

int camera_open(camera_ptr_t camera) {
  // 说明相机硬件中途断开,再次打开需要释放资源。
  if (camera->dev != NULL) {
    uvc_unref_device(camera->dev);
    camera->dev = NULL;
  }

  CAMERA_TEST_LIBUVC_SAFE_CALL(uvc_find_device, camera->ctx,
                               &camera->dev, camera->vid, camera->pid, NULL);

  printf("Device founded\n");

  CAMERA_TEST_LIBUVC_SAFE_CALL(uvc_open, camera->dev, &camera->devh);

  printf("Device opened\n");

  uvc_print_diag(camera->devh, stderr);

  [[maybe_unused]]
  const uvc_format_desc_t *format_desc = uvc_get_format_descs(camera->devh);
  [[maybe_unused]]
  const uvc_frame_desc_t *frame_desc = format_desc->frame_descs;
  [[maybe_unused]]
  enum uvc_frame_format frame_format;

  switch (format_desc->bDescriptorSubtype) {
    case UVC_VS_FORMAT_MJPEG:
      frame_format = UVC_COLOR_FORMAT_MJPEG;
      break;
    case UVC_VS_FORMAT_FRAME_BASED:
      frame_format = UVC_FRAME_FORMAT_H264;
      break;
    default:
      frame_format = UVC_FRAME_FORMAT_YUYV;
      break;
  }

  CAMERA_TEST_LIBUVC_SAFE_CALL(
      uvc_get_stream_ctrl_format_size,
      camera->devh,
      &camera->ctrl,
      frame_format,
      camera->width,
      camera->height,
      camera->fps);

  uvc_print_stream_ctrl(&camera->ctrl, stderr);

  CAMERA_TEST_LIBUVC_SAFE_CALL(
      uvc_start_streaming,
      camera->devh,
      &camera->ctrl,
      cb,
      (void *) camera,
      0);

  printf("Streaming...\n");

  return UVC_SUCCESS;
}

void camera_close(camera_ptr_t camera) {
  if (camera->devh != NULL && camera_opened(camera)) {
    uvc_stop_streaming(camera->devh);
    uvc_close(camera->devh);
    printf("Device closed\n");
    camera->devh = NULL;
  }
  if (camera->dev != NULL) {
    uvc_unref_device(camera->dev);
    camera->dev = NULL;
  }
}

int camera_opened(camera_ptr_t camera) {
  if (camera->ctx != NULL) {
    return uvc_device_opened(camera->ctx, camera->devh);
  }
  return 0;
}

void camera_destroy(camera_ptr_t camera) {
  // TODO(yangsiyu): stop stream
  camera_close(camera);

  if (camera->ctx != NULL) {
    uvc_exit(camera->ctx);
    camera->ctx = NULL; 
  }

  if (camera->frame.size != 0) {
    free(camera->frame.buffer); 
    camera->frame.size = 0;
  }

  pthread_mutex_destroy(&camera->mutex);

  free(camera);

  printf("UVC exited\n");
}

#undef CAMERA_TEST_LIBUVC_ERROR_HANDLE

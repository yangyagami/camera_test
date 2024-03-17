#ifndef CAMERA_TEST_BUTTON_H_
#define CAMERA_TEST_BUTTON_H_

struct button;

typedef struct button button_t;
typedef button_t* button_ptr_t;

struct button {
  const char *label_str;
  int x;
  int y;
  int width;
  int height;
};

button_t button_create(
    const char *label_str,
    int x,
    int y,
    int width,
    int height);

int button_is_clicked(button_ptr_t btn);

void button_draw(button_ptr_t btn);

#endif

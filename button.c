#include "button.h"

#include "raylib.h"

button_t button_create(
    const char *label_str,
    int x,
    int y,
    int width,
    int height) {
  button_t btn = {
      .label_str = label_str,
      .x = x,
      .y = y,
      .width = width,
      .height = height };

  return btn;
}

int button_is_clicked(button_ptr_t btn) {
  int x = GetMouseX();
  int y = GetMouseY();
  if (x > btn->x && x < btn->x + btn->width &&
      y > btn->y && y < btn->y + btn->height &&
      IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    return 1; 
  }
  return 0;
}

void button_draw(button_ptr_t btn) {
  DrawRectangle(btn->x, btn->y, btn->width, btn->height, GRAY);
}

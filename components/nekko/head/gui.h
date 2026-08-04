#pragma once

class Nengine {
public:
  Nengine();
  ~Nengine();

  void nengine_init();
  void nengine_window();
  void nengine_clear();
  void nengine_present();

  void nengine_draw_rect();
  void nengine_draw_tex();
  void nengine_draw_text();

  void nengine_spawn_audio();
  void nengine_spawn_signal();

private:
  void* w;
  void* r;
};

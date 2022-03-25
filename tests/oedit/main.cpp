

#include "oedit.h"

#include "imgui_utils.h"

class ImgUIoeditor : public imGuiDefaultApp {
  oeditor oedit;

 public:
  ImgUIoeditor() {}
  bool mainloop_tick() override {
    oedit.draw();
    return true;
  }
};

int main() {
  ImgUIoeditor app;
  app.mainloop();
}
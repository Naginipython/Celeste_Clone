#include "main.h"
#include "window.h"

int main(int argc, char* argv[]) {
  Window window{1200, 720, "Celeste Clone"};
  
  while (window.isRunning) {
    window.update();
  }
  return 0;
}
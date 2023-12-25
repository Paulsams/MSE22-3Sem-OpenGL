#include "scenes.h"

int main() {
    auto [scene, cam] = scenes::createSpheresScene();

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1920;
    cam.samples_per_pixel = 30;
    cam.max_depth         = 50;

    cam.render(scene);
}

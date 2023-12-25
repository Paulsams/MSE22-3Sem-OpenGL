#pragma once

#include "utils.h"
#include "Hittables/hittable_list.h"
#include "Materials/material.h"
#include "LoaderModels/LoaderModel.h"
#include "camera.h"
#include "Hittables/sphere.h"

namespace scenes {
    inline std::pair<hittable_list, camera> createSpheresScene() {
        hittable_list world;

        auto ground_material = std::make_shared<metal>(color(0.9, 0.5, 0.2), 1.0);
        auto ground_model = LoaderModel::load(vec3(), "Models/Plane.glb");
        ground_model[0]->setMaterial(ground_material);
        world.add(ground_model);

        for (int a = -11; a < 11; a++) {
            for (int b = -11; b < 11; b++) {
                auto choose_mat = random_double();
                point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

                if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                    std::shared_ptr<material> sphere_material;

                    if (choose_mat < 0.8) {
                        // diffuse
                        auto albedo = color::random() * color::random();
                        sphere_material = std::make_shared<lambertian>(albedo);
                        world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                    } else if (choose_mat < 0.95) {
                        // metal
                        auto albedo = color::random(0.5, 1);
                        auto fuzz = random_double(0, 0.5);
                        sphere_material = std::make_shared<metal>(albedo, fuzz);
                        world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                    } else {
                        // glass
                        sphere_material = std::make_shared<dielectric>(1.5);
                        world.add(std::make_shared<sphere>(center, 0.2, sphere_material));
                    }
                }
            }
        }

        auto material1 = std::make_shared<dielectric>(1.5);
        world.add(std::make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

        auto material2 = std::make_shared<lambertian>(color(0.4, 0.2, 0.1));
        world.add(std::make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

        auto material3 = std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
        world.add(std::make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

        world.add(ground_model);

        camera cam;

        cam.vfov     = 20;
        cam.lookfrom = point3(13, 2, 3);
        cam.lookat   = point3(0, 0, 0);
        cam.vup      = vec3(0, 1, 0);

        cam.defocus_angle = 0.6;
        cam.focus_dist    = 10.0;

        return {world, cam};
    }

    inline std::pair<hittable_list, camera> createModelScene() {
        hittable_list world;

        auto ground_material = std::make_shared<metal>(color(0.9, 0.5, 0.2), 1.0);
        auto ground_model = LoaderModel::load(vec3(), "Models/Plane.glb");
        ground_model[0]->setMaterial(ground_material);
        world.add(ground_model);

        auto loaded_model_1 = LoaderModel::load(vec3(0, 0, -2), "Models/Duck.glb");
        auto material1 = std::make_shared<dielectric>(1.5);
        loaded_model_1[0]->setMaterial(material1);

        auto loaded_model_2 = LoaderModel::load(vec3(0, 0,  0), "Models/Duck.glb");
        auto material2 = std::make_shared<lambertian>(color(0.4, 0.2, 0.1));
        loaded_model_2[0]->setMaterial(material2);

        auto loaded_model_3 = LoaderModel::load(vec3(0, 0,  2), "Models/Duck.glb");
        auto material3 = std::make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
        loaded_model_3[0]->setMaterial(material3);

        world.add(loaded_model_1);
        world.add(loaded_model_2);
        world.add(loaded_model_3);

        camera cam;

        cam.vfov     = 60;
        cam.lookfrom = point3(2.5, 1.2, 2.3);
        cam.lookat   = point3(0, 0, 0);
        cam.vup      = vec3(0, 1, 0);

        cam.defocus_angle = 0.6;
        cam.focus_dist    = 10.0;

        return {world, cam};
    }
}

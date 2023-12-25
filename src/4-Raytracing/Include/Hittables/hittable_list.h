#pragma once

#include "hittable.h"

#include <memory>
#include <vector>

class hittable_list : public hittable {
public:
    std::vector<std::shared_ptr<hittable>> _objects;

    hittable_list() = default;
    explicit hittable_list(const std::shared_ptr<hittable>& object) { add(object); }

    void clear() { _objects.clear(); }

    void add(const std::shared_ptr<hittable>& object) {
        _objects.push_back(object);
    }

    template<class THittable>
    void add(const std::vector<std::shared_ptr<THittable>>& objects) {
        _objects.insert(_objects.end(), objects.begin(), objects.end());
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object: _objects) {
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }
};

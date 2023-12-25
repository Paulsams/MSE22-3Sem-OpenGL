#pragma once

#include "hittable.h"
#include "Structures/triangle3.h"

class model : public hittable {
public:
    using index_type = unsigned short;

    model(std::vector<vec3>&& points, std::vector<vec3>&& normals,
          std::vector<index_type>&& indices, const std::shared_ptr<material>& material)
            : _points(std::move(points)), _normals(std::move(normals)), _indices(std::move(indices)), _material(material) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        if (_indices.empty()) {
            for (size_t i = 0; i < _points.size(); i += 3) {
                triangle3 triangle = triangle3(_points[i], _points[i + 1], _points[i + 2], _normals[i]);
                if (hit_triangle(r, interval(ray_t.min, closest_so_far), triangle, temp_rec)) {
                    hit_anything = true;
                    closest_so_far = temp_rec.t;
                    rec = temp_rec;
                }
            }
        } else {
            for (size_t i = 0; i < _indices.size(); i += 3) {
                triangle3 triangle = triangle3(
                        _points[_indices[i]],
                        _points[_indices[i + 1]],
                        _points[_indices[i + 2]],
                        _normals[_indices[i]]
                );
                if (hit_triangle(r, interval(ray_t.min, closest_so_far), triangle, temp_rec)) {
                    hit_anything = true;
                    closest_so_far = temp_rec.t;
                    rec = temp_rec;
                }
            }
        }

        return hit_anything;
    }

    void setMaterial(const std::shared_ptr<material>& material) {
        _material = material;
    }

private:
    std::vector<vec3> _points;
    std::vector<vec3> _normals;
    std::vector<index_type> _indices;
    std::shared_ptr<material> _material;

    bool hit_triangle(
            const ray& r, interval ray_t,
            const triangle3& triangle, hit_record& rec
    ) const {
        constexpr double epsilon = std::numeric_limits<double>::epsilon();

        vec3 edge1 = triangle.b - triangle.a;
        vec3 edge2 = triangle.c - triangle.a;
        vec3 ray_cross_e2 = cross(r.direction(), edge2);
        double det = dot(edge1, ray_cross_e2);

        if (det > -epsilon && det < epsilon)
            return false;    // This ray is parallel to this triangle.

        double inv_det = 1.0 / det;
        vec3 s = r.origin() - triangle.a;
        double u = inv_det * dot(s, ray_cross_e2);

        if (u < 0 || u > 1)
            return false;

        vec3 s_cross_e1 = cross(s, edge1);
        double v = inv_det * dot(r.direction(), s_cross_e1);

        if (v < 0 || u + v > 1)
            return false;

        // At this stage we can compute t to find out where the intersection point is on the line.
        double t = inv_det * dot(edge2, s_cross_e1);

        if (!ray_t.surrounds(t))
            return false;

        rec.t = t;
        rec.p = r.at(rec.t);
        // TODO: интерполировать нормали в идеале бы
        vec3 outward_normal = unit_vector(cross(edge1, edge2));
        rec.set_face_normal(r, outward_normal);
        rec.mat = _material;

        return true;
    }
};

#include <math.h>
#include <stdio.h>

#define IMAGE_HEIGHT 1080
#define IMAGE_WIDTH 1920
#define SEGMENTS 120

struct point_f {
    float x_;
    float y_;
};

struct color {
    float r_;
    float g_;
    float b_;
};

struct cube_bezier_curve {
    struct color curve_color_;
    struct point_f point_[4];
};

struct point_f get_bezier_point(float t, const struct cube_bezier_curve *curve);
float distance_points(const struct point_f *a, const struct point_f *b);
struct color get_color(const struct point_f *point,
                       const struct point_f bezier_curve_points[],
                       const struct color *bezier_curve_color,
                       int segments);

int main(void) {
    struct point_f bezier_curve_point_segments[SEGMENTS];

    struct cube_bezier_curve curve = {
        {0.10f, 0.55f, 0.95f},
        {{0.08f, 0.85f}, {0.25f, 0.10f}, {0.75f, 0.95f}, {0.92f, 0.20f}}
    };

    for (int i = 0; i < SEGMENTS; ++i) {
        float t = (float)i / (SEGMENTS - 1);
        bezier_curve_point_segments[i] = get_bezier_point(t, &curve);
    }

    printf("P3\n%d %d\n255\n", IMAGE_WIDTH, IMAGE_HEIGHT);

    for (int j = 0; j < IMAGE_HEIGHT; ++j) {
        for (int i = 0; i < IMAGE_WIDTH; ++i) {
            struct point_f point = {
                (float)i / IMAGE_WIDTH,
                (float)j / IMAGE_HEIGHT
            };

            struct color pixel_color = get_color(
                &point,
                bezier_curve_point_segments,
                &curve.curve_color_,
                SEGMENTS
            );

            int ri = (int)(pixel_color.r_ * 255.999f);
            int gi = (int)(pixel_color.g_ * 255.999f);
            int bi = (int)(pixel_color.b_ * 255.999f);

            printf("%d %d %d\n", ri, gi, bi);
        }
    }

    return 0;
}

struct point_f get_bezier_point(float t, const struct cube_bezier_curve *curve) {
    float a = 1.0f - t;
    float a2 = a * a;
    float a3 = a2 * a;
    float t2 = t * t;
    float t3 = t2 * t;

    struct point_f point;

    point.x_ = a3 * curve->point_[0].x_
             + 3.0f * a2 * t * curve->point_[1].x_
             + 3.0f * a * t2 * curve->point_[2].x_
             + t3 * curve->point_[3].x_;

    point.y_ = a3 * curve->point_[0].y_
             + 3.0f * a2 * t * curve->point_[1].y_
             + 3.0f * a * t2 * curve->point_[2].y_
             + t3 * curve->point_[3].y_;

    return point;
}

float distance_points(const struct point_f *a, const struct point_f *b) {
    float dx = a->x_ - b->x_;
    float dy = a->y_ - b->y_;
    return sqrtf(dx * dx + dy * dy);
}

struct color get_color(const struct point_f *point,
                       const struct point_f bezier_curve_points[],
                       const struct color *bezier_curve_color,
                       int segments) {
    float dist = 1.0f;

    for (int i = 0; i < segments; ++i) {
        float current_dist = distance_points(point, &bezier_curve_points[i]);
        if (current_dist < dist) {
            dist = current_dist;
        }
    }

    struct color background = {0.96f, 0.93f, 0.88f};
    float sigma = 0.035f;
    float weight = expf(-(dist * dist) / (2.0f * sigma * sigma));

    struct color pixel_color = {
        background.r_ * (1.0f - weight) + bezier_curve_color->r_ * weight,
        background.g_ * (1.0f - weight) + bezier_curve_color->g_ * weight,
        background.b_ * (1.0f - weight) + bezier_curve_color->b_ * weight
    };

    return pixel_color;
}
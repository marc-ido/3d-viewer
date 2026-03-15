#pragma once
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// =============================================================================
//  Vec3  —  simple 3-component vector
// =============================================================================
struct Vec3 {
    float x, y, z;
    Vec3(float x = 0.f, float y = 0.f, float z = 0.f) : x(x), y(y), z(z) {}

    Vec3  operator+(const Vec3& o) const { return { x + o.x, y + o.y, z + o.z }; }
    Vec3  operator-(const Vec3& o) const { return { x - o.x, y - o.y, z - o.z }; }
    Vec3  operator*(float s)       const { return { x * s,   y * s,   z * s   }; }

    float dot  (const Vec3& o) const { return x*o.x + y*o.y + z*o.z; }
    Vec3  cross(const Vec3& o) const {
        return { y*o.z - z*o.y, z*o.x - x*o.z, x*o.y - y*o.x };
    }
    Vec3 normalize() const {
        float len = std::sqrt(x*x + y*y + z*z);
        if (len < 1e-8f) return { 0.f, 1.f, 0.f };
        return { x/len, y/len, z/len };
    }
};

// =============================================================================
//  Mat4  —  column-major 4×4 matrix (OpenGL / GLSL convention)
//
//  Memory layout:  m[col * 4 + row]
//  i.e. m[0..3] = column 0, m[4..7] = column 1, …
// =============================================================================
struct Mat4 {
    float m[16];
    // Default-constructed to identity
    Mat4() {
        for (int i = 0; i < 16; i++) m[i] = 0.f;
        m[0] = m[5] = m[10] = m[15] = 1.f;
    }
};

// Declarations — definitions live in source.cpp
Mat4 mat4Multiply   (const Mat4& a, const Mat4& b);
Mat4 mat4Perspective(float fovY_rad, float aspect, float znear, float zfar);
Mat4 mat4LookAt     (Vec3 eye, Vec3 center, Vec3 up);

// =============================================================================
//  Camera  —  spherical-coordinate orbit camera
// =============================================================================
class Camera {
public:
    float azimuth;    ///< Horizontal angle around Y-axis (radians)
    float elevation;  ///< Vertical   angle above XZ-plane (radians)
    float distance;   ///< Distance from the look-at origin
    float aspect;     ///< Viewport width / height

    explicit Camera(float aspect = 1.f)
        : azimuth(0.6f), elevation(0.45f), distance(5.f), aspect(aspect) {}

    /// World-space eye position derived from spherical coordinates
    /// Uses Z-up convention: X=right, Y=depth, Z=up
    Vec3 getEye() const {
        float cosEl = std::cos(elevation);
        return Vec3(
            distance * cosEl * std::cos(azimuth),  // X
            distance * cosEl * std::sin(azimuth),  // Y
            distance * std::sin(elevation)          // Z (up)
        );
    }

    Mat4 getViewMatrix()       const { return mat4LookAt(getEye(), {0,0,0}, {0,0,1}); }
    Mat4 getProjectionMatrix() const {
        return mat4Perspective(45.f * (float)M_PI / 180.f, aspect, 0.1f, 200.f);
    }

    /// @param dAz  pixel-space horizontal delta (will be scaled internally)
    /// @param dEl  pixel-space vertical   delta
    void orbit(float dAz, float dEl) {
        azimuth   += dAz;
        elevation += dEl;
        const float limit = (float)M_PI * 0.49f;   // ~88°
        if (elevation >  limit) elevation =  limit;
        if (elevation < -limit) elevation = -limit;
    }

    void zoom(float delta) {
        distance += delta;
        if (distance <  0.5f)  distance =  0.5f;
        if (distance > 100.f)  distance = 100.f;
    }
};

#pragma once
#include <cmath>
#include <cassert>


namespace nu {
    struct Vector3
    {
        union {
            struct { float x, y, z; };
            struct { float r, g, b; };

        };
        
        Vector3() = default;
        Vector3(float x, float y, float z) : x{ x }, y{ y }, z{ z } {}
        explicit Vector3(float a) : x{ a }, y{ a }, z{ a } {}



        float operator [] (unsigned int i) const {
            assert(i < 3);
            return (&x)[i];
        }

        float& operator [] (unsigned int i) {
            assert(i < 3);
            return (&x)[i];
        }

        bool operator == (const Vector3& v) const {
            return x == v.x && y == v.y && z == v.z;
        }

        bool operator != (const Vector3& v) const {
            return !(*this == v);
        }

        Vector3 operator + (const Vector3& v) const {
            return Vector3{ x + v.x, y + v.y, z + v.z };
        }

        Vector3 operator - (const Vector3& v) const {
            return Vector3{ x - v.x, y - v.y, z - v.z };
        }

        Vector3 operator * (const Vector3& v) const {
            return Vector3{ x * v.x, y * v.y, z * v.z };
        }

        Vector3 operator / (const Vector3& v) const {
            return Vector3{ x / v.x, y / v.y, z / v.z };
        }

        Vector3 operator + (float v) const {
            return Vector3{ x + v, y + v, z + v };
        }

        Vector3 operator - (float v) const {
            return Vector3{ x - v, y - v, z - v };
        }

        Vector3 operator * (float v) const {
            return Vector3{ x * v, y * v, z * v };
        }

        Vector3 operator / (float v) const {
            return Vector3{ x / v, y / v, z / v };
        }

        Vector3& operator += (const Vector3& v) {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        Vector3& operator -= (const Vector3& v) {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        Vector3& operator *= (const Vector3& v) {
            x *= v.x;
            y *= v.y;
            z *= v.z;
            return *this;
        }

        Vector3& operator /= (const Vector3& v) {
            x /= v.x;
            y /= v.y;
            z /= v.z;
            return *this;
        }

        Vector3& operator += (float v) {
            x += v;
            y += v;
            z += v;
            return *this;
        }

        Vector3& operator -= (float v) {
            x -= v;
            y -= v;
            z -= v;
            return *this;
        }

        Vector3& operator *= (float v) {
            x *= v;
            y *= v;
            z *= v;
            return *this;
        }

        Vector3& operator /= (float v) {
            x /= v;
            y /= v;
            z /= v;
            return *this;
        }

        float Length() const {
            return sqrtf((float)pow(x, 2) + (float)pow(y,2) + (float)pow(z,2));
        }

        float LengthSq() const {
            return (float)pow(x,2) + (float)pow(y,2) + (float)pow(z,2);
        }

        Vector3 Normalized() const {
            return *this / Length();
        }

        

    };
    using Color = Vector3;
}

#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"

#include <vecmath.h>
#include <float.h>
#include <cmath>

class Camera
{
public:
    virtual ~Camera() {}

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point, bool jitter, int width, int height) = 0;
    virtual float getTMin() const = 0;
};

/// Fill in functions and add more fields if necessary
class PerspectiveCamera : public Camera
{
public:
    PerspectiveCamera(const Vector3f &center,
        const Vector3f &direction,
        const Vector3f &up,
        float angleradians) :
        _center(center),
        _direction(direction.normalized()),
        _up(up),
        _angle(angleradians)
    {
        _horizontal = Vector3f::cross(direction, up).normalized();
    }

    virtual Ray generateRay(const Vector2f &point, bool jitter, int width, int height) override // TODO: modify to add jitter
    {
        // BEGIN STARTER
        float d = 1.0f / (float)std::tan(_angle / 2.0f);

        float xJitter = 0;
        float yJitter = 0;
        if (jitter) {
            float pixelWidth = 2.0f / (width - 1);
            float pixelHeight = 2.0f / (height - 1);
            xJitter = ((float) rand() / RAND_MAX) * pixelWidth - pixelWidth/2;
            yJitter = ((float) rand() / RAND_MAX) * pixelHeight - pixelHeight/2;
        }

        Vector3f newDir = d * _direction + (point[0] + xJitter) * _horizontal + (point[1] + yJitter) * _up;
        newDir = newDir.normalized();

        return Ray(_center, newDir);
        // END STARTER
    }

    virtual float getTMin() const override
    {
        return 0.0f;
    }

private:
    Vector3f _center;
    Vector3f _direction;
    Vector3f _up;
    float _angle;
    Vector3f _horizontal;
};

#endif //CAMERA_H

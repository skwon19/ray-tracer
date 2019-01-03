#include "Light.h"
    void DirectionalLight::getIllumination(const Vector3f &p, 
                                 Vector3f &tolight, 
                                 Vector3f &intensity, 
                                 float &distToLight) const
    {
        // the direction to the light is the opposite of the
        // direction of the directional light source

        // BEGIN STARTER
        tolight =-_direction;
        intensity = _color;
        distToLight = std::numeric_limits<float>::max();
        // END STARTER
    }
    void PointLight::getIllumination(const Vector3f &p, 
                                 Vector3f &tolight, 
                                 Vector3f &intensity, 
                                 float &distToLight) const
    {
        // TODO Implement point light source
        // tolight, intensity, distToLight are outputs
        tolight = (_position - p).normalized();
        distToLight = (_position - p).abs();
        intensity = _color / (_falloff * (_position-p).absSquared());
    }
    void AreaLight::getIllumination(const Vector3f &p, 
                                 Vector3f &tolight, 
                                 Vector3f &intensity, 
                                 float &distToLight) const
    {
        // Pick a random point on the light, and give vector to light, intensity, and dist to light for that point
        Vector3f randomPoint = _p1 + ((float) rand() / RAND_MAX) * (_p3-_p1) + ((float) rand() / RAND_MAX) * (_p2-_p1);
        tolight = (randomPoint - p).normalized();
        distToLight = (randomPoint - p).abs();
        float cos = Vector3f::dot(_normal, -tolight); // cosine of angle between light's normal and path of light to p
        if (cos > 0) {
            intensity = _radiance * _area * cos / (2 * M_PI * distToLight * distToLight);
        } else { // p is behind or in same plane as light
            intensity = Vector3f(0);
        }
    }


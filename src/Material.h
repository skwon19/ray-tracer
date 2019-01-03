#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Image.h"
#include "Vector3f.h"

#include <string>

#include <math.h>

class Material
{
  public:
    Material(const Vector3f &diffuseColor, 
             const Vector3f &specularColor = Vector3f::ZERO, 
             float shininess = 0, 
             float reflectance = 0,
             float glossiness = 0,
             int transmittance = 0) :
        _diffuseColor(diffuseColor),
        _specularColor(specularColor),
        _shininess(shininess),
        _reflectance(reflectance),
        _glossiness(glossiness),
        _transmittance(transmittance)
    { }

    const Vector3f & getDiffuseColor() const {
        return _diffuseColor;
    }

    const Vector3f & getSpecularColor() const {
        return _specularColor;
    }

    const float getReflectance() const {
        return _reflectance;
    }

    const float getGlossiness() const {
        return _glossiness;
    }

    const float getTransmittance() const {
        return _transmittance;
    }

    Vector3f shade(const Ray &ray,
        const Hit &hit,
        const Vector3f &dirToLight,
        const Vector3f &lightIntensity);

protected:

    Vector3f _diffuseColor;
    Vector3f _specularColor;
    float   _shininess;
    float _reflectance;
    float _glossiness;
    int _transmittance;
};

#endif // MATERIAL_H

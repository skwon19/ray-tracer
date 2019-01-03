#ifndef LIGHT_H
#define LIGHT_H

#include <Vector3f.h>

#include "Object3D.h"

#include <limits>

class Light
{
  public:
    virtual ~Light() { }

    // in:  p           is the point to be shaded
    // out: tolight     is direction from p to light source
    // out: intensity   is the illumination intensity (RGB) at point p
    // out: distToLight is absolute distance from P to light (infinity for directional light)
    virtual void getIllumination(const Vector3f &p, 
                                 Vector3f &tolight, 
                                 Vector3f &intensity, 
                                 float &distToLight) const = 0;
};

class DirectionalLight : public Light
{
  public:
    DirectionalLight(const Vector3f &d, const Vector3f &c) :
        _direction(d.normalized()),
        _color(c)
    { }

    virtual void getIllumination(const Vector3f &p,
        Vector3f &tolight,
        Vector3f &intensity,
        float &distToLight) const override;

  private:
    Vector3f _direction;
    Vector3f _color;
};

class PointLight : public Light
{
  public:
    PointLight(const Vector3f &p, const Vector3f &c, float falloff) :
        _position(p),
        _color(c),
        _falloff(falloff)
    { }

    virtual void getIllumination(const Vector3f &p,
        Vector3f &tolight,
        Vector3f &intensity,
        float &distToLight) const override;

  private:
    Vector3f _position;
    Vector3f _color;
    float _falloff;
};

class AreaLight : public Light
{
  public:
    AreaLight(const Vector3f &p1, const Vector3f &p2, const Vector3f &p3, const Vector3f &color, const float flux)
        
    { 
      Vector3f p4 = p2 + p3 - p1;
      Vector3f n = (Vector3f::cross((p3-p1), (p2-p1))).normalized(); // normal to area light
      float area = (Vector3f::cross((p3-p1), (p2-p1))).abs();
      Vector3f radiance = flux * color / area;

      // Create mesh
      _mesh = new Group();
      Material * m = new Material(color);
      Triangle * triangle1 = new Triangle(p1, p3, p2, n, n, n, m);
      _mesh->addObject(triangle1);
      Triangle * triangle2 = new Triangle(p2, p3, p4, n, n, n, m);
      _mesh->addObject(triangle2);

      _radiance = radiance;
      _normal = n;
      _color = color;
      _p1 = p1;
      _p2 = p2;
      _p3 = p3;
      _area = area;
    }

    virtual void getIllumination(const Vector3f &p,
        Vector3f &tolight,
        Vector3f &intensity,
        float &distToLight) const override;

    Group * getMesh() {
      return _mesh;
    }

    Vector3f getColor() {
      return _color;
    }

    Vector3f getRadiance() {
      return _radiance;
    }

  private:
    Group * _mesh;
    Vector3f _radiance;
    Vector3f _normal;
    Vector3f _color;
    Vector3f _p1;
    Vector3f _p2;
    Vector3f _p3;
    float _area;
};

#endif // LIGHT_H

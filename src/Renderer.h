#ifndef RENDERER_H
#define RENDERER_H

#include <string>

#include "SceneParser.h"
#include "ArgParser.h"

class Hit;
class Vector3f;
class Ray;

class Renderer
{
  public:
    // Instantiates a renderer for the given scene.
    Renderer(const ArgParser &args);
    void Render();
  private:
  	Ray randomRay(Vector3f origin, Vector3f normal) const;
  	Vector3f lightPointContribution(Ray r, Hit &h, Vector3f dirToLight, Vector3f lightIntensity, float distToLight) const;
  	Vector3f directComponent(Ray r, Hit &h) const;
  	Vector3f indirectComponent(Ray r, Hit &h, int indirectBounces) const;
  	Vector3f specularComponent(Ray r, Hit h, int bounces, int indirectBounces) const;
  	Vector3f refractionComponent(Ray r, Hit &h, int bounces, int indirectBounces) const;
    Vector3f traceRay(const Ray &ray, float tmin, int bounces, 
                      Hit &hit, int indirectBounces) const;

    ArgParser _args;
    SceneParser _scene;
};

#endif // RENDERER_H

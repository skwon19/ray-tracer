#include "Material.h"

using namespace std;

Vector3f Material::shade(const Ray &ray,
    const Hit &hit,
    const Vector3f &dirToLight,
    const Vector3f &lightIntensity)
{
    // TODO implement Diffuse and Specular phong terms
    // return Vector3f(0, 0, 0);

    // Diffuse shading
    float diffuse = Vector3f::dot(dirToLight, hit.getNormal());
    float clampDiffuse = diffuse > 0 ? diffuse : 0;
    Vector3f Idiffuse = clampDiffuse * lightIntensity * _diffuseColor; // TODO: multiply by reflectance?

    // Specular shading
    Vector3f toEye = - ray.getDirection().normalized();
    Vector3f R = - toEye + 2 * Vector3f::dot(toEye, hit.getNormal()) * hit.getNormal();
    float specular = Vector3f::dot(dirToLight, R);
    float clampSpecular = specular > 0 ? specular : 0;
    Vector3f Ispecular = pow(clampSpecular, _shininess) * lightIntensity * _specularColor;

    return Idiffuse + Ispecular;
}

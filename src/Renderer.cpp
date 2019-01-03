#include "Renderer.h"

#include "ArgParser.h"
#include "Camera.h"
#include "Image.h"
#include "Ray.h"
#include "VecUtils.h"

#include <limits>

#include <math.h>


Renderer::Renderer(const ArgParser &args) :
    _args(args),
    _scene(args.input_file)
{
}

void
Renderer::Render()
{
    int w = _args.width;
    int h = _args.height;

    Image image(w, h);
    Image nimage(w, h);
    Image dimage(w, h);

    // loop through all the pixels in the image
    // generate all the samples

    // This look generates camera rays and callse traceRay.
    // It also write to the color, normal, and depth images.
    // You should understand what this code does.
    Camera* cam = _scene.getCamera();
    for (int y = 0; y < h; ++y) {
        float ndcy = 2 * (y / (h - 1.0f)) - 1.0f;
        for (int x = 0; x < w; ++x) {
            float ndcx = 2 * (x / (w - 1.0f)) - 1.0f;
            // Use PerspectiveCamera to generate a ray.
            // You should understand what generateRay() does.

            // Loop trace ray N times where N is subsampling rate
            Vector3f color = Vector3f(0,0,0);
            int subsamples = _args.subsamples;
            if (subsamples <= 0) {
                subsamples = 1;
            }
            for (int i=0; i<subsamples; i++) {
                Ray r = cam->generateRay(Vector2f(ndcx, ndcy), _args.jitter, w, h);

                Hit h;
                color += traceRay(r, cam->getTMin(), _args.bounces, h, _args.indirectBounces);
            }

            image.setPixel(x, y, color / subsamples);
            // nimage.setPixel(x, y, (h.getNormal() + 1.0f) / 2.0f);
            // float range = (_args.depth_max - _args.depth_min);
            // if (range) {
            //     dimage.setPixel(x, y, Vector3f((h.t - _args.depth_min) / range));
            // }
        }
    }
    // END SOLN

    // save the files 
    if (_args.output_file.size()) {
        image.savePNG(_args.output_file);
    }
    if (_args.depth_file.size()) {
        dimage.savePNG(_args.depth_file);
    }
    if (_args.normals_file.size()) {
        nimage.savePNG(_args.normals_file);
    }
}

// Helper function to generate a ray in a random direction, given an origin
Ray Renderer::randomRay(Vector3f origin, Vector3f normal) const {
    float theta = 2 * M_PI * ((float) rand() / RAND_MAX);
    float phi = acos(((float) rand() / RAND_MAX));

    // Find tangent
    Vector3f toCross = normal;
    if (normal.normalized() != Vector3f(1).normalized()) {
        toCross += Vector3f(1);
    } else {
        toCross += Vector3f(1,0,0);
    }
    Vector3f tangent = Vector3f::cross(normal, toCross);

    Matrix4f inclineRotation = Matrix4f::rotation(tangent, phi);
    Matrix4f inPlaneRotation = Matrix4f::rotation(normal, theta); // theta is "in plane" of surface

    Vector3f direction = (inPlaneRotation * inclineRotation * Vector4f(normal, 0)).xyz();
    return Ray(origin, direction);
}

// Calculate the contribution of a single light point to the direct component of illumination
Vector3f Renderer::lightPointContribution(Ray r, Hit &h, Vector3f dirToLight, Vector3f lightIntensity, float distToLight) const {
    Vector3f illuminationContribution = Vector3f(0);
    if (_args.shadows) { // support shadows
        Ray shadowRay = Ray(r.pointAtParameter(h.getT()), dirToLight);
        Hit shadowHit = Hit();
        if (_scene.getGroup()->intersect(shadowRay, 0.0001, shadowHit)) {
            Vector3f shadowIntersection = shadowRay.pointAtParameter(shadowHit.getT()) - r.pointAtParameter(h.getT());
            float shadowIntersectionDistance = shadowIntersection.abs();
            if (!(shadowIntersectionDistance < distToLight)) { // not in shadow
                illuminationContribution += h.getMaterial()->shade(r, h, dirToLight, lightIntensity);
            }
        } else { // not in shadow
            illuminationContribution += h.getMaterial()->shade(r, h, dirToLight, lightIntensity);
        }
    } else { // ignore shadows
        illuminationContribution += h.getMaterial()->shade(r, h, dirToLight, lightIntensity);
    }
    return illuminationContribution;
}

// Calculate direct lighting component of illumination
Vector3f Renderer::directComponent(Ray r, Hit &h) const {
    Vector3f illuminationComponent = Vector3f(0);
    for (int i=0; i<_scene.getNumLights(); i++) {
        Light* light = _scene.getLight(i);
        Vector3f dirToLight;
        Vector3f lightIntensity;
        float distToLight;
        light->getIllumination(r.pointAtParameter(h.getT()),
                             dirToLight, 
                             lightIntensity, 
                             distToLight);

        illuminationComponent += lightPointContribution(r, h, dirToLight, lightIntensity, distToLight);
    }
    return illuminationComponent;
}

// Use Monte Carlo path tracing to calculate indirect component of illumination
Vector3f Renderer::indirectComponent(Ray r, Hit &h, int indirectBounces) const {
    Hit newHit = Hit();
    Ray randomIndirectRay = randomRay(r.pointAtParameter(h.getT()), h.getNormal());

    // Russian roulette termination
    bool terminate = true;
    float continueProbability = 0.8;
    if (indirectBounces == _args.indirectBounces) { // always continue on first indirect bounce
        terminate = false;
    } else {
        if (((float) rand() / RAND_MAX) <= continueProbability) {
            terminate = false;
        }
    }

    if (!terminate) {
        Vector3f lightAtRandomHitPoint = traceRay(randomIndirectRay, 0.0001, 0, newHit, indirectBounces - 1);
        return h.getMaterial()->getDiffuseColor() * h.getMaterial()->getReflectance() * lightAtRandomHitPoint / continueProbability;
    } else {
        return Vector3f(0);
    }
}

// Calculate specular component of illumination
Vector3f Renderer::specularComponent(Ray r, Hit h, int bounces, int indirectBounces) const {
    if (bounces == 0) {
        return Vector3f(0);
    }
    else {
        Vector3f toEye = - r.getDirection().normalized();
        Vector3f R = - toEye + 2 * Vector3f::dot(toEye, h.getNormal()) * h.getNormal();
        Vector3f newOrigin = r.pointAtParameter(h.getT());
        if (h.getMaterial()->getGlossiness() == 0) {
            Ray perfectReflection = Ray(newOrigin, R);
            Hit newHit = Hit();
            return h.getMaterial()->getSpecularColor() * Renderer::traceRay(perfectReflection, 0.0001, bounces-1, newHit, indirectBounces);
        } else { // slightly jitter reflection ray to implement glossy surfaces
            float theta = 2 * M_PI * ((float) rand() / RAND_MAX);
            float phi = ((float) rand() / RAND_MAX) * h.getMaterial()->getGlossiness() / 2;

            // Find tangent
            Vector3f toCross = R;
            if (R.normalized() != Vector3f(1).normalized()) {
                toCross += Vector3f(1);
            } else {
                toCross += Vector3f(1,0,0);
            }
            Vector3f tangent = Vector3f::cross(R, toCross);

            Matrix4f inclineRotation = Matrix4f::rotation(tangent, phi);
            Matrix4f inPlaneRotation = Matrix4f::rotation(R, theta); // theta is "in plane" of surface

            Vector3f jitteredReflection = (inPlaneRotation * inclineRotation * Vector4f(R, 0)).xyz();

            Ray glossyReflection = Ray(newOrigin, jitteredReflection);
            Hit newHit;
            return h.getMaterial()->getSpecularColor() * Renderer::traceRay(glossyReflection, 0.0001, bounces-1, newHit, indirectBounces);
        }
        
    }
}

// Calculate refraction component of illumination
Vector3f Renderer::refractionComponent(Ray r, Hit &h, int bounces, int indirectBounces) const {
    if (bounces == 0) {
        return Vector3f(0);
    }

    const float n_glass = 1.5;
    const float n_air = 1.0;
    float n_r = n_air / n_glass;

    const Vector3f I = - r.getDirection().normalized();
    Vector3f N = h.getNormal().normalized();

    // Check whether your ray is inside going out or outside going in
    float insideCheck = Vector3f::dot(N, r.getDirection());
    if (insideCheck > 0) { // ray from inside going out
        n_r = n_glass / n_air;
        N = -N;
    }

    // Formula from lecture 11
    float radicand = 1 - pow(n_r, 2) * (1 - pow(Vector3f::dot(N, I), 2));
    if (radicand <=0 ) {
        return Vector3f(0);
    }
    Vector3f T = (n_r * Vector3f::dot(N, I) - pow(radicand, 0.5)) * N - n_r * I;
    Ray refractedRay = Ray(r.pointAtParameter(h.getT()), T.normalized());
    Hit newHit;
    return h.getMaterial()->getDiffuseColor() * traceRay(refractedRay, 0.0001, bounces-1, newHit, indirectBounces);
}


Vector3f
Renderer::traceRay(const Ray &r,
    float tmin,
    int bounces,
    Hit &h,
    int indirectBounces) const
{
    // The starter code only implements basic drawing of sphere primitives.
    // You will implement phong shading, recursive ray tracing, and shadow rays.

    // TODO: IMPLEMENT 
    // if (_scene.getGroup()->intersect(r, tmin, h)) {
    //     return h.getMaterial()->getDiffuseColor();
    // } else {
    //     return Vector3f(0, 0, 0);
    // };

    Hit areaLightHit;
    bool intersectAreaLight = false;
    AreaLight * closestAreaLight;
    for (int areaLightIndex=0; areaLightIndex<_scene.areaLights.size(); areaLightIndex++) {
        AreaLight * currentAreaLight = _scene.areaLights.at(areaLightIndex);
        if (currentAreaLight->getMesh()->intersect(r, tmin, areaLightHit)) {
            intersectAreaLight = true;
            closestAreaLight = currentAreaLight;
        }
    }

    if (_scene.getGroup()->intersect(r, tmin, h)) {
        if (intersectAreaLight) {
            if (areaLightHit.getT() < h.getT()) {
                return closestAreaLight->getColor();
            }
        }

        Vector3f illumination = Vector3f(0);
        // Ambient illumination
        illumination += _scene.getAmbientLight() * h.getMaterial()->getDiffuseColor();

        // Refraction, no diffuse/specular
        if (h.getMaterial()->getTransmittance() == 1) {
            illumination += refractionComponent(r, h, bounces, indirectBounces);
            return illumination;
        }


        // Direct illumination
        illumination += directComponent(r, h);
        // Indirect component: Monte Carlo ray tracing
        if (indirectBounces > 0) {
            illumination += indirectComponent(r, h, indirectBounces);
        }
        // Specular component
        illumination += specularComponent(r, h, bounces, indirectBounces);
        return illumination;
    } else {
        // return Vector3f(0, 0, 0);
        if (intersectAreaLight) {
            return closestAreaLight->getColor();
        } else {
            return _scene.getBackgroundColor(r.getDirection());
        }
    };
}


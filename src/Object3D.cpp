#include "Object3D.h"

bool Sphere::intersect(const Ray &r, float tmin, Hit &h) const
{
    // BEGIN STARTER

    // We provide sphere intersection code for you.
    // You should model other intersection implementations after this one.

    // Locate intersection point ( 2 pts )
    const Vector3f &rayOrigin = r.getOrigin(); //Ray origin in the world coordinate
    const Vector3f &dir = r.getDirection();

    Vector3f origin = rayOrigin - _center;      //Ray origin in the sphere coordinate

    float a = dir.absSquared();
    float b = 2 * Vector3f::dot(dir, origin);
    float c = origin.absSquared() - _radius * _radius;

    // no intersection
    if (b * b - 4 * a * c < 0) {
        return false;
    }

    float d = sqrt(b * b - 4 * a * c);

    float tplus = (-b + d) / (2.0f*a);
    float tminus = (-b - d) / (2.0f*a);

    // the two intersections are at the camera back
    if ((tplus < tmin) && (tminus < tmin)) {
        return false;
    }

    float t = 10000;
    // the two intersections are at the camera front
    if (tminus > tmin) {
        t = tminus;
    }

    // one intersection at the front. one at the back 
    if ((tplus > tmin) && (tminus < tmin)) {
        t = tplus;
    }

    if (t < h.getT()) {
        Vector3f normal = r.pointAtParameter(t) - _center;
        normal = normal.normalized();
        h.set(t, this->material, normal);
        return true;
    }
    // END STARTER
    return false;
}

// Add object to group
void Group::addObject(Object3D *obj) {
    m_members.push_back(obj);
}

// Return number of objects in group
int Group::getGroupSize() const {
    return (int)m_members.size();
}

bool Group::intersect(const Ray &r, float tmin, Hit &h) const
{
    // BEGIN STARTER
    // we implemented this for you
    bool hit = false;
    for (Object3D* o : m_members) {
        if (o->intersect(r, tmin, h)) {
            hit = true;
        }
    }
    return hit;
    // END STARTER
}


Plane::Plane(const Vector3f &normal, float d, Material *m) : Object3D(m) {
    // TODO implement Plane constructor
    _normal = normal;
    _d = d;
}
bool Plane::intersect(const Ray &r, float tmin, Hit &h) const
{
    // TODO implement
    // return false;

    const Vector3f &Ro = r.getOrigin();
    const Vector3f &Rd = r.getDirection();

    if (Vector3f::dot(Rd, _normal) == 0) { // ray parallel to plane
        return false;
    }

    float t = (_d - Vector3f::dot(Ro, _normal)) / Vector3f::dot(Rd, _normal);

    if (t < tmin) { // intersect behind eye
        return false;
    }
    if (t < h.getT()) {
        Vector3f hitNormal = _normal;
        hitNormal = hitNormal.normalized();
        h.set(t, this->material, hitNormal);
        return true;
    }
    return false;
}
bool Triangle::intersect(const Ray &r, float tmin, Hit &h) const 
{
    // TODO implement
    // return false;

    const Vector3f &Ro = r.getOrigin();
    const Vector3f &Rd = r.getDirection();

    // AX = B
    Matrix3f A = Matrix3f(_v[0] - _v[1], _v[0] - _v[2], Rd, true);
    Vector3f B = _v[0] - Ro;
    Vector3f X = A.inverse() * B;
    float beta = X[0];
    float gamma = X[1];
    float t = X[2];
    float alpha = 1 - beta - gamma;

    if ((alpha < 0) || (beta < 0) || (gamma < 0)) { // intersection outside triangle
        return false;
    }
    if (t < tmin) {
        return false;
    }
    if (t < h.getT()) {
        Vector3f hitNormal = alpha * _normals[0] + beta * _normals[1] + gamma * _normals[2];
        hitNormal = hitNormal.normalized();
        h.set(t, this->material, hitNormal);
        return true;
    }
    return false;
}


Transform::Transform(const Matrix4f &m,
    Object3D *obj) : _object(obj) {
    // TODO implement Transform constructor
    _object = obj;
    _m = m;
}
bool Transform::intersect(const Ray &r, float tmin, Hit &h) const
{
    // TODO implement
    // return false;

    // Move ray from world space to object space
    Vector4f newOrigin = _m.inverse() * Vector4f(r.getOrigin(), 1);
    Vector4f newDir = _m.inverse() * Vector4f(r.getDirection(), 0);
    Ray rayObject = Ray(newOrigin.xyz(), newDir.xyz());

    // Determine whether there's an intersection
    // Hit hitObject = Hit(); // new hit, in object coordinates
    bool intersects = _object->intersect(rayObject, tmin, h);
    if (intersects == false) {
        return false;
    }

    // Transform the hit normal back to world coordinates
    Vector4f normalObject = Vector4f(h.getNormal(), 0);
    Vector3f normalWorld = (_m.inverse().transposed() * normalObject).xyz();
    normalWorld.normalize();
    h.set(h.getT(), _object->material, normalWorld);
    return true;
}
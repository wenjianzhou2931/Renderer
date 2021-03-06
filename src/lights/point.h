#ifndef LIGHT_POINT_H
#define LIGHT_POINT_H

#include "../core/light.h"

class PointLight : public Light {
public:
    PointLight(const Spectrum &I, const Point3f pLight) : I(I), pLight(pLight), Light(1) {}
    Spectrum Sample_Li(const HitRecord &ref, const Point2f &u, Vector3f *wi, float *pdf, VisibilityTester *vis) const;
    float Pdf_Li(const HitRecord &ref, const Vector3f &wi) const;
    Spectrum L(const HitRecord &intr, const Vector3f &w) const {}
private:
    const Point3f pLight;
    const Spectrum I;
};

#endif
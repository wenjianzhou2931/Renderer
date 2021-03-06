#ifndef MICROFACET_H
#define MICROFACET_H

#include "vector.h"
#include "bsdf.h"

class MicrofacetDistribution {
public:
    virtual ~MicrofacetDistribution() {}
    virtual float D(const Vector3f &wh) const = 0;
    virtual float Lambda(const Vector3f &w) const = 0;
    float G1(const Vector3f &w) const {
        return 1 / (1 + Lambda(w));
    }
    float G(const Vector3f &wo, const Vector3f &wi) const {
        return 1 / (1 + Lambda(wo) + Lambda(wi));
    }
    virtual Vector3f Sample_wh(const Vector3f &wo, const Point2f &u) const = 0;
    float Pdf(const Vector3f &wo, const Vector3f &wh) const;
protected:
    MicrofacetDistribution(bool sampleVisibleArea)
        : sampleVisibleArea(sampleVisibleArea) {}

    const bool sampleVisibleArea;
};

class TrowbridgeReitzDistribution : public MicrofacetDistribution {
public:
    static inline float RoughnessToAlpha(float roughness);
    TrowbridgeReitzDistribution(float alphax, float alphay, bool samplevis = true)
        : MicrofacetDistribution(samplevis), alphax(alphax), alphay(alphay) {}
    float D(const Vector3f &wh) const;
    Vector3f Sample_wh(const Vector3f &wo, const Point2f &u) const;
private:
    float Lambda(const Vector3f &w) const;
    const float alphax, alphay;
};

inline float TrowbridgeReitzDistribution::RoughnessToAlpha(float roughness) {
    roughness = std::max(roughness, (float)1e-3);
    float x = std::log(roughness);
    return 1.62142f + 0.819955f * x + 0.1734f * x * x + 0.0171201f * x * x * x +
           0.000640711f * x * x * x * x;
}

#endif
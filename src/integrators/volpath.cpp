#include "volpath.h"

Spectrum VolPathIntegrator::Li(const Ray &r, const Scene &scene, Sampler &sampler) const {
    Spectrum L(0.f), beta(1.f);
    Ray ray(r);
    bool specularBounce = false;
    for (int bounces = 0; ; ++bounces) {
        HitRecord isect;
        bool foundIntersection = scene.Intersect(ray, isect);

        HitRecord mi;
        if (ray.medium) {
            beta *= ray.medium->Sample(ray, sampler, mi);
        }
        if (beta.IsBlack()) 
            break;

        if (mi.mediumRecord.IsValid()) {
            L += beta * UniformSampleOneLight(ray, mi, scene, sampler, true);
            Vector3f wo = -ray.d, wi;
            mi.mediumRecord.phase->Sample_p(wo, &wi, sampler.Next2D());
            ray = Ray(mi.p, wi, INF, 0.f, ray.medium);
            specularBounce = false;
        }
        else {
            if (bounces == 0 || specularBounce) {
                if (foundIntersection && !isect.mat_ptr) 
                    L += beta * 8.0f * Spectrum(0.747f+0.058f, 0.747f+0.258f, 0.747f) + 15.6f * Spectrum(0.740f+0.287f,0.740f+0.160f,0.740f) + 18.4f * Spectrum(0.737f+0.642f,0.737f+0.159f,0.737f);
                else
                    for (const auto &light : scene.lights) {
                        L += beta * light->Le(ray);
                    }
            }
            if (!foundIntersection || bounces >= maxDepth)
                break;

            if (!isect.mat_ptr) {
                ray = Ray(isect.p + ray.d * 0.0001, ray.d, INF, 0.f, ray.medium);
                bounces --;
                continue;
            }
            isect.mat_ptr->ComputeScatteringFunctions(&isect, TransportMode::Radiance);

            L += beta * UniformSampleOneLight(ray, isect, scene, sampler, true);

            Vector3f wo = -ray.d, wi;
            float pdf;
            BxDFType flags;
            Spectrum f = isect.bsdf->Sample_f(wo, &wi, sampler.Next2D(), &pdf, BSDF_ALL, &flags);

            if (f.IsBlack() || pdf == 0.f)
                break;
            beta *= f * AbsDot(wi, isect.normal) / pdf;
            specularBounce = (flags & BSDF_SPECULAR) != 0;
            ray = Ray(isect.p, wi, INF, 0.0f, isect.GetMedium(wi));
        }

        if (bounces > 3) {
            float q = std::max((float).05, 1 - beta.y());
            if (sampler.Next1D() < q)
                break;
            beta /= 1 - q;
        }
    }
    //std::cout << L << std::endl;
    return L;
}
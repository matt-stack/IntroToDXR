#pragma once


//#include "PBR_include/vecmath.h"

#include <map>
#include <string>
#include <optional>
#include <PBR_include/pbrt.h>
#include <vector>
#include <PBR_include/transform.h>
#include <PBR_include/paramdict.h>
#include <PBR_include/scene.h>

namespace PBR {

    // Shape Declarations
    class Triangle;
    class BilinearPatch;
    class Curve;
    class Sphere;
    class Cylinder;
    class Disk;

    struct ShapeSample;
    struct ShapeIntersection;
    struct ShapeSampleContext;

    // Shape Definition
    class Shape{
    public:
        // Shape Interface
        //using TaggedPointer::TaggedPointer;

        static std::vector<Shape> Create(
            const std::string& name, const Transform* renderFromObject,
            const Transform* objectFromRender, bool reverseOrientation,
            const ParameterDictionary& parameters,
            const std::map<std::string, FloatTexture>& floatTextures, const FileLoc* loc,
            Allocator alloc);
        std::string ToString() const;

        //inline Bounds3f Bounds() const;

         //inline DirectionCone NormalBounds() const;

        // inline std::optional<ShapeIntersection> Intersect(
        //    const Ray& ray, Float tMax = Infinity) const;

         //inline bool IntersectP(const Ray& ray, Float tMax = Infinity) const;

         inline float Area() const;

         inline std::optional<ShapeSample> Sample(Point2f u) const;

         //inline float PDF(const Interaction&) const;

         inline std::optional<ShapeSample> Sample(const ShapeSampleContext& ctx,
            Point2f u) const;

         inline float PDF(const ShapeSampleContext& ctx, Vector3f wi) const;
    };


    // BilinearPatch Definition
    class BilinearPatch {
    public:
        // BilinearPatch Public Methods
        BilinearPatch(const BilinearPatchMesh* mesh, int meshIndex, int blpIndex);

        static void Init(Allocator alloc);

        static BilinearPatchMesh* CreateMesh(const Transform* renderFromObject,
            bool reverseOrientation,
            const ParameterDictionary& parameters,
            const FileLoc* loc, Allocator alloc);

        static std::vector<Shape> CreatePatches(const BilinearPatchMesh* mesh,
            Allocator alloc);

            //Bounds3f Bounds() const;

        
            //std::optional<ShapeIntersection> Intersect(const Ray& ray,
            //    float tMax = Infinity) const;

        
            //bool IntersectP(const Ray& ray, float tMax = Infinity) const;

        
            std::optional<ShapeSample> Sample(const ShapeSampleContext& ctx, Point2f u) const;

        
            float PDF(const ShapeSampleContext& ctx, Vector3f wi) const;

        
            std::optional<ShapeSample> Sample(Point2f u) const;

        
            //float PDF(const Interaction&) const;

        
            //DirectionCone NormalBounds() const;

        std::string ToString() const;

        
            float Area() const { return area; }

        
            static SurfaceInteraction InteractionFromIntersection(const BilinearPatchMesh* mesh,
                int blpIndex, Point2f uv,
                Float time, Vector3f wo) {
            // Compute bilinear patch point $\pt{}$, $\dpdu$, and $\dpdv$ for $(u,v)$
            // Get bilinear patch vertices in _p00_, _p01_, _p10_, and _p11_
            const int* v = &mesh->vertexIndices[4 * blpIndex];
            Point3f p00 = mesh->p[v[0]], p10 = mesh->p[v[1]];
            Point3f p01 = mesh->p[v[2]], p11 = mesh->p[v[3]];

            Point3f p = Lerp(uv[0], Lerp(uv[1], p00, p01), Lerp(uv[1], p10, p11));
            Vector3f dpdu = Lerp(uv[1], p10, p11) - Lerp(uv[1], p00, p01);
            Vector3f dpdv = Lerp(uv[0], p01, p11) - Lerp(uv[0], p00, p10);

            // Compute $(s,t)$ texture coordinates at bilinear patch $(u,v)$
            Point2f st = uv;
            float duds = 1, dudt = 0, dvds = 0, dvdt = 1;
            if (mesh->uv) {
                // Compute texture coordinates for bilinear patch intersection point
                Point2f uv00 = mesh->uv[v[0]], uv10 = mesh->uv[v[1]];
                Point2f uv01 = mesh->uv[v[2]], uv11 = mesh->uv[v[3]];
                st = Lerp(uv[0], Lerp(uv[1], uv00, uv01), Lerp(uv[1], uv10, uv11));

                // Update bilinear patch $\dpdu$ and $\dpdv$ accounting for $(s,t)$
                // Compute partial derivatives of $(u,v)$ with respect to $(s,t)$
                Vector2f dstdu = Lerp(uv[1], uv10, uv11) - Lerp(uv[1], uv00, uv01);
                Vector2f dstdv = Lerp(uv[0], uv01, uv11) - Lerp(uv[0], uv00, uv10);
                duds = std::abs(dstdu[0]) < 1e-8f ? 0 : 1 / dstdu[0];
                dvds = std::abs(dstdv[0]) < 1e-8f ? 0 : 1 / dstdv[0];
                dudt = std::abs(dstdu[1]) < 1e-8f ? 0 : 1 / dstdu[1];
                dvdt = std::abs(dstdv[1]) < 1e-8f ? 0 : 1 / dstdv[1];

                // Compute partial derivatives of $\pt{}$ with respect to $(s,t)$
                Vector3f dpds = dpdu * duds + dpdv * dvds;
                Vector3f dpdt = dpdu * dudt + dpdv * dvdt;

                // Set _dpdu_ and _dpdv_ to updated partial derivatives
                if (Cross(dpds, dpdt) != Vector3f(0, 0, 0)) {
                    if (Dot(Cross(dpdu, dpdv), Cross(dpds, dpdt)) < 0)
                        dpdt = -dpdt;
                    DCHECK_GE(Dot(Normalize(Cross(dpdu, dpdv)), Normalize(Cross(dpds, dpdt))),
                        -1e-3);
                    dpdu = dpds;
                    dpdv = dpdt;
                }
            }

            // Find partial derivatives $\dndu$ and $\dndv$ for bilinear patch
            Vector3f d2Pduu(0, 0, 0), d2Pdvv(0, 0, 0);
            Vector3f d2Pduv = (p00 - p01) + (p11 - p10);
            // Compute coefficients for fundamental forms
            float E = Dot(dpdu, dpdu), F = Dot(dpdu, dpdv), G = Dot(dpdv, dpdv);
            Vector3f n = Normalize(Cross(dpdu, dpdv));
            float e = Dot(n, d2Pduu), f = Dot(n, d2Pduv), g = Dot(n, d2Pdvv);

            // Compute $\dndu$ and $\dndv$ from fundamental form coefficients
            float EGF2 = DifferenceOfProducts(E, G, F, F);
            float invEGF2 = (EGF2 == 0) ? Float(0) : 1 / EGF2;
            Normal3f dndu =
                Normal3f((f * F - e * G) * invEGF2 * dpdu + (e * F - f * E) * invEGF2 * dpdv);
            Normal3f dndv =
                Normal3f((g * F - f * G) * invEGF2 * dpdu + (f * F - g * E) * invEGF2 * dpdv);

            // Update $\dndu$ and $\dndv$ to account for $(s,t)$ parameterization
            Normal3f dnds = dndu * duds + dndv * dvds;
            Normal3f dndt = dndu * dudt + dndv * dvdt;
            dndu = dnds;
            dndv = dndt;

            // Initialize bilinear patch intersection point error _pError_
            Point3f pAbsSum = Abs(p00) + Abs(p01) + Abs(p10) + Abs(p11);
            Vector3f pError = gamma(6) * Vector3f(pAbsSum);

            // Initialize _SurfaceInteraction_ for bilinear patch intersection
            int faceIndex = mesh->faceIndices ? mesh->faceIndices[blpIndex] : 0;
            bool flipNormal = mesh->reverseOrientation ^ mesh->transformSwapsHandedness;
            SurfaceInteraction isect(Point3fi(p, pError), st, wo, dpdu, dpdv, dndu, dndv,
                time, flipNormal, faceIndex);

            // Compute bilinear patch shading normal if necessary
            if (mesh->n) {
                // Compute shading normals for bilinear patch intersection point
                Normal3f n00 = mesh->n[v[0]], n10 = mesh->n[v[1]];
                Normal3f n01 = mesh->n[v[2]], n11 = mesh->n[v[3]];
                Normal3f ns = Lerp(uv[0], Lerp(uv[1], n00, n01), Lerp(uv[1], n10, n11));
                if (LengthSquared(ns) > 0) {
                    ns = Normalize(ns);
                    // Set shading geometry for bilinear patch intersection
                    Normal3f dndu = Lerp(uv[1], n10, n11) - Lerp(uv[1], n00, n01);
                    Normal3f dndv = Lerp(uv[0], n01, n11) - Lerp(uv[0], n00, n10);
                    // Update $\dndu$ and $\dndv$ to account for $(s,t)$ parameterization
                    Normal3f dnds = dndu * duds + dndv * dvds;
                    Normal3f dndt = dndu * dudt + dndv * dvdt;
                    dndu = dnds;
                    dndv = dndt;

                    Transform r = RotateFromTo(Vector3f(Normalize(isect.n)), Vector3f(ns));
                    isect.SetShadingGeometry(ns, r(dpdu), r(dpdv), dndu, dndv, true);
                }
            }

            return isect;
        }

    private:
        // BilinearPatch Private Methods
        PBRT_CPU_GPU
            const BilinearPatchMesh* GetMesh() const {
#ifdef PBRT_IS_GPU_CODE
            return (*allBilinearMeshesGPU)[meshIndex];
#else
            return (*allMeshes)[meshIndex];
#endif
        }

        PBRT_CPU_GPU
            bool IsRectangle(const BilinearPatchMesh* mesh) const {
            // Get bilinear patch vertices in _p00_, _p01_, _p10_, and _p11_
            const int* v = &mesh->vertexIndices[4 * blpIndex];
            Point3f p00 = mesh->p[v[0]], p10 = mesh->p[v[1]];
            Point3f p01 = mesh->p[v[2]], p11 = mesh->p[v[3]];

            if (p00 == p01 || p01 == p11 || p11 == p10 || p10 == p00)
                return false;
            // Check if bilinear patch vertices are coplanar
            Normal3f n(Normalize(Cross(p10 - p00, p01 - p00)));
            if (AbsDot(Normalize(p11 - p00), n) > 1e-5f)
                return false;

            // Check if planar vertices form a rectangle
            Point3f pCenter = (p00 + p01 + p10 + p11) / 4;
            Float d2[4] = { DistanceSquared(p00, pCenter), DistanceSquared(p01, pCenter),
                           DistanceSquared(p10, pCenter), DistanceSquared(p11, pCenter) };
            for (int i = 1; i < 4; ++i)
                if (std::abs(d2[i] - d2[0]) / d2[0] > 1e-4f)
                    return false;
            return true;
        }

        // BilinearPatch Private Members
        int meshIndex, blpIndex;
        static pstd::vector<const BilinearPatchMesh*>* allMeshes;
        Float area;
        static constexpr Float MinSphericalSampleArea = 1e-4;
    };


}  // namespace pbrt



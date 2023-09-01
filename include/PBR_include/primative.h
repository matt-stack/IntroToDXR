#pragma once
#include <optional>
#include <PBR_include/transform.h>
//#include <PBR_include/scene.h>

namespace PBR {

    class SimplePrimitive;
    class GeometricPrimitive;
    class TransformedPrimitive;
// fix Primitive, then add Shapes, 
    // Primitive Definition
    class Primitive
//        : public TaggedPointer<SimplePrimitive, GeometricPrimitive, TransformedPrimitive,
//        AnimatedPrimitive, BVHAggregate, KdTreeAggregate> 
    {
    public:
        // Primitive Interface
        //using TaggedPointer::TaggedPointer;

        //Bounds3f Bounds() const;

        //std::optional<ShapeIntersection> Intersect(const Ray& r,
        //    Float tMax = Infinity) const;
        //bool IntersectP(const Ray& r, Float tMax = Infinity) const;
    };

    // GeometricPrimitive Definition
    class GeometricPrimitive : public Primitive{
    public:
        // GeometricPrimitive Public Methods
        GeometricPrimitive(Shape shape, Material material, Light areaLight,
            FloatTexture alpha);
            //FloatTexture alpha = nullptr);
        //Bounds3f Bounds() const;
        //pstd::optional<ShapeIntersection> Intersect(const Ray& r, Float tMax) const;
        //bool IntersectP(const Ray& r, Float tMax) const;

    private:
        // GeometricPrimitive Private Members
        Shape shape;
        Material material;
        Light areaLight;
        FloatTexture alpha;
    };

    // SimplePrimitive Definition
    class SimplePrimitive : public Primitive {
    public:
        // SimplePrimitive Public Methods
        //Bounds3f Bounds() const;
        //pstd::optional<ShapeIntersection> Intersect(const Ray& r, Float tMax) const;
        //bool IntersectP(const Ray& r, Float tMax) const;
        SimplePrimitive(Shape shape, Material material);

    private:
        // SimplePrimitive Private Members
        Shape shape;
        Material material;
    };

    // TransformedPrimitive Definition
    class TransformedPrimitive : public Primitive {
    public:
        // TransformedPrimitive Public Methods
        TransformedPrimitive(Primitive primitive, const Transform* renderFromPrimitive)
            : primitive(primitive), renderFromPrimitive(renderFromPrimitive) {
            //primitiveMemory += sizeof(*this);
        }

        //std::optional<ShapeIntersection> Intersect(const Ray& r, Float tMax) const;
        //bool IntersectP(const Ray& r, Float tMax) const;

        //Bounds3f Bounds() const { return (*renderFromPrimitive)(primitive.Bounds()); }

    private:
        // TransformedPrimitive Private Members
        Primitive primitive;
        const Transform* renderFromPrimitive;
    };


}  // namespace pbrt



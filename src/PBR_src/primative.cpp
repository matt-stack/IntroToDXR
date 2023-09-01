#include "PBR_include/primative.h"

namespace PBR {

    // SimplePrimitive Method Definitions
    SimplePrimitive::SimplePrimitive(Shape shape, Material material)
        : shape(shape), material(material) {
        //primitiveMemory += sizeof(*this); // this is for the allocator, but Im keeping it around
    }

    // GeometricPrimitive Method Definitions
    GeometricPrimitive::GeometricPrimitive(Shape shape, Material material, Light areaLight,
        const MediumInterface& mediumInterface,
        FloatTexture alpha)
        : shape(shape),
        material(material),
        areaLight(areaLight),
        mediumInterface(mediumInterface),
        alpha(alpha) {
        //primitiveMemory += sizeof(*this);
    }

    
}

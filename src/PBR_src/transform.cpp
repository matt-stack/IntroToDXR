#include "PBR_include/transform.h"

namespace PBR {

    Transform Transform::operator*(const Transform& t2) const {
        return Transform(m * t2.m, t2.mInv * mInv);
    }

    Transform Scale(float x, float y, float z) {
        SquareMatrix<4> m(x, 0, 0, 0,
            0, y, 0, 0,
            0, 0, z, 0,
            0, 0, 0, 1);
        SquareMatrix<4> minv(1 / x, 0, 0, 0,
            0, 1 / y, 0, 0,
            0, 0, 1 / z, 0,
            0, 0, 0, 1);
        return Transform(m, minv);
    }

}

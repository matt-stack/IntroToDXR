#pragma once

#include <string>
#include "PBR_include/math.h"
#include <optional>
#include <limits>

namespace PBR {

    // Transform Definition
    class Transform {
    public:
        // Transform Public Methods
            //inline Ray ApplyInverse(const Ray& r, Float* tMax = nullptr) const;
       //     inline RayDifferential ApplyInverse(const RayDifferential& r,
       //         Float* tMax = nullptr) const;
       // template <typename T>
       // inline Vector3<T> ApplyInverse(Vector3<T> v) const;
       // template <typename T>
       // inline Normal3<T> ApplyInverse(Normal3<T>) const;

       // std::string ToString() const;

        Transform() = default;

        // Keep working on this and make sure that scene.cpp line 100 works, then do TransformSet

            Transform(const SquareMatrix<4>& m) : m(m) {
            std::optional<SquareMatrix<4>> inv = Inverse<4>(m);
            if (inv)
                mInv = *inv;
            else {
                // Initialize _mInv_ with not-a-number values
                float NaN = std::numeric_limits<float>::has_signaling_NaN
                    ? std::numeric_limits<float>::signaling_NaN()
                    : std::numeric_limits<float>::quiet_NaN();
                for (int i = 0; i < 4; ++i)
                    for (int j = 0; j < 4; ++j)
                        mInv[i][j] = NaN;
            }
        }

        Transform(const SquareMatrix<4>& m, const SquareMatrix<4>& mInv) : m(m), mInv(mInv) {}

        const SquareMatrix<4>& GetMatrix() const { return m; }
        const SquareMatrix<4>& GetInverseMatrix() const { return mInv; }



    private:
        // Transform Private Members
        SquareMatrix<4> m, mInv;
    };

    // Transform Inline Functions
    inline Transform Inverse(const Transform& t) {
        return Transform(t.GetInverseMatrix(), t.GetMatrix());
    }

    inline Transform Transpose(const Transform& t) {
        return Transform(Transpose(t.GetMatrix()), Transpose(t.GetInverseMatrix()));
    }



} // PBR


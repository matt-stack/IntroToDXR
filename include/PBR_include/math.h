#pragma once

#include <vector>
#include "PBR_include/span.h"
#include <vector>
#include <string>
#include <optional>
#include <cmath>
#include <tuple>

namespace PBR {

/*

    // basic way to use this SquareMatrix
    // this should also work inline with current PBR, 
    // as it still uses span for most access

    SquareMatrix<4> mr(1, 0, 0, 3,
        0, 1, 0, 3,
        0, 0, 1, 3,
        0, 0, 0, 1);
    SquareMatrix<4> minv(1, 0, 0, 2,
        0, 1, 0, 2,
        0, 0, 1, 2,
        0, 0, 0, 1);

    minv.print();

    std::printf("\n\n");

    auto new_mat = Transpose(minv);

    new_mat.print();
*/
    constexpr float Pi = 3.14159265358979323846;


    inline float Radians(float deg) {
        return (Pi / 180) * deg;
    }
    inline float Degrees(float rad) {
        return (180 / Pi) * rad;
    }

    using Point2f = std::tuple<float, float>;
    using Vector2f = std::tuple<float, float>;
    using Point3f = std::tuple<float, float, float>;
    using Vector3f = std::tuple<float, float, float>;
    using Normal3f = std::tuple<float, float, float>;

    template <typename T>
    inline constexpr T Sqr(T v) {
        return v * v;
    }


    // math for vector3f
    //template <typename T>
    inline float LengthSquared(Vector3f v) {
        return Sqr(std::get<0>(v)) + Sqr(std::get<1>(v)) + Sqr(std::get<2>(v));
    }

    //template <typename T>
    inline float Length(Vector3f v) {
        using std::sqrt;
        return sqrt(LengthSquared(v));
    }

   // template <typename T>
    inline Vector3f Normalize(Vector3f v) {
        return { std::get<0>(v) / Length(v), std::get<1>(v) / Length(v), std::get<2>(v) / Length(v)};
    }

    template <int N>
    inline void init(float m[N][N], int i, int j) {}

    template <int N, typename... Args>
    inline void init(float m[N][N], int i, int j, float v, Args... args) {
        m[i][j] = v;
        if (++j == N) {
            ++i;
            j = 0;
        }
        init<N>(m, i, j, args...);
    }

    // SquareMatrix Definition
    template <int N>
    class SquareMatrix {
    public:
	void print(){
        for (int i = 0; i < N; ++i){
            for (int j = 0; j < N; ++j){
		std::printf("%f\t", this->m[i][j]);
	    }
	    std::printf("\n");
	}
	}
	
        // SquareMatrix Public Methods
        static SquareMatrix Zero() {
            SquareMatrix m;
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    m.m[i][j] = 0;
            return m;
        }

        SquareMatrix() {
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    m[i][j] = (i == j) ? 1 : 0;
        }

        SquareMatrix(const float mat[N][N]) {
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    m[i][j] = mat[i][j];
        }

        SquareMatrix(span<const float> t);


        template <typename... Args>
        SquareMatrix(float v, Args... args) {
            static_assert(1 + sizeof...(Args) == N * N,
                "Incorrect number of values provided to SquareMatrix constructor");
            init<N>(m, 0, 0, v, args...);
        }
/*
        template <typename... Args>
        static SquareMatrix Diag(float v, Args... args) {
            static_assert(1 + sizeof...(Args) == N,
                "Incorrect number of values provided to SquareMatrix::Diag");
            SquareMatrix m;
            initDiag<N>(m.m, 0, v, args...);
            return m;
        }
*/
        SquareMatrix operator+(const SquareMatrix& m) const {
            SquareMatrix r = *this;
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    r.m[i][j] += m.m[i][j];
            return r;
        }

        SquareMatrix operator*(float s) const {
            SquareMatrix r = *this;
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    r.m[i][j] *= s;
            return r;
        }
        SquareMatrix operator/(float s) const {
            //DCHECK_NE(s, 0);
            SquareMatrix r = *this;
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    r.m[i][j] /= s;
            return r;
        }

        bool operator==(const SquareMatrix<N>& m2) const {
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    if (m[i][j] != m2.m[i][j])
                        return false;
            return true;
        }

        bool operator!=(const SquareMatrix<N>& m2) const {
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    if (m[i][j] != m2.m[i][j])
                        return true;
            return false;
        }

        bool operator<(const SquareMatrix<N>& m2) const {
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j) {
                    if (m[i][j] < m2.m[i][j])
                        return true;
                    if (m[i][j] > m2.m[i][j])
                        return false;
                }
            return false;
        }

        bool IsIdentity() const;

        std::string ToString() const;

        span<const float> operator[](int i) const { return m[i]; }
        span<float> operator[](int i) { return span<float>(m[i]); }

    private:
        float m[N][N];
    };

    template <int N>
    inline SquareMatrix<N>::SquareMatrix(PBR::span<const float> t) {
        //CHECK_EQ(N * N, t.size());
        for (int i = 0; i < N * N; ++i)
            m[i / N][i % N] = t[i];
    }


    // SquareMatrix Inline Methods
    template <int N>
    inline bool SquareMatrix<N>::IsIdentity() const {
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) {
                if (i == j) {
                    if (m[i][j] != 1)
                        return false;
                }
                else if (m[i][j] != 0)
                    return false;
            }
        return true;
    }

    // SquareMatrix Inline Functions
    template <int N>
    inline SquareMatrix<N> operator*(float s, const SquareMatrix<N>& m) {
        return m * s;
    }

    template <int N>
    inline SquareMatrix<N> operator*(const SquareMatrix<N>& m1,
        const SquareMatrix<N>& m2) {
        SquareMatrix<N> r;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j) {
                r[i][j] = 0;
                for (int k = 0; k < N; ++k)
                    r[i][j] = FMA(m1[i][k], m2[k][j], r[i][j]);
            }
        return r;
    }


    template <int N>
    inline SquareMatrix<N> Transpose(const SquareMatrix<N>& m) {
        SquareMatrix<N> r;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                r[i][j] = m[j][i];
        return r;
    }


    // CompensatedFloat Definition
    struct CompensatedFloat {
    public:
        // CompensatedFloat Public Methods
        CompensatedFloat(float v, float err = 0) : v(v), err(err) {}

        explicit operator float() const { return v + err; }

        explicit operator double() const { return double(v) + double(err); }

        //std::string ToString() const;

        float v, err;
    };

    // in <cmath>
    inline float FMA(float a, float b, float c) {
        return std::fma(a, b, c);
    }

    inline CompensatedFloat TwoProd(float a, float b) {
        float ab = a * b;
        return { ab, FMA(a, b, -ab) };
    }

    inline CompensatedFloat TwoSum(float a, float b) {
        float s = a + b, delta = s - a;
        return { s, (a - (s - delta)) + (b - delta) };
    }



    namespace internal {
        // InnerProduct Helper Functions
        template <typename Float>
        inline CompensatedFloat InnerProduct(Float a, Float b) {
            return TwoProd(a, b);
        }

        // Accurate dot products with FMA: Graillat et al.,
        // https://www-pequan.lip6.fr/~graillat/papers/posterRNC7.pdf
        //
        // Accurate summation, dot product and polynomial evaluation in complex
        // floating point arithmetic, Graillat and Menissier-Morain.
        template <typename Float, typename... T>
        inline CompensatedFloat InnerProduct(Float a, Float b, T... terms) {
            CompensatedFloat ab = TwoProd(a, b);
            CompensatedFloat tp = InnerProduct(terms...);
            CompensatedFloat sum = TwoSum(ab.v, tp.v);
            return { sum.v, ab.err + (tp.err + sum.err) };
        }

    }  // namespace internal

    template <typename... T>
    inline std::enable_if_t<std::conjunction_v<std::is_arithmetic<T>...>, float>
        InnerProduct(T... terms) {
        CompensatedFloat ip = internal::InnerProduct(terms...);
        return float(ip);
    }


    template <typename Ta, typename Tb, typename Tc, typename Td>
    inline auto DifferenceOfProducts(Ta a, Tb b, Tc c, Td d) {
        auto cd = c * d;
        auto differenceOfProducts = FMA(a, b, -cd);
        auto error = FMA(-c, d, cd);
        return differenceOfProducts + error;
    }


    // Inverse is used for the Transform transform(SquareMatric) function
    //template <>
    //inline std::optional<SquareMatrix<4>> Inverse(const SquareMatrix<4>& m) {
    // the original only has template<>, but I am required to do template<int>, not quite sure
    // why
    template <int>
    inline std::optional<SquareMatrix<4>> Inverse(const SquareMatrix<4>& m) {
        // Via: https://github.com/google/ion/blob/master/ion/math/matrixutils.cc,
        // (c) Google, Apache license.

        // For 4x4 do not compute the adjugate as the transpose of the cofactor
        // matrix, because this results in extra work. Several calculations can be
        // shared across the sub-determinants.
        //
        // This approach is explained in David Eberly's Geometric Tools book,
        // excerpted here:
        //   http://www.geometrictools.com/Documentation/LaplaceExpansionTheorem.pdf
        float s0 = DifferenceOfProducts(m[0][0], m[1][1], m[1][0], m[0][1]);
        float s1 = DifferenceOfProducts(m[0][0], m[1][2], m[1][0], m[0][2]);
        float s2 = DifferenceOfProducts(m[0][0], m[1][3], m[1][0], m[0][3]);

        float s3 = DifferenceOfProducts(m[0][1], m[1][2], m[1][1], m[0][2]);
        float s4 = DifferenceOfProducts(m[0][1], m[1][3], m[1][1], m[0][3]);
        float s5 = DifferenceOfProducts(m[0][2], m[1][3], m[1][2], m[0][3]);

        float c0 = DifferenceOfProducts(m[2][0], m[3][1], m[3][0], m[2][1]);
        float c1 = DifferenceOfProducts(m[2][0], m[3][2], m[3][0], m[2][2]);
        float c2 = DifferenceOfProducts(m[2][0], m[3][3], m[3][0], m[2][3]);

        float c3 = DifferenceOfProducts(m[2][1], m[3][2], m[3][1], m[2][2]);
        float c4 = DifferenceOfProducts(m[2][1], m[3][3], m[3][1], m[2][3]);
        float c5 = DifferenceOfProducts(m[2][2], m[3][3], m[3][2], m[2][3]);

        float determinant = InnerProduct(s0, c5, -s1, c4, s2, c3, s3, c2, s5, c0, -s4, c1);
        if (determinant == 0)
            return {};
        float s = 1 / determinant;

        float inv[4][4] = { {s * InnerProduct(m[1][1], c5, m[1][3], c3, -m[1][2], c4),
                            s * InnerProduct(-m[0][1], c5, m[0][2], c4, -m[0][3], c3),
                            s * InnerProduct(m[3][1], s5, m[3][3], s3, -m[3][2], s4),
                            s * InnerProduct(-m[2][1], s5, m[2][2], s4, -m[2][3], s3)},

                           {s * InnerProduct(-m[1][0], c5, m[1][2], c2, -m[1][3], c1),
                            s * InnerProduct(m[0][0], c5, m[0][3], c1, -m[0][2], c2),
                            s * InnerProduct(-m[3][0], s5, m[3][2], s2, -m[3][3], s1),
                            s * InnerProduct(m[2][0], s5, m[2][3], s1, -m[2][2], s2)},

                           {s * InnerProduct(m[1][0], c4, m[1][3], c0, -m[1][1], c2),
                            s * InnerProduct(-m[0][0], c4, m[0][1], c2, -m[0][3], c0),
                            s * InnerProduct(m[3][0], s4, m[3][3], s0, -m[3][1], s2),
                            s * InnerProduct(-m[2][0], s4, m[2][1], s2, -m[2][3], s0)},

                           {s * InnerProduct(-m[1][0], c3, m[1][1], c1, -m[1][2], c0),
                            s * InnerProduct(m[0][0], c3, m[0][2], c0, -m[0][1], c1),
                            s * InnerProduct(-m[3][0], s3, m[3][1], s1, -m[3][2], s0),
                            s * InnerProduct(m[2][0], s3, m[2][2], s0, -m[2][1], s1)} };

        return SquareMatrix<4>(inv);
    }


}

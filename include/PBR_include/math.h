#pragma once

#include <vector>

namespace PBR {

    // SquareMatrix Definition
    template <int N>
    class SquareMatrix {
    public:
        // SquareMatrix Public Methods
        PBRT_CPU_GPU
            static SquareMatrix Zero() {
            SquareMatrix m;
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    m.m[i][j] = 0;
            return m;
        }

        PBRT_CPU_GPU
            SquareMatrix() {
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    m[i][j] = (i == j) ? 1 : 0;
        }
        PBRT_CPU_GPU
            SquareMatrix(const Float mat[N][N]) {
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    m[i][j] = mat[i][j];
        }
        PBRT_CPU_GPU
            SquareMatrix(std::vector<const Float> t);
        template <typename... Args>
        PBRT_CPU_GPU SquareMatrix(Float v, Args... args) {
            static_assert(1 + sizeof...(Args) == N * N,
                "Incorrect number of values provided to SquareMatrix constructor");
            init<N>(m, 0, 0, v, args...);
        }
        template <typename... Args>
        PBRT_CPU_GPU static SquareMatrix Diag(Float v, Args... args) {
            static_assert(1 + sizeof...(Args) == N,
                "Incorrect number of values provided to SquareMatrix::Diag");
            SquareMatrix m;
            initDiag<N>(m.m, 0, v, args...);
            return m;
        }

        PBRT_CPU_GPU
            SquareMatrix operator+(const SquareMatrix& m) const {
            SquareMatrix r = *this;
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    r.m[i][j] += m.m[i][j];
            return r;
        }

        PBRT_CPU_GPU
            SquareMatrix operator*(Float s) const {
            SquareMatrix r = *this;
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    r.m[i][j] *= s;
            return r;
        }
        PBRT_CPU_GPU
            SquareMatrix operator/(Float s) const {
            DCHECK_NE(s, 0);
            SquareMatrix r = *this;
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    r.m[i][j] /= s;
            return r;
        }

        PBRT_CPU_GPU
            bool operator==(const SquareMatrix<N>& m2) const {
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    if (m[i][j] != m2.m[i][j])
                        return false;
            return true;
        }

        PBRT_CPU_GPU
            bool operator!=(const SquareMatrix<N>& m2) const {
            for (int i = 0; i < N; ++i)
                for (int j = 0; j < N; ++j)
                    if (m[i][j] != m2.m[i][j])
                        return true;
            return false;
        }

        PBRT_CPU_GPU
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

        std::string ToString() const;

        std::vector<const float> operator[](int i) const { return m[i]; }

    private:
        //Float m[N][N];
        std::vector<std::vector<float>> m[N][N];
    };




    template <int N>
    PBRT_CPU_GPU inline SquareMatrix<N> Transpose(const SquareMatrix<N>& m) {
        SquareMatrix<N> r;
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                r[i][j] = m[j][i];
        return r;
    }
}

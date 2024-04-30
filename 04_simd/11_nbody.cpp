#include <x86intrin.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

int main() {
    const int N = 8;
    float x[N], y[N], m[N], fx[N], fy[N];
    for (int i = 0; i < N; i++) {
        x[i] = drand48();
        y[i] = drand48();
        m[i] = drand48();
        fx[i] = fy[i] = 0;
    }

    // for (int i = 0; i < N; i++) {
    //     for (int j = 0; j < N; j++) {
    //         if (i != j) {
    //             float rx = x[i] - x[j];
    //             float ry = y[i] - y[j];
    //             float r = std::sqrt(rx * rx + ry * ry);
    //             fx[i] -= rx * m[j] / (r * r * r);
    //             fy[i] -= ry * m[j] / (r * r * r);
    //         }
    //     }
    //     printf("%d %g %g\n", i, fx[i], fy[i]);
    // }

    // 256 bit で十分なので __m256 を使ってみる
    constexpr auto LOAD = _mm256_loadu_ps;
    constexpr auto ZERO = _mm256_setzero_ps;
    constexpr auto ALL = _mm256_set1_ps;
    constexpr auto ADD = _mm256_add_ps;
    constexpr auto SUB = _mm256_sub_ps;
    constexpr auto MUL = _mm256_mul_ps;
    constexpr auto MUL_MASK = _mm256_mask_mul_ps;
    constexpr auto RSQRT = _mm256_rsqrt_ps;
    constexpr auto STORE = _mm256_store_ps;

    __m256 x8 = LOAD(x);
    __m256 y8 = LOAD(y);
    __m256 fx8 = ZERO();
    __m256 fy8 = ZERO();
    for (int j = 0; j < N; j++) {
        __m256 rx8 = SUB(x8, ALL(x[j]));
        __m256 ry8 = SUB(y8, ALL(y[j]));
        __m256 r8 = RSQRT(ADD(MUL(rx8, rx8), MUL(ry8, ry8)));
        r8 = MUL_MASK(ZERO(), ~(1 << j), MUL(r8, r8), MUL(r8, ALL(m[j])));
        fx8 = SUB(fx8, MUL(rx8, r8));
        fy8 = SUB(fy8, MUL(ry8, r8));
        
    }
    STORE(fx, fx8);
    STORE(fy, fy8);

    for (int i = 0; i < N; i++) {
        printf("%d %g %g\n", i, fx[i], fy[i]);
    }
}

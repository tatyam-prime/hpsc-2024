#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <vector>

const int n = 1e7;
const int range = 1e5;
int key[n];
int bucket[range + 1], offset[range + 1];
int main() {
    for (int i = 0; i < n; i++) {
        key[i] = rand() % range;
        printf("%d ", key[i]);
    }
    printf("\n");
    
    // std::copy(std::begin(key), std::end(key), std::begin(ans));
    // std::sort(std::begin(ans), std::end(ans));

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        #pragma omp atomic
        bucket[key[i] + 1]++;
    }

    // parallel prefix sum
    #pragma omp parallel for
    for (int i = 0; i <= range; i++) offset[i] = bucket[i];
    
    for (int w = 1; w < range; w <<= 1) {
        #pragma omp parallel for
        for (int i = 1; i <= range; i++) bucket[i] = offset[i];
        #pragma omp parallel for
        for (int i = w; i <= range; i++) offset[i] += bucket[i - w];
    }

    assert(offset[range] == n);

    #pragma omp parallel for
    for (int i = 0; i < range; i++) {
        #pragma omp parallel for
        for (int j = offset[i]; j < offset[i + 1]; j++) key[j] = i;
    }

    for (int i = 0; i < n; i++) {
        printf("%d ", key[i]);
    }
    printf("\n");

    // for(int i = 0; i < n; i++) assert(key[i] == ans[i]);
}

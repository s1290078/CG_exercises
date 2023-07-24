#include <stdio.h>
#include <math.h>

#define N 12
#define M 20

void saveIcosahedron(const char* fn);

int main() {
    saveIcosahedron("output.txt");
    return 0;
}

void saveIcosahedron(const char* fn) {
    FILE* f = fopen(fn, "w");
    fprintf(f, "OFF\n");

    float phi = (1.f + sqrtf(5.f)) * .5f;
    float a = 1.f;
    float b = 1.f / phi;
    float v[N][3] = { {0.f,b,-a}, {b,a,0.f}, {-b,a,0.f},
    {0.f,b,a}, {0.f,-b,a}, {-a,0.f,b},
    {0.f,-b,-a}, {a,0.f,-b}, {a,0.f,b},
    {-a,0.f,-b},{b,-a,0.f},{-b,-a,0.f}};

    int fa[M][3] = { {2,1,0}, {1,2,3}, {5,4,3}, {4,8,3},
    {7,6,0}, {6,9,0}, {11,10,4}, {10,11,6},
    {9,5,2}, {5,9,11}, {8,7,1}, {7,8,10},
    {2,5,3}, {8,1,3}, {9,2,0}, {1,7,0},
    {11,9,6}, {7,10,6}, {5,11,4}, {10,8,4}
    };

    fprintf(f, "%d %d 0\n", N, M);
    for (int i = 0; i < N; ++i) {
        fprintf(f, "%f %f %f\n", v[i][0], v[i][1], v[i][2]);
    }

    for (int i = 0; i < M; ++i) {
        fprintf(f, "3 %d %d %d\n", fa[i][0], fa[i][1], fa[i][2]);
    }

    fclose(f);
}
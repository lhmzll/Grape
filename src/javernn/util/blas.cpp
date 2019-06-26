#include <math.h>
#include <assert.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "javernn/util/blas.h"

namespace javernn
{
    void mul_cpu(int N, float *X, int INCX, float *Y, int INCY)
    {
        int i;
        for(i = 0; i < N; ++i) Y[i*INCY] *= X[i*INCX];
    }

    void pow_cpu(int N, float ALPHA, float *X, int INCX, float *Y, int INCY)
    {
        int i;
        for(i = 0; i < N; ++i) Y[i*INCY] = pow(X[i*INCX], ALPHA);
    }

    void axpy_cpu(int N, float ALPHA, float *X, int INCX, float *Y, int INCY)
    {
        int i;
        for(i = 0; i < N; ++i) Y[i*INCY] += ALPHA*X[i*INCX];
    }

    void scal_cpu(int N, float ALPHA, float *X, int INCX)
    {
        int i;
        for(i = 0; i < N; ++i) X[i*INCX] *= ALPHA;
    }

    void fill_cpu(int N, float ALPHA, float *X, int INCX)
    {
        int i;
        for(i = 0; i < N; ++i) X[i*INCX] = ALPHA;
    }

    void copy_cpu(int N, float *X, int INCX, float *Y, int INCY)
    {
        int i;
        for(i = 0; i < N; ++i) Y[i*INCY] = X[i*INCX];
    }

    void mult_add_into_cpu(int N, float *X, float *Y, float *Z)
    {
        int i;
        for(i = 0; i < N; ++i) Z[i] += X[i]*Y[i];
    }

    void add_cpu(int N, float *X, int INCX, float *Y, int INCY)
    {
        int i;
        for(i = 0; i < N; ++i) Y[i*INCY] += X[i*INCX];
    }
} // namespace javernn





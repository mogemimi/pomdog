#ifndef SharedStructures_h
#define SharedStructures_h

#include <simd/simd.h>

typedef struct __attribute__((__aligned__(256)))
{
    matrix_float4x4 Model;
    matrix_float4x4 ViewProjection;
} MyShaderConstants;

#endif /* SharedStructures_h */

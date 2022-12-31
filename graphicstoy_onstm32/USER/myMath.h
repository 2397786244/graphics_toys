#ifndef _MY_MATH_H
#define _MY_MATH_H

struct vec2{
		float u;
		float v;
};

struct vec3{
		float x;
		float y;
		float z;
};

typedef struct vec2 vec2;
typedef struct vec3 vec3;

//向量初始化
vec2 VEC2(float u,float v);
vec3 VEC3(float x,float y,float z);
//向量与浮点数相乘
vec2 vec2multi(vec2 v,float f);
vec3 vec3multi(vec3 v,float f);
//向量与向量相乘
vec2 vec2xvec2(vec2 v1,vec2 v2);
vec3 vec3xvec3(vec3 v1,vec3 v2);
//点乘
float dot(vec3 v1,vec3 v2);
//标准化
vec3 normalize(vec3 v);
float max(float f1,float f2);
float min(float f1,float f2);
float length(vec3 v);
float lengthv2(vec2 v);
vec2 vec2add(vec2 v,float f);
vec3 vec3add(vec3 v,float f);
vec3 vec3addv(vec3 v1,vec3 v2);
vec3 vec3subv(vec3 v1,vec3 v2);
vec2 vec2subv(vec2 v1,vec2 v2);
vec3 fract(vec3 v);
float fractf(float f);
vec3 mixv3(vec3 a,vec3 b ,float f);
float mix(float f1,float f2,float a);
vec3 vec3max(vec3 v1,vec3 v2);
float sign(float f);
float smoothstep(float t1,float t2,float x);
float clamp(float x,float min,float max);
vec3 cross(vec3 v1,vec3 v2);  //crossProduct
float distance(vec3 p0,vec3 p1);
vec3 vec3abs(vec3 v);
vec2 vec2abs(vec2 v);
vec2 vec2max(vec2 v1,vec2 v2);
float step(float a,float b);
vec3 reflect(vec3 r,vec3 n);
vec3 vec3min(vec3 v1,vec3 v2);
//matrix
struct mat3{
		float m33[9];
};
typedef struct mat3 mat3;

//matrix init
mat3 MAT3(float a11,float a12,float a13,
	float a21,float a22,float a23,
		float a31,float a32,float a33);
mat3 MAT3From3v(vec3 x,vec3 y,vec3 z);
vec3 Mat3xVec3(mat3 m3,vec3 v3);
	
struct mat2{
			float m22[4];
	};
typedef struct mat2 mat2;
mat2 MAT2(float a11,float a12,float a21,float a22);	
vec2 Mat2xVec2(mat2 m2,vec2 v2);
	
#endif
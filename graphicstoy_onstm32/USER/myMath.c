#include "myMath.h"
#include "math.h"

//向量初始化
vec2 VEC2(float u,float v){
	vec2 vv;
	vv.u = u;
	vv.v = v;
	return vv;
}
vec3 VEC3(float x,float y,float z){
	vec3 vv;
	vv.x = x;vv.y = y;vv.z = z;
	return vv;
}
//向量与浮点数相乘
vec2 vec2multi(vec2 v,float f){
	v.u *= f;
	v.v *= f;
	return v;
}
vec3 vec3multi(vec3 v,float f){
	v.x *= f;
	v.y *= f;
	v.z *= f;
	return v;
}
//向量与向量相乘
vec2 vec2xvec2(vec2 v1,vec2 v2){
	vec2 vv;
	vv.u = v1.u * v2.u;
	vv.v = v1.v * v2.v;
	return vv;
}
vec3 vec3xvec3(vec3 v1,vec3 v2){
	vec3 vv;
	vv.x = v1.x * v2.x;
	vv.y = v1.y * v2.y;
	vv.z = v1.z * v2.z;
	return vv;
}
//点乘
float dot(vec3 v1,vec3 v2){
		return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}
//标准化
vec3 normalize(vec3 v){
	float l = length(v);
	v.x /= l;
	v.y /= l;
	v.z /= l;
	return v;
}
float max(float f1,float f2){
	return (f1 > f2 ) ? f1 : f2;
}
float length(vec3 v){
	return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vec2 vec2add(vec2 v,float f){
	v.u += f;
	v.v += f;
	return v;
}
vec3 vec3add(vec3 v,float f){
	v.x += f;
	v.y += f;
	v.z += f;
	return v;
}

vec3 vec3addv(vec3 v1,vec3 v2){
	v1.x += v2.x;
	v1.y += v2.y;
	v1.z += v2.z;
	return v1;
}

vec3 fract(vec3 v){
		v.x -= floorf(v.x);
	v.y -= floorf(v.y);
	v.z -= floorf(v.z);
	return  v;
}

vec3 vec3max(vec3 v1,vec3 v2){
		vec3 vvv;
	vvv.x = (v1.x > v2.x) ? v1.x : v2.x;
	vvv.y = (v1.y > v2.y) ? v1.y : v2.y;
	vvv.z = (v1.z > v2.z) ? v1.z : v2.z;
	return vvv;
}
vec3 vec3min(vec3 v1,vec3 v2){
		vec3 vvv;
	vvv.x = (v1.x < v2.x) ? v1.x : v2.x;
	vvv.y = (v1.y < v2.y) ? v1.y : v2.y;
	vvv.z = (v1.z < v2.z) ? v1.z : v2.z;
	return vvv;
}

float lengthv2(vec2 v){
	
	return (sqrt(v.u * v.u + v.v * v.v));
}

vec3 mixv3(vec3 a,vec3 b ,float f){
	return vec3addv(vec3multi(a,(1 - f)),vec3multi(b,f));
}

float mix(float f1,float f2,float a){
	return f1 * (1-a) + f2 * a;
}

float smoothstep(float t1, float t2, float x) {
  // Scale, bias and saturate x to 0..1 range
  // ??????remap??????
  x = clamp((x - t1) / (t2 - t1), 0.0, 1.0); 
  // Evaluate polynomial
  return x * x * (3 - 2 * x);
}

float clamp(float x,float min,float max)
{
	if(x < min)
		return min;
	else if(x > max)
		return max;
	else 
		return x;
}

vec3 cross(vec3 v1,vec3 v2)
{
	vec3 vv;
	vv.x = v1.y * v2.z - v2.y * v1.z;
	vv.y = v2.x * v1.z - v1.x * v2.z;
	vv.z = v1.x * v2.y - v2.x * v1.y;
	return vv;
}

mat3 MAT3(float a11,float a12,float a13,
	float a21,float a22,float a23,
		float a31,float a32,float a33){
			mat3 t;
			t.m33[0] = a11;
			t.m33[1] = a12;
			t.m33[2] = a13;
			t.m33[3] = a21;
			t.m33[4] = a22;
			t.m33[5] = a23;
			t.m33[6] = a31;
			t.m33[7] = a32;
			t.m33[8] = a33;
			return t;
		}
		
		float min(float f1,float f2){
			return (f1 < f2) ? f1 : f2;
		}
		
		vec3 vec3subv(vec3 v1,vec3 v2){
				v1.x -= v2.x;
				v1.y -= v2.y;
				v1.z -= v2.z;
			return v1;
		}
		
mat3 MAT3From3v(vec3 x,vec3 y,vec3 z){
	mat3 t;
	t.m33[0] = x.x;
	t.m33[3] = x.y;
	t.m33[6] = x.z;
	
	t.m33[1] = y.x;
	t.m33[4] = y.y;
	t.m33[7] = y.z;
	
	t.m33[2] = z.x;
	t.m33[5] = z.y;
	t.m33[8] = z.z;
	return t;
}	

vec3 vec3abs(vec3 v){
		return VEC3(fabs(v.x),fabs(v.y),fabs(v.z));
}

float sign(float f){
	if(f < 0.0f)
	{return -1.0f;}
	else if(f > 0.0f)
	{	return 1.0f;}
	else
	{return 0.0;}
}

vec2 vec2abs(vec2 v){
	return VEC2(fabs(v.u),fabs(v.v));
}
vec2 vec2max(vec2 v1,vec2 v2){
		vec2 vv;
	vv.u = (v1.u > v2.u) ? v1.u : v2.u;
	vv.v = (v1.v > v2.v) ? v1.v : v2.v;
	return vv;
}

vec2 vec2subv(vec2 v1,vec2 v2){
	v1.u -= v2.u;
	v1.v -= v2.v;
	return v1;
}
vec3 Mat3xVec3(mat3 m3,vec3 v3){
	vec3 tt;
	
	tt.x = m3.m33[0] * v3.x + m3.m33[1] * v3.y + m3.m33[2] * v3.z;
	tt.y = m3.m33[3] * v3.x + m3.m33[4] * v3.y + m3.m33[5] * v3.z;
	tt.z = m3.m33[6] * v3.x + m3.m33[7] * v3.y + m3.m33[8] * v3.z;
	return tt;
}

float fractf(float f){
		return f - floorf(f);
}

mat2 MAT2(float a11,float a12,float a21,float a22){
		mat2 m22;
		m22.m22[0] = a11;
		m22.m22[1] = a12;
		m22.m22[2] = a21;
		m22.m22[3] = a22;
	return m22;
}
vec2 Mat2xVec2(mat2 m2,vec2 v2){
		vec2 v;
		v.u = m2.m22[0] * v2.u + m2.m22[1] * v2.v;
		v.v = m2.m22[2] * v2.u + m2.m22[3] * v2.v;
	return v;
}

float distance(vec3 p0,vec3 p1){
		return length( vec3subv(p0,p1));
}
float step(float edge,float x){
	return (x < edge) ? 0. : 1. ;
}
vec3 reflect(vec3 r,vec3 n){
		return vec3subv(r , vec3multi(n , 2.0  * dot(n,r) ));
}
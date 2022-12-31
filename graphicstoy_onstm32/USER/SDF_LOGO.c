#include "SDF_LOGO.h"
#include "usart.h"

const float tmin = 0.01f;
const float tmax = 80.0f;

vec2 fixUV(vec2 fc)
{
    vec2 fixUV = vec2multi(fc,2.0f);
		fixUV.u -= WIDTH;
		fixUV.v -= HEIGHT;
		fixUV.u /= HEIGHT;
		fixUV.v /= HEIGHT;
	return fixUV;
}

mat3 setCamera(vec3 ro,vec3 t,float theta)
{
    vec3 z = normalize(vec3subv(t,ro));
    vec3 cp = normalize( VEC3(sin(theta),cos(theta),0.) );
    vec3 x = normalize(cross(z , cp));
    vec3 y = normalize(cross(x,z) );
    return MAT3From3v(x,y,z);
}

/*
SDFs
*/

float sdTorus( vec3 p, vec2 t)
{
  vec2 q = VEC2(lengthv2(VEC2(p.x,p.z))- t.u ,p.y);
  return lengthv2(q)-t.v;
}

float plane(vec3 p)
{
    return p.y;
}

float sdBox( vec3 p, vec3 b)
{
  vec3 d = vec3subv(vec3abs(p) , b );
  return length( vec3max( d , VEC3(0.0,0.0,0.0)) ) + min( max( d.x,max(d.y,d.z)),0.0);
}

float sdSphere( vec3 p,float r)
{
    return length(p) - r;
}
//???
float sdTriPrism( vec3 p, vec2 h )
{
  vec3 q = vec3abs(p);
  return max(q.z-h.v ,max(q.x*0.866025+p.y*0.5,-p.y)-h.u*0.5f);
}

float dot2( vec3 v ) { return dot(v,v); }

float Triangle( vec3 p, vec3 a, vec3 b, vec3 c )
{
  vec3 ba = vec3subv(b , a); vec3 pa = vec3subv(p , a);
  vec3 cb = vec3subv(c , b); vec3 pb = vec3subv(p , b);
  vec3 ac = vec3subv(a , c); vec3 pc = vec3subv(p , c);
  vec3 nor = cross( ba, ac );
	
  return sqrt(
    (sign(dot(cross(ba,nor),pa)) +
     sign(dot(cross(cb,nor),pb)) +
     sign(dot(cross(ac,nor),pc))<2.0)
     ?
     min( min(
     dot2(vec3subv(vec3multi(ba,  clamp(dot(ba,pa)/dot2(ba),0.0,1.0)),pa)),
     dot2(vec3subv(vec3multi(cb,clamp(dot(cb,pb)/dot2(cb),0.0,1.0)),pb)) ),
     dot2(vec3subv(vec3multi(ac,clamp(dot(ac,pc)/dot2(ac),0.0,1.0)),pc)) )
     :
     dot(nor,pa)*dot(nor,pa)/dot2(nor) );
	
}

float sdCappedCylinder( vec3 p, float h, float r )
{
  vec2 d = vec2subv( vec2abs(VEC2(lengthv2(VEC2(p.x,p.z)),p.y)) , VEC2(h,r));
  return min(max(d.u,d.v),0.0) + lengthv2(vec2max(d,VEC2(0.0,0.0)));
}

vec2 vec2Min(vec2 a,vec2 b)
{
    return (a.u < b.u) ? a : b;
}

/*
    None -1 
    Sphere : 0
    Plane : 1
    Box : 2
    Torus : 3
    TriPrism : 4
    Tri : 5
    Cylinder:6
*/

//????
float opUnion(float d1,float d2)
{
    return min(d1,d2);
}

//????
float opInter(float d1,float d2)
{
    return max(d1,d2);
}

//????.????d1???d2
float opSub(float d1,float d2)
{
    return opInter(d1,-d2);
}
vec2 map( vec3 p )
{
    float floor = plane(p);
    vec2 plane = VEC2(floor,1.);
	
		vec3 vvv = vec3subv(p , VEC3(0.,1.05,0.0));
		vvv = VEC3(vvv.x,vvv.z,vvv.y);
    float outer_d = sdCappedCylinder(vvv,0.95,0.03);
    //??????
    float minus_outer_d = sdCappedCylinder(vvv,0.90,0.1);
    vec2 outer = VEC2(opSub(outer_d,minus_outer_d),3.);
    float inner_d = sdCappedCylinder(vvv,0.75,0.03);
    //??????
    float minus_inner_d = sdCappedCylinder(vvv,0.70,0.1);
    vec2 inner = VEC2(opSub(inner_d,minus_inner_d),3.);
    //??
		vvv = vec3subv(p , VEC3(0.0,1.15,0.0)) ;
    float building_d = sdBox(vvv,VEC3(0.32,0.35,0.01));
    //????,?main_building ???????box
		vvv = vec3addv(vvv,VEC3(0.,0.2,0.0));
    float minus_from_main_d1 = sdBox(vvv,VEC3(0.18,0.22,0.05));
    vec2 main_building = VEC2(opSub(building_d,minus_from_main_d1),2.);
    //????,?main_building????????box??
		vvv = vec3subv(p , VEC3(0.0,1.15,0.0));
		vvv = vec3subv(vvv,VEC3(0.,0.1,0.0));
    float minus_from_main_d2 = sdBox(vvv,VEC3(0.08,0.5,0.05));
    float main_building_d = opSub(opSub(building_d,minus_from_main_d1),minus_from_main_d2);
    main_building = VEC2(main_building_d,2.);
    //????
		vvv = vec3subv(p , VEC3(0.0,1.15,0.0));
    float roof_d = Triangle(vec3subv( vvv , VEC3(-0.08,0.35,0.0)),VEC3(0.,0.,0.),VEC3(0.0,0.10,0.0),VEC3(-0.25,0.0,0.0));
    main_building_d = opUnion(main_building_d,roof_d);
    //????
    float roof_d2 = Triangle(vec3subv(vvv , VEC3(0.08,0.35,0.0)),VEC3(0.,0.,0.),VEC3(0.0,0.10,0.0),VEC3(0.25,0.0,0.0));
    main_building_d = opUnion(main_building_d,roof_d2);
    //????,??sdBox
		vvv = vec3subv(p , VEC3(0.0,0.8,0.0));
    float step_d1 = sdBox(vec3addv( vvv, VEC3(0.0,-0.08,0.0) ),VEC3(0.12,0.01,0.05));
    main_building_d = opUnion(main_building_d,step_d1);
    float step_d2 = sdBox( vec3addv(vvv, VEC3(0.0,-0.04,0.0)),VEC3(0.12,0.01,0.05));
    main_building_d = opUnion(main_building_d,step_d2);
    float step_d3 = sdBox( vec3addv(vvv , VEC3(0.0,0.02,0.0)),VEC3(0.25,0.01,0.05));
    main_building_d = opUnion(main_building_d,step_d3);
    float step_d4 = sdBox(vec3addv( vvv, VEC3(0.0,0.08,0.0) ),VEC3(0.36,0.01,0.05));
    main_building_d = opUnion(main_building_d,step_d4);
    main_building = VEC2(main_building_d,4.);
    
    return vec2Min(vec2Min(vec2Min(outer,plane),inner),main_building);  //????????sdf????? 
}

// p is a point on the box.
vec3 calcNormal(vec3 p)
{
    const float h = 0.0001;
    const vec2 k = VEC2(1,-1);
		vec3 v_xyy = VEC3(k.u,k.v,k.v);
		vec3 v_yyx = VEC3(k.v,k.v,k.u);
		vec3 v_yxy = VEC3(k.v,k.u,k.v);
		vec3 v_xxx = VEC3(k.u,k.u,k.u);
		vec3 temp;
		v_xyy = vec3multi( v_xyy, map(vec3addv(p , vec3multi(v_xyy,h))).u);
		v_yyx = vec3multi( v_yyx, map(vec3addv(p , vec3multi(v_yyx,h))).u);
		v_yxy = vec3multi( v_yxy, map(vec3addv(p , vec3multi(v_yxy,h))).u);
		v_xxx = vec3multi( v_xxx, map(vec3addv(p , vec3multi(v_xxx,h))).u);
		temp = v_xyy;
		temp = vec3addv(temp,v_yyx);
		temp = vec3addv(temp,v_yxy);
		temp = vec3addv(temp,v_xxx);
    return 
        normalize(temp
       );
}

vec2 rayMarch(vec3 o,vec3 d)
{
    
    const float e = 0.0001;
    const int max_iter_cnt = 100;
    __IO float t = tmin;
    __IO int i = 0;
    vec3 pos;
    __IO float rr;
    for(;i < max_iter_cnt && t <= tmax;i++)
    {
          pos = vec3addv(o , vec3multi(d , t));
          vec2 obj = map(pos);
          rr = obj.u;
          if(rr < e)
          {
                return VEC2(t,obj.v);
          }
          else
          {
              t += rr;
          }
    }
    return VEC2(t,-1.);

}

vec3 render(vec2 uv)
{
    RTC_TimeTypeDef time;
		RTC_GetTime(RTC_Format_BIN,&time);
		float iTime = (float)(time.RTC_Seconds + time.RTC_Minutes * 60);
    vec3 camPos = VEC3(-0.5,1.2,2.5);
    vec3 target = VEC3(0.,0.,0.);
    vec3 col = VEC3(0.2,0.2,0.2);
    mat3 camMat = setCamera(camPos,target,0.0);
    vec3 frag = VEC3(uv.u,uv.v,1.0);
    vec3 rd = normalize(Mat3xVec3(camMat , frag));
    vec2 t = rayMarch(camPos,rd);
    if( t.v >= 0.0 )
    {
        //????
        vec3 p = vec3addv(camPos , vec3multi(rd,t.u) );
        vec3 n = calcNormal(p);
        vec3 LightPos = VEC3(0.8,2.5
       ,0.5);
        float LLength = length(vec3subv(LightPos , p));
        vec3 LightDir = normalize(vec3subv(LightPos , p));
        float diff = clamp(dot(LightDir,n),0.,1.) / (LLength * LLength);
        vec3 objcol = VEC3(0.,0.,0.);
        if(fabs(t.v) < 0.001)
        {
            // Sphere
            objcol = VEC3(1.0,0.,0.);
        }
        else if(t.v < 1.1 && t.v > 0.9)
        {
            //Plane
            vec2 planeUV = VEC2(floor(p.x),floor(p.z));
            objcol = vec3add( VEC3(0.2,0.2,0.2) ,  0.2 * fmod(planeUV.u + planeUV.v ,2.0));
        }
        else if(t.v < 2.1 && t.v > 1.9)
        {
            //Box    
            objcol = VEC3(1.,0.,0.);
        }
        
        else if (t.v < 3.1 && t.v > 2.9)
        {
            //Torus
            objcol = VEC3(1.,0.,0.0);
        }
        else{
            //other Objects
            objcol = VEC3(1.,0.,0.);
        
        }
        col = vec3addv(vec3multi(objcol,(0.5 + 0.5 * dot(n , VEC3(0.,1.,0.) ) ) ) , VEC3(diff,diff,diff) );
    }
		col.x = sqrt(col.x);
		col.y = sqrt(col.y);
		col.z = sqrt(col.z);
    return col;
}

uint32_t mainImage_LOGO(vec2 fragCoord )
{
    vec2 uv = fixUV(fragCoord);
    vec3 col = render(uv);
    
		col.x = (col.x > 1.0f) ? 1.0 : col.x;
		col.y = (col.y > 1.0f) ? 1.0 : col.y;
		col.z = (col.z > 1.0f) ? 1.0 : col.z;
		return SetRGB24((uint32_t)(col.x * 255),(uint32_t)(col.y * 255),(uint32_t)(col.z * 255));
}
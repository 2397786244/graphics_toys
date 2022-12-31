#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "key.h"
#include "lcd.h"
#include "rng.h"
#include "math.h"
#include "myMath.h"
#include "rtc.h"
#include "SDF_LOGO.h"
#include "bunny.h"

__IO uint16_t image[WIDTH * HEIGHT];

//爱心定义函数
float heart(vec3 p)
{
	vec3 pp;
	pp = vec3xvec3(p,p);
	vec3 ppp = vec3xvec3(pp,p);
	float a = pp.x + 2.25f * pp.y + pp.z - 1.0f;
	return a * a * a - (pp.x + 0.1125* pp.y) * ppp.z;
}

float h(float x,float z)
{
	float a = 0.0f, b = 0.75f, y = 0.5f;
	int i;
    for (i = 0; i < 10; i++) {
        if (heart(VEC3(x, y, z)) <= 0.0f)
            a = y;
        else
            b = y;
        y = (a + b) * 0.5;
    }
    return y;
}

vec3 normal(vec2 p) {
    vec3 v = VEC3(p.u, h(p.u, p.v), p.v);
    vec3 vv = vec3xvec3(v,v);
    vec3 vvv = vec3xvec3(vv , v);
    float a = -1.0f + dot(vv, VEC3(1, 2.25, 1));
   	a *= a;
    
  	return normalize(VEC3(
        -2.0 * v.x * vvv.z +  6.0 * v.x * a,
      -0.225 * v.y * vvv.z + 13.5 * v.y * a,
      v.z * (-3.0 * vv.x * v.z - 0.3375 * vv.y * v.z + 6.0 * a)));
}

uint32_t mainImage_3dHeart(vec2 fragCoord)
{
		RTC_TimeTypeDef time;
		RTC_GetTime(RTC_Format_BIN,&time);
		float t = (float)(time.RTC_Seconds + time.RTC_Minutes * 60);
		vec2 fixUV = vec2multi(fragCoord,2.0f);
		fixUV.u -= WIDTH;
		fixUV.v -= HEIGHT;
		fixUV.u /= HEIGHT;
		fixUV.v /= HEIGHT;
		vec3 p = VEC3(fixUV.u,fixUV.v, 0.0f);
    float s = sin(t * 3.0);
    s *= s;
    s *= s;
    s *= 0.1;
    vec3 tp = VEC3(1.0 + s, 1.0 - s, 0.0);
		tp = vec3multi(tp,2.0f);
    tp = vec3xvec3(p,tp);
    vec3 c;
    if (heart(VEC3(tp.x,tp.z,tp.y)) <= 0.0) {
        vec3 n = normal(VEC2(tp.x,tp.y));
        float diffuse = dot(n, normalize(VEC3(-1.0, 1.0, 1.0))) * 0.5 + 0.5;
        float specular = pow(max(dot(n, normalize(VEC3(-1.0, 2.0, 1.0))), 0.0), 48.0);
        c = vec3multi(VEC3(1.0,0.0,0.0),diffuse);
				float rim = 1.0 - dot(n, VEC3(0.0, 1.0, 0.0));
				c = vec3addv(c,vec3multi(VEC3(0.8f,0.8f,0.8f),specular));
				c = vec3addv(c,vec3multi(VEC3(0.5f,0.5f,0.5f),rim));
				c.x = (c.x > 1.0f) ? 1.0 : c.x;
				c.y = (c.y > 1.0f) ? 1.0 : c.y;
				c.z = (c.z > 1.0f) ? 1.0 : c.z;
		}
	else
        c = vec3multi(VEC3(1.0, 0.8, 0.7 - 0.07 * p.y) , (1.0 - 0.15 * length(p)));
   
	return SetRGB24((uint32_t)(c.x * 255),(uint32_t)(c.y * 255),(uint32_t)(c.z * 255));
}

// FireWork.

#define PI 3.141592653589793

#define EXPLOSION_COUNT 3.
#define SPARKS_PER_EXPLOSION 50.
#define EXPLOSION_DURATION 12.
#define EXPLOSION_SPEED 5.
#define EXPLOSION_RADIUS_THESHOLD .06

// Hash function by Dave_Hoskins.
#define MOD3 VEC3(.1031,.11369,.13787)
vec3 hash31(float p) {
   vec3 p3 = fract(vec3xvec3(VEC3(p,p,p),MOD3));
   p3 = vec3add(p3,dot(p3, VEC3(p3.y+ 19.19, p3.z+19.19, p3.x + 19.19)));
   return fract(VEC3((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y, (p3.y + p3.z) * p3.x));
}

uint32_t mainImage_fireWork(vec2 fragCoord)
{	
		RTC_TimeTypeDef time;
		RTC_GetTime(RTC_Format_BIN,&time);
		float iTime = (float)(time.RTC_Seconds + time.RTC_Minutes * 60);
		
    float aspectRatio = WIDTH / HEIGHT;
    vec2 uv = VEC2(fragCoord.u / WIDTH,fragCoord.v / HEIGHT);
    float t = fmod(iTime + 10.0 ,7200.0);
		vec3 col = VEC3(0.,0.,0.); 
    vec2 origin = VEC2(0.,0.);
    
    for (float j = 0.; j < EXPLOSION_COUNT; ++j)
    {
        vec3 oh = hash31((j + 1234.1939) * 641.6974);
        origin = vec2add(vec2multi(VEC2(oh.x, oh.y),0.6), .2); // .2 - .8 to avoid boundaries
        origin.u *= aspectRatio;
        // Change t value to randomize the spawning of explosions
        t += (j + 1.) * 9.6491 * oh.z;
        for (float i = 0.; i < SPARKS_PER_EXPLOSION; ++i)
    	{
            // Thanks Dave_Hoskins for the suggestion
            vec3 h = hash31(j * 963.31 + i + 497.8943);
            // random angle (0 - 2*PI)
            float a = h.x * PI * 2.;
            // random radius scale for spawning points anywhere in a circle
            float rScale = h.y * EXPLOSION_RADIUS_THESHOLD;
            // explosion loop based on time
            if (fmod(t * EXPLOSION_SPEED, EXPLOSION_DURATION) > 2.)
            {
                // random radius 
                float r = fmod(t * EXPLOSION_SPEED, EXPLOSION_DURATION) * rScale;
                // explosion spark polar coords 
                vec2 sparkPos = VEC2(r * cos(a), r * sin(a));
               	sparkPos.v -= pow(sparkPos.u, 4.); // fake gravity
                // shiny spark particles
								vec2 tempv2 = uv;
								tempv2.u -= (sparkPos.u + origin.u);
								tempv2.v -= (sparkPos.v + origin.v);
                float spark = .0002/pow(lengthv2(tempv2), 1.65);
                // Make the explosion spark shimmer/sparkle
								tempv2 = origin;
								tempv2.u -= sparkPos.u;
								tempv2.v -= sparkPos.v;
                float sd = 2. * lengthv2(tempv2);
                float shimmer = max(0., sqrt(sd) * (sin((t + h.y * 2. * PI) * 20.)));
                float shimmerThreshold = EXPLOSION_DURATION * .32;
                // fade the particles towards the end of explosion
                float fade = max(0., (EXPLOSION_DURATION - 5.) * rScale - r);
                // mix it all together
                col = vec3addv(col , vec3multi(oh , mix(spark, shimmer * spark, smoothstep(shimmerThreshold * rScale,
					(shimmerThreshold + 1.) * rScale , r) ) * fade ));
            }
    	}
    }
    
    // evening-sh background gradient
    col = vec3max(VEC3(0.1,0.1,0.1), col);
    col = vec3addv(col,vec3multi(VEC3(.12, .06, .02) , (1.- uv.v)));
		col.x = (col.x > 1.0f) ? 1.0 : col.x;
		col.y = (col.y > 1.0f) ? 1.0 : col.y;
		col.z = (col.z > 1.0f) ? 1.0 : col.z;
		return SetRGB24((uint32_t)(col.x * 255),(uint32_t)(col.y * 255),(uint32_t)(col.z * 255));
} 

//绘图函数
void ScreenImagePrepare()
{
	int i,j;
	for(i = 0;i<HEIGHT;i++)
	{
		for(j = 0;j<WIDTH;j++){
			image[i * WIDTH + j] = RGB24ToRGB16Convert(mainImage_DrawBunny(VEC2((float)j,(float)i)));
		}
	}
}

int main(void)
{ 
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);		//延时初始化 
	uart_init(115200);	//串口初始化波特率为115200
	LCD_Config();
	LCD_Reg_Config();
	RNG_Init();
	Counter_Init();
	LCD_BackLight_ON;
	printf("the manufacture id %02x \r\n",LCD_Read_ID4());
	while(1)
	{
		ScreenImagePrepare();
		LCD_FullScreenWrite(WIDTH,HEIGHT,(uint16_t *)image);
		printf("Finish a frame.\r\n");
	}
}


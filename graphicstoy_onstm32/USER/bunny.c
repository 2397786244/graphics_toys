#include "bunny.h"

float time;

// Polynomial smooth min (k = 0.1) (from IQ)
float smin( float a, float b, float k )
{
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}


float smax(float a,float b, float k)
{
    return -smin(-a,-b,k);
}

mat2 rotmat(float a)
{
    return MAT2(cos(a),sin(a),-sin(a),cos(a));
}

float cylinder(vec3 p,vec3 dir,float h,float r)
{
    float t=dot(p,dir);
    float d=distance(p,vec3multi(dir,t));
    return lengthv2( vec2max( vec2subv (vec2abs(VEC2(d,t)),VEC2(r,h)),VEC2(0.0,0.0)));
}

float pupdist=1e4;

float rabdist(vec3 p)
{
    float an=.5*.5*2.*6. + 1./ 6.;
		vec2 rr = Mat2xVec2 (MAT2(cos(an),sin(an),sin(an),-cos(an)),VEC2(p.x,p.z));
    p.x = rr.u;
    p.z = rr.v;
		
    float time2=time*2.4;

    p.y+=2.3;
		rr = Mat2xVec2(rotmat(cos(time2+1.)*.04),VEC2(p.x,p.y));
    p.x = rr.u;
		p.y = rr.v;
	
    p.y-=2.3;

    vec3 op=p;

    vec3 p2=p;
		rr= Mat2xVec2(rotmat(cos(time2)*.1),VEC2(p2.x,p2.y));
    p2.x = rr.u;
		p2.y = rr.v;
	
    vec3 p3=p;
		rr= Mat2xVec2(rotmat(cos(time2-.0-length(p)/2.)*.13),VEC2(p3.x,p3.y));
    p3.x = rr.u;
		p3.y = rr.v;

    float d=1e4;
    p.x=fabs(p.x);
    p2.x=fabs(p2.x);
    p3.x=fabs(p3.x);

    d=smin(length( vec3subv(p2,VEC3(-.75,0.,-.1))) -.4 ,length( vec3subv(p2,VEC3(.75,0.,-.1)) )-.5,2.);
    d=smin(d,length( vec3subv(p2,VEC3(0,0.4,-.1)))-.9,1.6);
    d+=.1;  
    
    // Ears 1
    d=smin(d,distance(VEC3(.7,clamp(p3.y,0.,2.2),0.),VEC3(p3.x,p3.y,p3.z))-.4,.14);   
    d=smax(d,-(length(vec3subv(p3,VEC3(.7,1.7,-0.5)))-.5),.2);

    // Neck
    d=smin(d,distance(VEC3(0.,clamp(p.y,-1.6,-1.1),0.),VEC3(p.x,p.y,p.z))-.6,.04);   
    
    // Legs
    vec3 p4=op;
    float ld=-.75;
    p4.y-=ld;
		vec2 ttt = Mat2xVec2(rotmat(cos(time*2.+3.1415926/1.*0.)*.1),VEC2(p4.y,p4.z));
    p4.y = ttt.u;
		p4.z = ttt.v;
    p4.y+=ld;
    p4.y-=max(0.,cos(time*2.+3.1415926/2.))*.1;
    d=smin(d,distance(VEC3(.3,clamp(p4.y,-2.6,-2.),0.),VEC3(p4.x,p4.y,p4.z))-.3,.1);
    p4=op;
    p4.y-=ld;
		ttt = Mat2xVec2(rotmat(cos(time*2.+3.1415926/1.*1.)*.1),VEC2(p4.y,p4.z));
    p4.y = ttt.u;
		p4.z = ttt.v;
    p4.y+=ld;
    p4.y-=max(0.,cos(time*2.+3.1415926/2.+3.1415926/1.*1.))*.1;
    d=smin(d,distance(VEC3(.3,clamp(p4.y,-2.6,-2.),0.),vec3xvec3(VEC3(p4.x,p4.y,p4.z),VEC3(-1,1,1)))-.3,.1);   
    
    // Belly
    d=smin(d,distance(VEC3(0.,-1.5,-.2),p)-.5+cos(time*3.)*.03,.4);   

    // Ears 2
    d=smin(d,distance(VEC3(1.1,2.3,-.1),p3)-.2,.8);   

    // Tail
    d=smin(d,distance(VEC3(0,-1.7,.6),p)-.3,.1);   

    vec3 q=VEC3(0.35,.4,-1);

    if(fmod(time-1.,4.)>.04)
    {
        d=smax(d,-(cylinder(vec3subv(p2,q),normalize(vec3subv(q,p2)),.3,.1)-.0001),.05);
        d=smin(d,(length( vec3subv(p2 , vec3multi(q,.9)))-.2),.24);

        // Eye pupils
        if(op.x>0.)
            pupdist=(length( vec3subv(p2,VEC3(.39,.32,-1.)))-.2);
        else
            pupdist=(length( vec3subv(p2,VEC3(.28,.32,-1.02)))-.2);

        d=smin(d,pupdist,.005);
    }

    // Nose
    d=smin(d,(length( vec3subv(p2,VEC3(0,.1,-1.02)))-.2),.02);

    // Mouth (two states)
    float d3=smax(-(length( vec3subv(p,VEC3(-.05,-.29,-1.02) ))-.1),-(length( vec3subv(p,VEC3(.05,-.29,-1.02)) ) -.1),.1);

    float d2=max(p2.z,distance(p2,VEC3(clamp(p2.x,0.,.3),-.2,clamp(p2.z,-2.,2.)))+.01);

    float time4=time/8.;
    float gg=smoothstep(0.,1.,clamp((min(fractf(time4),1.-fractf(time4))-.25)*64.,0.,1.));
    d=smax(d,mix(-d2,d3,gg),.1);

    // Tooth
    d=min(d,(length( vec3subv(p,VEC3(.0,-.2,-1.02)) )-.08));

    p.y+=.2;
		ttt = Mat2xVec2(rotmat(.4+cos(time2*2.)*.02),VEC2(p.x,p.y));
    p.x = ttt.u;
		p.y = ttt.v;
    // Arms
    float armd=smin(distance(VEC3(.2,clamp(p.y,-1.8,-0.),0.),VEC3(p.x,p.y,p.z))-.2,
                    distance(p,VEC3(0.2,-1.7,0))-.2,.2);

    d=smin(d,armd,.05);   

    return d;
}

float floordist(vec3 p)
{
    return p.y+2.85;
}

float f(vec3 p)
{
    return min(rabdist(p),floordist(p));
}

float sceneDist(vec3 p) { return f(p); }

vec3 sceneNorm(vec3 p)
{
    vec3 e=VEC3(1e-2,0,0);
    float d = sceneDist(p);
    return normalize(VEC3(sceneDist( vec3addv(p , VEC3(e.x,e.y,e.y))) - d, sceneDist(vec3addv(p , VEC3(e.y,e.x,e.y))) - d,
                          sceneDist(vec3addv(p , VEC3(e.y,e.y,e.x))) - d));
}

// From simon green and others
float ambientOcclusion(vec3 p, vec3 n)
{
    const int steps = 8;
    const float delta = 1.5;

    float a = 0.0;
    float weight = .5;
    for(int i=1; i<=steps; i++) {
        float d = ((float)(i) / (float)(steps)) * delta; 
        a += weight*(d - sceneDist( vec3addv(p , vec3multi(n, d) )));
        weight *= 0.6;
    }
    return clamp(1.0 - a, 0.0, 1.0);
}

uint32_t mainImage_DrawBunny(vec2 fragCoord)
{
		
    vec2 uv = VEC2((fragCoord.u/WIDTH * 2. - 1.) * .75,(fragCoord.v/HEIGHT * 2. - 1.) * .75);
    uv.u *= WIDTH / HEIGHT;
    vec2 vt=uv;
    float an;
    time=20.0;

    vec3 ro=VEC3(0.,-.1,-8.+0.5);
    vec3 rd=normalize(VEC3(uv.u,uv.v,1.8));


    float s=20.;

    // Raymarching
    float t=4.,d=0.;
    for(int i=0;i<100;++i)
    {
        d=f(  vec3addv(ro , vec3multi(rd ,t )) );
        if(d<1e-4)break;
        if(t>10.)break;
        t+=d;
    }

    // Directional light
		vec3 dl =  vec3addv( vec3addv (ro, vec3multi(rd,t)) , vec3multi(normalize(VEC3(1,2,-2)),5e-2)) ;
    float d2=f( dl );
    float l=.5+.5*(d2-d)/5e-2;

    vec3 rp=(vec3addv(ro , vec3multi(rd ,t )));

    vec3 n=sceneNorm(rp);
    
    vec3 bcol=VEC3(1,.4,.18);
    vec3 col=bcol;

    if(t<10.)
    {
				float f = mix(.1,1.,smoothstep(0.0,.01,pupdist));
        col=VEC3(f,f,f);

        if(floordist(rp)<rabdist(rp))
            col=vec3max(mixv3( vec3add(bcol ,(.1-lengthv2(VEC2(vt.u,vt.v))/3.)),VEC3(1.,1.,1.),.1),VEC3(0.,0.,0.));

        col = vec3multi(col,l);

        // Some fake bounce / environment lighting
        col = vec3addv(col,vec3multi(bcol,pow(clamp(-n.y,0.,1.),2.)/1.5));
        if(n.y<.9999) col = vec3addv( col,vec3multi (VEC3(1,.4,.18) ,pow(clamp(-rp.y-1.8,0.,1.),4.)/3.) );

        if(n.y>.99999)
        {
            // Floor shading
            col= vec3multi( col ,pow(ambientOcclusion(rp,n),1.) );
            col = vec3multi(col ,mix(.7,1.,smoothstep(0.,2.,lengthv2( VEC2(rp.x,rp.z) ))));
        }
        else
        {
            // Rabbit body shading
            vec3 r= reflect(rd,n);
            col = vec3add( col , step(0.5,r.y)*.2*pow(clamp(0.,1.,1.-dot(vec3multi(rd,-1.0f),n)),2.) );
            col = vec3multi( col ,pow(ambientOcclusion(rp,n),2.) );
        }
    }
    else
    	col= vec3multi(vec3max( mixv3( vec3add(bcol,(.1-lengthv2( VEC2(vt.u,vt.v))/3.) ),VEC3(1.,1.,1.),.1 ), VEC3(0.,0.0,0.0)) ,0.84);
    vec3 rgb;
    rgb= vec3max(col,VEC3(0.,0.0,0.0));
    rgb= vec3add(rgb ,.01 );
		rgb.x = sqrtf(rgb.x);
		rgb.y = sqrtf(rgb.y);
		rgb.z = sqrtf(rgb.z);
		rgb = vec3min(rgb,VEC3(1.0,1.0,1.0));
		return SetRGB24((uint32_t)(rgb.x * 255),(uint32_t)(rgb.y * 255),(uint32_t)(rgb.z * 255));
}

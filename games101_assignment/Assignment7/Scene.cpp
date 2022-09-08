//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){  //找到光源
            emit_area_sum += objects[k]->getArea();
        }
    }

    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::shade(Material & m,Vector3f & N,Vector3f &p,Vector3f& wo) const{
    Intersection inter;
    Vector3f ldir {0,0,0},lindir {0,0,0};
    float pdf_light = 0.f;
    //随机选择一个方向 wi
    Vector3f wi;
    Vector3f ws;
    Vector3f emit;
    Vector3f NN;  //光源击中位置的法向量。
    Vector3f x;  //光源击中位置.
    sampleLight(inter,pdf_light);
    N = N.normalized();
    NN = inter.normal.normalized();
    x = inter.coords;  //光源位置
    float ws_length = (p-x).norm();
    ws = (x-p).normalized();
    Ray r(p,ws);//判断能否被光源照到
    Intersection in = intersect(r);
    if(in.distance >= (ws_length - 0.0001)){
        emit = inter.emit;
        ldir = emit * m.eval(wo,r.direction,N) * dotProduct(NN,-r.direction) * dotProduct(N,r.direction) / (std::pow(ws_length,2)) / pdf_light;
    }//该表面被光源照亮，ldir返回由Kd决定的颜色。

    //P_RR 决定光线是否继续反射。如果不通过则直接返回该着色点的直接光照。
    float rr = get_random_float();
    if(rr > RussianRoulette){
        return ldir;
    }
    wi = m.sample(wo,N).normalized();  //这时候wi就是着色点接收到其他物体反光的反方向。
    //再次寻找相交物体。判断相交项是否是不发光物体。
    inter = intersect(Ray(p,wi));
    Material next_m;
    if(inter.happened){
        next_m = *inter.m;
        Object * o = inter.obj;
        if(!o->hasEmit()){
            lindir = shade(next_m,inter.normal,inter.coords,wi) * m.eval(wo,wi,N) * dotProduct(wi,N) /m.pdf(wo,wi,N) /RussianRoulette;        
        }
    }
    return ldir + lindir;
}
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    if(depth > Scene::maxDepth){
        return backgroundColor;
    }
    Intersection intersection = intersect(ray);
    Material m;
    Object * hitObject = intersection.obj;
    Vector3f N = intersection.normal;
    Vector3f dir = ray.direction;
    Vector3f wo = dir;
    if(intersection.happened){
        m = *intersection.m;
        if(intersection.m->hasEmission())
        {
            return intersection.m->m_emission;
        }
        return shade(m,N,intersection.coords,wo);
    }
    return backgroundColor;
}

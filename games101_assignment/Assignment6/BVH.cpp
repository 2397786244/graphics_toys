#include <algorithm>
#include <cassert>
#include "BVH.hpp"
#include "vector"

BVHAccel::BVHAccel(std::vector<Object*> p, int maxPrimsInNode,
                   SplitMethod splitMethod)
    : maxPrimsInNode(std::min(255, maxPrimsInNode)), splitMethod(splitMethod),
      primitives(std::move(p))
{
    time_t start, stop;
    time(&start);
    if (primitives.empty())
        return;
    if(splitMethod == SplitMethod::NAIVE){
        printf("Choose NAIVE Mode\n");
        root = recursiveBuild(primitives);
    }
    else if(splitMethod == SplitMethod::SAH){
        printf("Choose SAH Mode\n");
        root = recursiveBuild_SAH(primitives);
    }
    time(&stop);
    double diff = difftime(stop, start);
    int hrs = (int)diff / 3600;
    int mins = ((int)diff / 60) - (hrs * 60);
    int secs = (int)diff - (hrs * 3600) - (mins * 60);

    printf(
        "\rBVH Generation complete: \nTime Taken: %i hrs, %i mins, %i secs\n\n",
        hrs, mins, secs);
}

//SAH优化
BVHBuildNode* BVHAccel::recursiveBuild_SAH(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;

    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild_SAH(std::vector{objects[0]});
        node->right = recursiveBuild_SAH(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        //size >= 3
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        //根据分割轴对各物体进行排序。
        
        int dim = centroidBounds.maxExtent();
        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        std::vector<Object*>::iterator beginning = objects.begin();
        std::vector<Object*>::iterator middling;
        std::vector<Object*>::iterator ending = objects.end();;

        Bounds3 left,right;
        float l_s,r_s,total_s;
        std::vector<float> best_time;
        std::vector<Object*> left_s,right_s;
        total_s = bounds.SurfaceArea();
        for(int i = 1;i<objects.size()-1;i++)
        {
            middling = beginning + i;
            left_s = std::vector<Object*>(beginning,middling);
            right_s = std::vector<Object*>(middling,ending);
            assert(objects.size() == (left_s.size() + right_s.size()));
            for(int i = 0;i<left_s.size();i++){
                left = Union(left,left_s[i]->getBounds());
            }            
            for(int i = 0;i<right_s.size();i++){
                right = Union(right,right_s[i]->getBounds());
            }

            l_s = left.SurfaceArea();
            r_s = right.SurfaceArea();
            float cost = l_s / total_s * left_s.size() + r_s / total_s * right_s.size();
            best_time.push_back(cost);
        }

        beginning = objects.begin();
        ending = objects.end();

        auto min_iterator = std::min_element(best_time.begin(),best_time.end());
        int ind = min_iterator - best_time.begin();
        middling = objects.begin() + ind + 1;

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild_SAH(leftshapes);
        node->right = recursiveBuild_SAH(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}


BVHBuildNode* BVHAccel::recursiveBuild(std::vector<Object*> objects)
{
    BVHBuildNode* node = new BVHBuildNode();

    // Compute bounds of all primitives in BVH node
    Bounds3 bounds;
    for (int i = 0; i < objects.size(); ++i)
        bounds = Union(bounds, objects[i]->getBounds());
    if (objects.size() == 1) {
        // Create leaf _BVHBuildNode_
        node->bounds = objects[0]->getBounds();
        node->object = objects[0];
        node->left = nullptr;
        node->right = nullptr;
        return node;
    }
    else if (objects.size() == 2) {
        node->left = recursiveBuild(std::vector{objects[0]});
        node->right = recursiveBuild(std::vector{objects[1]});

        node->bounds = Union(node->left->bounds, node->right->bounds);
        return node;
    }
    else {
        Bounds3 centroidBounds;
        for (int i = 0; i < objects.size(); ++i)
            centroidBounds =
                Union(centroidBounds, objects[i]->getBounds().Centroid());
        int dim = centroidBounds.maxExtent();
        switch (dim) {
        case 0:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().x <
                       f2->getBounds().Centroid().x;
            });
            break;
        case 1:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().y <
                       f2->getBounds().Centroid().y;
            });
            break;
        case 2:
            std::sort(objects.begin(), objects.end(), [](auto f1, auto f2) {
                return f1->getBounds().Centroid().z <
                       f2->getBounds().Centroid().z;
            });
            break;
        }

        auto beginning = objects.begin();
        auto middling = objects.begin() + (objects.size() / 2);
        auto ending = objects.end();

        auto leftshapes = std::vector<Object*>(beginning, middling);
        auto rightshapes = std::vector<Object*>(middling, ending);

        assert(objects.size() == (leftshapes.size() + rightshapes.size()));

        node->left = recursiveBuild(leftshapes);
        node->right = recursiveBuild(rightshapes);

        node->bounds = Union(node->left->bounds, node->right->bounds);
    }

    return node;
}

Intersection BVHAccel::Intersect(const Ray& ray) const
{
    Intersection isect;
    if (!root)
        return isect;
    isect = BVHAccel::getIntersection(root, ray);
    return isect;
}

Intersection BVHAccel::getIntersection(BVHBuildNode* node, const Ray& ray) const
{
    // Traverse the BVH to find intersection
    Intersection inter;
    static std::array<int,3> dirIsNeg = {int(ray.direction.x>0),int(ray.direction.y>0),int(ray.direction.z>0)};
    bool status = node->bounds.IntersectP(ray,ray.direction_inv,dirIsNeg);
    if(status == false){
        return inter;
    }
    if(node->left && node->right){
        //中间结点.其左右子结点不为空，则判断能否跟左/右边得包围盒相交，且返回t最近的那个
        auto hit1 = getIntersection(node->left,ray);
        auto hit2 = getIntersection(node->right,ray);
        inter = (hit1.distance < hit2.distance) ? hit1 : hit2;
    }
    else
    { //叶子节点
        inter = node->object->getIntersection(ray);
    }
    return inter;
}
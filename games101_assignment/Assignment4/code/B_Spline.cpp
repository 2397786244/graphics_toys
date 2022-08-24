#include <opencv2/opencv.hpp>
#include "iostream"
#include "vector"
std::vector<float> knots;

//递归计算基函数Ni。k为阶数。 k=1的所有基函数值为1。定义0/0 = 0.
static float recursiveGetNi(float t,int i,int k,int n){
    //static std::vector<float > knots = {1.f/7.f,2.f/7.f,3.f/7.f,4.f/7.f,5.f/7.f,6.f/7.f,1.f};  //4个控制点(n=3) 3阶。 均匀B样条。
    float Nik1,Ni1k1;
    float ik1_d1 = t - knots.at(i);  // t - t_i
    float ik1_d2 = knots.at(i+k-1) - knots.at(i);  // t_i+k-1 - t_i
    //float ik1 = (ik1_d1== 0.f || ik1_d2 == 0.f) ? 0.f : ik1_d1 / ik1_d2;
    float ik1 = ik1_d1 / ik1_d2;
    float ik2_d1 = knots.at(i+k) - t;  // t_i+k - t
    float ik2_d2 = knots.at(i+k)-knots.at(i+1);  // t_i+k - t_i+1
    //float ik2 = (ik2_d1== 0.f || ik2_d2 == 0.f) ? 0.f : ik2_d1 / ik2_d2;
    float ik2 = ik2_d1 / ik2_d2;
    if(k == 1){
        //阶数等于1，t在 t_i 到 t_i+1 范围内，Ni = 1，否则为0.
        if(t >= knots.at(i) && t <= knots.at(i+1)){
            return 1.f;
        }
        else{
            return 0.f;
        }
    }
    else
    {
        Nik1 = recursiveGetNi(t,i,k-1,n);
        Ni1k1 = recursiveGetNi(t,i+1,k-1,n);
        return ik1 * Nik1 + ik2 * Ni1k1;  //例如 控制点为4，n=3，3阶，N_0_3 就需要计算N_0_2(需要计算N_0_1、N_1_1。都在结点向量中)、N_1_2(需要计算N_1_1、N_2_1)
    }
}

//n = cpoints.size - 1
static cv::Point2f calcEachSnip(float t,int k,const std::vector<cv::Point2f> & cpoints){
    cv::Point2f p {0.f,0.f};
    float Ni;
    for(int i = 0;i<cpoints.size();i++)
    {
        Ni = recursiveGetNi(t,i,k,cpoints.size()-1);
        p += cpoints[i] * Ni;  //cp_0 * Ni_0_3 + cp_1 * Ni_1_3 + ...
    }
    return p;
}

// 传入控制点序列和阶数以及屏幕矩阵。
void drawBSpline(const std::vector<cv::Point2f> & cpoints,cv::Mat & window,int k){
    cv::Point2f p;
    k = 3;
    //knots长度为 控制点数+k。
    for(int i = 0;i<cpoints.size() + k;i++){
        knots.push_back(float(i));
    }
    for(float t = k-1;t<= cpoints.size(); t+=0.001f){
        p = calcEachSnip(t,k,cpoints);
        window.at<cv::Vec3b>(p.y,p.x)[1] = 255;
    }
}
#include <Eigen/Core>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#define aspect_ratio 2.5
#define eye_fov 70
#define zNear 20.0
#define zFar 50.0

int main(){
    Eigen::Matrix4f mat_o;
    Eigen::Matrix4f mat_t;

    mat_o << 1 / (aspect_ratio * std::tan(eye_fov/2)*zNear),0,0,0,
            0,1/(zNear * std::tan(eye_fov/2)),0,0,
            0,0,2/(zNear - zFar),-(zNear+zFar)/(zNear-zFar),
            0,0,0,1;
    mat_t << zNear ,0,0,0,
            0,zNear,0,0,
            0,0,zNear + zFar,-zNear * zFar,
            0,0,1,0;
    Eigen::Matrix4f out;
    out = mat_t * mat_o;
    std::cout << out << std::endl;
    std::cout << "output_2" << std::endl;
    std::cout << mat_o * mat_t;
    return 0;
}

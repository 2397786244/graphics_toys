#include <Eigen/Eigen>
#include <iostream>
using namespace Eigen;
int main(){
    Vector3f dv0(2,3,0);
    Vector3f v1v0(1,2,0);
    std::cout << v1v0.cross(dv0);
    std::cout << std::endl << dv0.cross(v1v0);
}
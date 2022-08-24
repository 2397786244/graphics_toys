#ifndef BEZIERCURVE_BSPLINE_H
#define BEZIERCURVE_BSPLINE_H
#include <opencv2/opencv.hpp>
#include "iostream"
#include "vector"
void drawBSpline(const std::vector<cv::Point2f> & cpoints,cv::Mat & window,int k=3);
#endif //BEZIERCURVE_BSPLINE_H

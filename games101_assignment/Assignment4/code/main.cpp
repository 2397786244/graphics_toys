#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "bspline.h"
std::vector<cv::Point2f> control_points;

void mouse_handler(int event, int x, int y, int flags, void *userdata) 
{
    if (event == cv::EVENT_LBUTTONDOWN && control_points.size() < 4) 
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
        << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }     
}

void BSpline_mouseHandler(int event, int x, int y, int flags, void *userdata){
    if (event == cv::EVENT_LBUTTONDOWN)
    {
        std::cout << "Left button of the mouse is clicked - position (" << x << ", "
                  << y << ")" << '\n';
        control_points.emplace_back(x, y);
    }
}
void naive_bezier(const std::vector<cv::Point2f> &points, cv::Mat &window) 
{
    auto &p_0 = points[0];
    auto &p_1 = points[1];
    auto &p_2 = points[2];
    auto &p_3 = points[3];

    for (double t = 0.0; t <= 1.0; t += 0.001) 
    {
        auto point = std::pow(1 - t, 3) * p_0 + 3 * t * std::pow(1 - t, 2) * p_1 +
                 3 * std::pow(t, 2) * (1 - t) * p_2 + std::pow(t, 3) * p_3;

        window.at<cv::Vec3b>(point.y, point.x)[2] = 255;
    }
}

cv::Point2f recursive_bezier(const std::vector<cv::Point2f> &control_points, float t) 
{
    size_t cpoint_cnt = control_points.size();
    std::vector<cv::Point2f> cp_next;
    cv::Point2f c0,c1,cnew;
    if(cpoint_cnt == 1)
    {
        return control_points.at(0);
    }
    else
    {
        for (size_t i = 0; i < cpoint_cnt - 1; i++) {
            c0 = control_points.at(i);
            c1 = control_points.at(i + 1);
            cnew = (1-t) * c0 + t * c1;
            cp_next.push_back(cnew);
        }
        return recursive_bezier(cp_next,t);
    }
}

void bezier(const std::vector<cv::Point2f> &control_points, cv::Mat &window) 
{
    for(float t =0.0f;t<=1.0;t+=0.001f){
        auto point = recursive_bezier(control_points,t);
        window.at<cv::Vec3b>(point.y,point.x)[1] = 255;
    }
}

//曲线反走样。
void aa(const std::vector<cv::Point2f> & cpoints,cv::Mat & w){
    int bb_min_x,bb_min_y,bb_max_x,bb_max_y;
    int sum = 0;
    bb_min_x = std::min({cpoints.at(0).x,cpoints.at(1).x,cpoints.at(2).x,cpoints.at(3).x});
    bb_max_x = std::max({cpoints.at(0).x,cpoints.at(1).x,cpoints.at(2).x,cpoints.at(3).x});
    bb_min_y = std::min({cpoints.at(0).y,cpoints.at(1).y,cpoints.at(2).y,cpoints.at(3).y});
    bb_max_y = std::max({cpoints.at(0).y,cpoints.at(1).y,cpoints.at(2).y,cpoints.at(3).y});
    for(int x = bb_min_x+1;x<bb_max_x-1;x+=1){
        for(int y = bb_min_y+1; y < bb_max_y -1; y+=1){
            if(w.at<cv::Vec3b>(y,x)[1] == 0){
                sum += w.at<cv::Vec3b>(y-1,x)[1] + w.at<cv::Vec3b>(y+1,x)[1]+  w.at<cv::Vec3b>(y,x-1)[1]
                        +w.at<cv::Vec3b>(y,x+1)[1] + w.at<cv::Vec3b>(y+1,x+1)[1] + w.at<cv::Vec3b>(y-1,x-1)[1] +w.at<cv::Vec3b>(y-1,x+1)[1]
                                + w.at<cv::Vec3b>(y+1,x-1)[1];
                w.at<cv::Vec3b>(y,x)[1] = sum / 8;
                sum = 0;
            }
        }
    }
}

int main() 
{
    cv::Mat window = cv::Mat(700, 700, CV_8UC3, cv::Scalar(0));
    cv::cvtColor(window, window, cv::COLOR_BGR2RGB);
    cv::namedWindow("BSpline", cv::WINDOW_AUTOSIZE);

    //cv::setMouseCallback("Bezier Curve", mouse_handler, nullptr);
    cv::setMouseCallback("BSpline",BSpline_mouseHandler, nullptr);
    int key = -1;
    while (key != 27) 
    {
        for (auto &point : control_points) 
        {
            cv::circle(window, point, 3, {255, 255, 255}, 3);
        }
        //Bezier Code
        /*
        if (control_points.size() == 4) 
        {
            //naive_bezier(control_points, window);
            bezier(control_points, window);

            // 检查window矩阵中在贝塞尔曲线凸包内的像素点。判断与曲线的距离决定该像素的颜色。
            //aa(control_points,window);
            cv::imshow("Bezier Curve", window);
            cv::imwrite("my_bezier_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }
        */
        //BSpline  有5个控制点
        if(control_points.size() == 5)
        {
            drawBSpline(control_points, window);
            cv::imshow("BSpline", window);
            cv::imwrite("my_bspline_curve.png", window);
            key = cv::waitKey(0);

            return 0;
        }
        cv::imshow("BSpline", window);
        key = cv::waitKey(20);
    }

return 0;
}

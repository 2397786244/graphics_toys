// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}

//返回覆盖样本的数量
static std::vector<Vector3f> insideTriangle(int x, int y, const Vector3f* _v)
{   
    //点D(x,y)在三角形内，那么DV_0 x V_1V_0 < 0,DV_1 x V2V1 < 0,DV_2 x V_0V_2 <0
    Vector3f v1v0 = _v[1]-_v[0],v2v1= _v[2] - _v[1] ,v0v2= _v[0] - _v[2];
    Vector3f sampleList[4] {Vector3f(x + 0.25,y+0.25,1),Vector3f(x+0.75,y+0.25,1),Vector3f(x+0.25,y+0.75,1),Vector3f(x+0.75,y+0.75,1)};
    //对sampleList内的四个采样点逐个判断是否在三角形内。返回三角形覆盖的样本数。
    Vector3f d0,d1,d2;
    std::vector<Vector3f> coverVector;
    float z_0,z_1,z_2;
    int index = 0;
    for(auto & i : sampleList){
        d0 = i - _v[0];
        d1 = i - _v[1];
        d2 = i - _v[2];
        z_0 = v1v0.cross(d0)[2];
        z_1 = v2v1.cross(d1)[2];
        z_2 = v0v2.cross(d2)[2];
        if((z_1 <0 && z_2 <0 && z_0 <0 ) || (z_0 > 0 && z_1 >0 && z_2 > 0)){
            coverVector.push_back(Vector3f(i[0],i[1],index));
        }
        index++;
    }
    return coverVector;

}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;

    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);

        rasterize_triangle(t);
    }
}

//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t) {
    auto v = t.toVector4();
    int x_min = std::min({v[0][0],v[1][0],v[2][0]}) + 0.5;
    int x_max = std::max({v[0][0],v[1][0],v[2][0]}) + 0.5;
    int y_min = std::min({v[0][1],v[1][1],v[2][1]}) + 0.5;
    int y_max = std::max({v[0][1],v[1][1],v[2][1]}) + 0.5;
    // iterate through the pixel and find if the current pixel is inside the triangle
    //check pixel  from y_min to y_max
    // and from x_min to x_max
    //三角形三个顶点的坐标数组。
    Vector3f TriangleColor = t.getColor();
    std::vector<Vector3f > coverCount;
    for(int x = x_min;x<x_max;x++)
    {
        for(int y=y_min;y<y_max;y++)
        {
            if(!(coverCount = insideTriangle(x,y,t.v)).empty())
            {
                //像素点在三角形内，从考虑其点的z值是否离屏幕更近。
                //从coverCount中取出三角形内的采样点。
                float z_min = FLT_MAX;
                for(auto each:coverCount)
                {
                    auto [alpha, beta, gamma] = computeBarycentric2D(each[0], each[1], t.v);
                    float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                    float z_interpolated =
                            alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                    z_interpolated *= w_reciprocal;
                    z_min = (z_min > z_interpolated) ?  z_interpolated : z_min;

                }
                    //深度测试
                    if (depth_buf[get_index(x,y)] > z_min) {
                        //离相机更近，那么着色
                        set_pixel(Eigen::Vector3f(x,y, z_min), float(coverCount.size()) / 4 * TriangleColor);
                        depth_buf[get_index(x,y)] = z_min;  //更新深度缓冲区
                    }

            }
        }
    }

}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::infinity());
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();
    frame_buf[ind] = color;

}

// clang-format on
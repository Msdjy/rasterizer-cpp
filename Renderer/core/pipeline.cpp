#pragma once
#include "./pipeline.h"

#include <thread>
#include <mutex>
#include <cmath>
std::mutex mutex_ins;


// framebuffer以左下角为原点
// 屏幕空间坐标以左上角为原点
// TODO staic作用
static void set_color(unsigned char* framebuffer, int x, int y, unsigned char color[]) {
	int index = ((WINDOW_HEIGHT - y - 1) * WINDOW_WIDTH + x) * 4;

	for (int i = 0; i < 4; i++) {
		framebuffer[index + i] = color[i];
	}
}

static void set_color(unsigned char* framebuffer, int x, int y, vec4 color) {
    // color
    unsigned char c[4];
    for (int t = 0; t < 4; t++)
    {
        c[t] = (int)float_clamp(color[t] * 255, 0, 255);
    }

    int index = ((WINDOW_HEIGHT - y - 1) * WINDOW_WIDTH + x) * 4;
    for (int i = 0; i < 4; i++) {
        framebuffer[index + i] = c[i];
    }
}

vec4 get_color(unsigned char* framebuffer, int x, int y) {
    // color
    vec4 color;

    int index = ((WINDOW_HEIGHT - y - 1) * WINDOW_WIDTH + x) * 4;
    for (int i = 0; i < 4; i++) {
        color[i] = (float)(int)framebuffer[index + i] / 255.0f;
    }
    return color;
}

static vec4 HDR_ReinhardMap(vec4 color) {

    const float gamma = 2.2;
    vec4 hdrColor = color;

    //// Reinhard色调映射
    vec4 mapped = hdrColor;
    for (int w = 0; w < 3; w++) {
        mapped[w] = hdrColor[w] / (hdrColor[w] + 1.0f);
        mapped[w] = std::pow(mapped[w], 1.0 / gamma);
    }
    return mapped;
}

int get_index(int x, int y) {
    return ((WINDOW_HEIGHT - y - 1) * WINDOW_WIDTH + x);
}



static bool is_back_pace(vec3 pos[]) {
    //vec3 ab = pos[1] - pos[0];
    //vec3 bc = pos[2] - pos[1];
    //return (cross(ab, bc)).norm() > 0.0f;
    //TODO
    vec3 a = pos[0];
    vec3 b = pos[1];
    vec3 c = pos[2];
    float signed_area = a.x() * b.y() - a.y() * b.x() +
        b.x() * c.y() - b.y() * c.x() +
        c.x() * a.y() - c.y() * a.x();   //|AB AC|
    return signed_area <= 0;
}

static bool is_front_pace(vec3 pos[]) {
    //vec3 ab = pos[1] - pos[0];
    //vec3 bc = pos[2] - pos[1];
    //return (cross(ab, bc)).norm() > 0.0f;
    //TODO
    vec3 a = pos[2];
    vec3 b = pos[1];
    vec3 c = pos[0];
    float signed_area = a.x() * b.y() - a.y() * b.x() +
        b.x() * c.y() - b.y() * c.x() +
        c.x() * a.y() - c.y() * a.x();   //|AB AC|
    return signed_area <= 0;
}

// TODO 
//static void line(unsigned char* framebuffer, vec2 p0, vec2 p1, unsigned char color[]) {
//    bool steep = false;
//    if (std::abs(p0.x() - p1.x()) < std::abs(p0.y() - p1.y())) {
//        std::swap(p0[0], p0[1]);
//        std::swap(p1[0], p1[1]);
//        steep = true;
//    }
//    if (p0.x() > p1.x()) {
//        std::swap(p0, p1);
//    }
//
//    for (int x = p0.x(); x <= p1.x(); x++) {
//        float t = (x - p0.x()) / (float)(p1.x() - p0.x());
//        int y = p0.y() * (1. - t) + p1.y() * t;
//        if (steep) {
//            set_color(framebuffer, y, x, color);
//        }
//        else {
//            set_color(framebuffer, x, y, color);
//        }
//    }
//}
//
//
//static void triangle(unsigned char* framebuffer, vec3 vertexs[], unsigned char color[]) {
//    line(framebuffer, vertexs[0], vertexs[1], color);
//    line(framebuffer, vertexs[1], vertexs[2], color);
//    line(framebuffer, vertexs[2], vertexs[0], color);
//}


static std::tuple<float, float, float> compute_barycentric2D(float x, float y, vec3 vertexs[])
{
    float c1 = (x * (vertexs[1].y() - vertexs[2].y()) + (vertexs[2].x() - vertexs[1].x()) * y + vertexs[1].x() * vertexs[2].y() - vertexs[2].x() * vertexs[1].y()) / (vertexs[0].x() * (vertexs[1].y() - vertexs[2].y()) + (vertexs[2].x() - vertexs[1].x()) * vertexs[0].y() + vertexs[1].x() * vertexs[2].y() - vertexs[2].x() * vertexs[1].y());
    float c2 = (x * (vertexs[2].y() - vertexs[0].y()) + (vertexs[0].x() - vertexs[2].x()) * y + vertexs[2].x() * vertexs[0].y() - vertexs[0].x() * vertexs[2].y()) / (vertexs[1].x() * (vertexs[2].y() - vertexs[0].y()) + (vertexs[0].x() - vertexs[2].x()) * vertexs[1].y() + vertexs[2].x() * vertexs[0].y() - vertexs[0].x() * vertexs[2].y());
    return { c1, c2, 1 - c1 - c2 };
}


static int is_inside_triangle(float alpha, float beta, float gamma)
{
    int flag = 0;
    // here epsilon is to alleviate precision bug
    if (alpha > -EPSILON && beta > -EPSILON && gamma > -EPSILON)
        flag = 1;

    return flag;
}


//static bool insideTriangle(float u, float v, vec2 vertexs[0], vec2 vertexs[1], vec2 vertexs[2]) {
//    vec3 v[3] = { vec3(vertexs[0], 1.0f), vertexs[1], vertexs[2] };
//    vec3 p(u, v, 1.0f)
//
//    f0 = cross();
//    f1 = v[2].cross(v[1]);
//    f2 = v[0].cross(v[2]);
//    Vector3f p(x, y, 1.);
//    if ((p.dot(f0) * f0.dot(v[2]) > 0) && (p.dot(f1) * f1.dot(v[0]) > 0) && (p.dot(f2) * f2.dot(v[1]) > 0))
//        return true;
//    return false;
//}

static std::tuple<int, int, int, int> get_AABB(vec3 screen_vertex3[])
{
    auto minx = std::min(screen_vertex3[0].x(), std::min(screen_vertex3[1].x(), screen_vertex3[2].x()));
    auto maxx = std::max(screen_vertex3[0].x(), std::max(screen_vertex3[1].x(), screen_vertex3[2].x()));

    auto miny = std::min(screen_vertex3[0].y(), std::min(screen_vertex3[1].y(), screen_vertex3[2].y()));
    auto maxy = std::max(screen_vertex3[0].y(), std::max(screen_vertex3[1].y(), screen_vertex3[2].y()));

    minx = (int)std::floor(minx); // 对x进行向下取整
    maxx = (int)std::ceil(maxx); // 对y进行向上取整
    miny = (int)std::floor(miny); // 对x进行向下取整
    maxy = (int)std::ceil(maxy); // 对y进行向上取整

    return {minx, maxx, miny, maxy};
}
vec3 interpolate(float alpha, float beta, float gamma, const vec3(&vert3)[], const vec4(&mvp_vertex3)[], float weight)
{
    return (alpha * vert3[0] / mvp_vertex3[0].w() + beta * vert3[1] / mvp_vertex3[1].w() + gamma * vert3[2] / mvp_vertex3[2].w() ) * weight;
    //return (alpha * vert3[0] + beta * vert3[1] + gamma * vert3[2]  ) * weight;
}

vec4 interpolate(float alpha, float beta, float gamma, const vec4(&vert3)[], const vec4(&mvp_vertex3)[], float weight)
{
    return (alpha * vert3[0] / mvp_vertex3[0].w() + beta * vert3[1] / mvp_vertex3[1].w() + gamma * vert3[2] / mvp_vertex3[2].w()) * weight;
}



// 由投影矩阵创建的观察箱(Viewing Box)被称为平截头体(Frustum)，每个出现在平截头体范围内的坐标都会最终出现在用户的屏幕上。
// 将特定范围内的坐标转化到标准化设备坐标系的过程（而且它很容易被映射到2D观察空间坐标）被称之为投影(Projection)，因为使用投影矩阵能将3D坐标投影(Project)到很容易映射到2D的标准化设备坐标系中。

//                   mvp空间应该就是裁剪空间
// 一旦所有顶点被变换到裁剪空间，最终的操作——透视除法(Perspective Division)将会执行，在这个过程中我们将位置向量的x，y，z分量分别除以向量的齐次w分量；
// 透视除法是将4D裁剪空间坐标变换为3D标准化设备坐标的过程。这一步会在每一个顶点着色器运行的最后被自动执行。
// 在这一阶段之后，最终的坐标将会被映射到屏幕空间中（使用glViewport中的设定），并被变换成片段。


// 顶点着色器的输出要求所有的顶点都在裁剪空间内，这正是我们刚才使用变换矩阵所做的。
// OpenGL然后对裁剪坐标执行透视除法从而将它们变换到标准化设备坐标。
// OpenGL会使用glViewPort内部的参数来将标准化设备坐标映射到屏幕坐标，每个坐标都关联了一个屏幕上的点（在我们的例子中是一个800x600的屏幕）。这个过程称为视口变换。

// 光栅化
static void triangle_draw(unsigned char* framebuffer, float* zbuffer, IShader* shader, std::vector<vec3>& screen_vertexs) {

    int n = shader->attribute.vertexs.size();

    // 设置遍历三角形
    for (int id = 0; id < n; id = id + 3)
    {
        // 选取三角形三个点的不同坐标空间下的值NDC_vertexs3，screen_vertex3等
        vec3 screen_vertex3[3] = { screen_vertexs[id + 0], screen_vertexs[id + 1], screen_vertexs[id + 2] };

        // 背面剔除 / 渲染depthbuffer时正面剔除
        if (  (is_back_pace(screen_vertex3) && (!shader->is_shadow_shader) ) || (is_front_pace(screen_vertex3) && (shader->is_shadow_shader)) ) {
            continue;
        }


        vec4 position3[3]            = { shader->gl.positions[id + 0], shader->gl.positions[id + 1], shader->gl.positions[id + 2] };
        vec3 normal3[3]              = { shader->varying.normals[id + 0], shader->varying.normals[id + 1], shader->varying.normals[id + 2] };
        vec3 fragpose3[3]            = { shader->varying.fragposes[id + 0], shader->varying.fragposes[id + 1], shader->varying.fragposes[id + 2] };
        //vec4 position3_from_light[3] = { shader->varying.positions_from_light[id + 0], shader->varying.positions_from_light[id + 1], shader->varying.positions_from_light[id + 2] };


        // AABB BOX
        auto [minx, maxx, miny, maxy] = get_AABB(screen_vertex3);

        for (int i = minx; i <= maxx; i++) {
            for (int j = miny; j <= maxy; j++) {
                if (i < 0 || i >= WINDOW_WIDTH || j < 0 || j >= WINDOW_HEIGHT)continue;
                // 求屏幕空间中的重心坐标
                auto [alpha, beta, gamma] = compute_barycentric2D((float)(i + 0.5), (float)(j + 0.5), screen_vertex3);
                if (alpha > 0 && beta > 0 && gamma > 0) {
                    // interpolation correct term
                    // 修正用了真实空间的深度值，mvp空间的w值齐次坐标
                    float normalizer = 1.0 / (alpha / position3[0].w() + beta / position3[1].w() + gamma / position3[2].w());

                    //for larger z means away from camera, needs to interpolate z-value as a property 
                    shader->gl.FragCoord = alpha * screen_vertex3[0] + beta * screen_vertex3[1]  + gamma * screen_vertex3[2] ;
                    shader->gl.FragCoord[3] = 1 / normalizer;

                    float depth = shader->gl.FragCoord.z();
                    if (zbuffer[get_index(i, j)] > depth) {
                        zbuffer[get_index(i, j)] = depth;

                        // 插值法向量，世界空间坐标，uv
                        shader->varying.Normal = interpolate(alpha, beta, gamma, normal3, position3, normalizer);
                        shader->varying.FragPos = interpolate(alpha, beta, gamma, fragpose3, position3, normalizer);
                        //shader->varying.Position_From_Light = interpolate(alpha, beta, gamma, position3_from_light, position3, normalizer);

                        // 片段着色器一般需要的变量
                        //插值
                            //顶点法向量
                            //顶点vu值
                            //纹理
                        //光源
                        //相机位姿
                        vec4 color = shader->fragment_shader();

                        set_color(framebuffer, i, j, color);
                    }
                }

            }
        }
    }
}


void model_draw(unsigned char* framebuffer, float* zbuffer, IShader* shader) {

    int n = shader->attribute.vertexs.size();
    // gl_position是mvp空间的点
    shader->gl.positions.resize(n);
    shader->varying.normals.resize(n);
    shader->varying.fragposes.resize(n);
    //shader->varying.positions_from_light.resize(n);

    // 顶点着色器 mvp变换
    shader->vertex_shader();

    // 管线的中间变量
    std::vector<vec3> NDC_vertexs(n);
    std::vector<vec3> screen_vertexs(n);
    for (int i = 0; i < n; i++)
    {
        // TODO 裁剪
        // 裁切过程运行在每个三角形之上；这个过程可能生成新的顶点。
        // 在裁切坐标空间（clip coordinate space）运行线性插值来确定每个新顶点的裁切坐标和变异变量（varying variables）值。
        // 太过靠近物体有问题,透视除法除0问题


        // NDC transformation
        // homogeneous division
        NDC_vertexs[i] = to_vec3(shader->gl.positions[i] / shader->gl.positions[i].w());

        // 屏幕空间映射
        screen_vertexs[i][0] = (WINDOW_WIDTH - 1) * (0.5 * NDC_vertexs[i][0] + 0.5);
        screen_vertexs[i][1] = (WINDOW_HEIGHT - 1) * (0.5 * NDC_vertexs[i][1] + 0.5);

        screen_vertexs[i][2] = (NDC_vertexs[i][2] * 0.5 + 0.5);	//view space z-value
        // ndc的近平面在正，远平面在负，相对于深度值，远平面应该更大，1-屏幕空间的深度值，
        screen_vertexs[i][2] = 1 - screen_vertexs[i][2];
    }

    triangle_draw(framebuffer, zbuffer, shader, screen_vertexs);

}





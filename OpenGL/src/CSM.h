#pragma once
#include "Camera.h"
class Frustum {
private:
    float fov;
    float ratio;//宽高比
    float near;
    float far;
public:
    std::vector<glm::vec3> m_points = std::vector<glm::vec3>(8);//平截头体8个点
    Frustum();
    ~Frustum();
    void set(float t_fov, float t_ratio, float t_near, float t_far);
    float Getfov() const { return fov; }
    float Getratio() const { return ratio; }
    float Getnear() const { return near; }
    float Getfar() const { return far; }

    void Setnear(float t_near);
    void Setfar(float t_far);
    void Setfov(float t_fov);
    void Setratio(float t_ratio);
};
Frustum::Frustum(): fov(45.0f), ratio(1.0f), near(0.1f), far(100.0f)
{
}
Frustum::~Frustum()
{
}
void Frustum::Setnear(float t_near) {
    near = t_near;
}
void Frustum::Setfar(float t_far) {
    far = t_far;
}
void Frustum::Setfov(float t_fov) {
    fov = t_fov;
}
void Frustum::Setratio(float t_ratio) {
    ratio = t_ratio;
}
void Frustum::set(float t_fov, float t_ratio, float t_near, float t_far) {
    fov = t_fov;
    ratio = t_ratio;
    near = t_near;
    far = t_far;
}
class CSM_Dirlight {
private:
    int splitNum;//视锥分片数
    float lambda;//划分权重
    std::vector<Frustum> camera_frustums = std::vector<Frustum>(splitNum);
    std::vector<float> z_distance = std::vector<float>(splitNum);
    std::vector<float> xy_distance = std::vector<float>(splitNum);
public:
    CSM_Dirlight(Camera& camera, int splitnum, float weight);
    ~CSM_Dirlight();
    glm::mat4 lightView;//光空间view矩阵
    std::vector<glm::mat4> light_orth_matrix = std::vector<glm::mat4>(splitNum);//分片光空间变换矩阵(投影*观察)
    std::vector<glm::mat4> light_projection_matrix = std::vector<glm::mat4>(splitNum);//光空间投影矩阵
    //生成CSM所需的资源(分片光空间变换矩阵，眼空间到光空间变换矩阵)
    void split_camera_frustum(Camera& camera);
    float Get_frustum_far(int i)
    {
        return camera_frustums[i].Getfar();
    }
    //计算世界空间中的相机视锥切片边界点
    void camera_frustum_points(Camera& camera);
    //生成光视锥切片的光空间变换矩阵
    void light_projection();
    void Get_light_projection(Camera& camera, const glm::vec3& DirlightPosition);
    float Get_z_distance(int i) { return z_distance[i]; }
    float Get_xy_distance(int i) { return xy_distance[i]; }
};
CSM_Dirlight::CSM_Dirlight(Camera& camera, int splitnum = 4, float weight = 0.99f) :splitNum(splitnum), lambda(weight)
{
    float camera_fov = camera.Zoom;
    float ratio = 1.0;
    //略微添加偏移(这里为0.2)，避免在边框处有伪像
    for (int i = 0; i < splitNum; i++) {
        camera_frustums[i].Setfov(camera_fov);
        camera_frustums[i].Setratio(ratio);
    }
    //计算摄像机视线空间中每个平截头体切片的近距离和远距离
    split_camera_frustum(camera);
}
CSM_Dirlight::~CSM_Dirlight()
{}

//生成CSM所需的资源(分片光空间变换矩阵，眼空间到光空间变换矩阵(负灯光向量))
void CSM_Dirlight::Get_light_projection(Camera& camera, const glm::vec3& DirlightPosition) 
{ 
    lightView = glm::lookAt(DirlightPosition, camera.Position, glm::vec3(0.0f, 1.0f, 0.0f));
    //更新摄像机视锥分块的世界空间位置(8个顶点)
    camera_frustum_points(camera);
    //生成光空间的摄像机分片包围盒投影矩阵
    light_projection();
}
//计算摄像机视线空间中每个平截头体切片的近距离和远距离
void CSM_Dirlight::split_camera_frustum(Camera& camera) {
    float near_plane = camera.near_plane;
    float far_plane = camera.far_plane;
    float ratio = far_plane / near_plane;
    camera_frustums[0].Setnear(near_plane);
    for (int i = 1; i < splitNum; i++) {
        float si = i / (float)splitNum;
        float t_near = lambda * near_plane * powf(ratio, si) + (1 - lambda) * (near_plane + (far_plane - near_plane) * si);
        float t_far = t_near * 1.5f;//略微增加重合，避免断裂
        camera_frustums[i].Setnear(t_near);
        camera_frustums[i - 1].Setfar(t_far);
    }
    camera_frustums[splitNum - 1].Setfar(far_plane);
}
//更新摄像机视锥分块的世界空间位置(8个顶点)
void CSM_Dirlight::camera_frustum_points(Camera& camera) {
    glm::vec3 center;
    if (camera.third_view)
        center = camera.character_pos - glm::vec3(3.0f) * camera.Front;
    else
        center = camera.Position;
    glm::vec3 view_dir = camera.Front;
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::cross(view_dir, up);

    for (int i = 0; i < splitNum; i++) {
        Frustum& t_frustum = camera_frustums[i];

        glm::vec3 fc = center + view_dir * t_frustum.Getfar();
        glm::vec3 nc = center + view_dir * t_frustum.Getnear();

        right = glm::normalize(right);
        up = glm::normalize(glm::cross(right, view_dir));
        // 计算当前分片的近平面和远平面宽高的一半
        float near_height = tan(t_frustum.Getfov() / 2.0f) * t_frustum.Getnear();
        float near_width = near_height * t_frustum.Getratio();
        float far_height = tan(t_frustum.Getfov() / 2.0f) * t_frustum.Getfar();
        float far_width = far_height * t_frustum.Getratio();

        //记录眼视锥8个顶点
        t_frustum.m_points[0] = nc - up * near_height - right * near_width;//左下后
        t_frustum.m_points[1] = nc + up * near_height - right * near_width;//左上后
        t_frustum.m_points[2] = nc + up * near_height + right * near_width;//左上前
        t_frustum.m_points[3] = nc - up * near_height + right * near_width;//左下前
        t_frustum.m_points[4] = fc - up * far_height - right * far_width;
        t_frustum.m_points[5] = fc + up * far_height - right * far_width;
        t_frustum.m_points[6] = fc + up * far_height + right * far_width;
        t_frustum.m_points[7] = fc - up * far_height + right * far_width;
    }
}

//生成光视锥切片的光空间变换矩阵(lightView为光观察矩阵)
void CSM_Dirlight::light_projection()
{
    float distance[4];
    for (int i = 0; i < splitNum; i++) {
        Frustum& t_frustum = camera_frustums[i];
        glm::vec3 tmax(-1000.0f, -1000.0f, 0.0f);
        glm::vec3 tmin(1000.0f, 1000.0f, 0.0f);
        //寻找光空间八个顶点的最大最小z值
        glm::vec4 t_transf = lightView * glm::vec4(t_frustum.m_points[0], 1.0f);
        tmin.z = t_transf.z;//注意z是负数，near值比较大
        tmax.z = t_transf.z;

        for (int j = 1; j < 8; j++) {
            t_transf = lightView * glm::vec4(t_frustum.m_points[j], 1.0f);
            if (t_transf.z > tmax.z) { tmax.z = t_transf.z; }
            if (t_transf.z < tmin.z) { tmin.z = t_transf.z; }
        }
        //可能在距离光源较近处存在遮挡物(不知道给啥值，给个大一点的吧)
        tmax.z += 10;//如果没有这个，近距离观察时会失去阴影，很重要
        tmin.z -= 10;
        distance[i] = abs(tmax.z - tmin.z);
        z_distance[i] = distance[i] / distance[0];
        //生成光的正交投影矩阵(长宽用单位1，每个视锥分片的光正交投影矩阵可以通过该单位宽高投影矩阵缩放平移后得到)
        glm::mat4 light_orth = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -tmax.z, -tmin.z);
        //找到在光空间下视锥切片的(x,y)范围
        for (int j = 0; j < 8; j++) {
            t_transf = lightView * glm::vec4(t_frustum.m_points[j], 1.0f);
            if (t_transf.x > tmax.x) { tmax.x = t_transf.x; }
            if (t_transf.x < tmin.x) { tmin.x = t_transf.x; }
            if (t_transf.y > tmax.y) { tmax.y = t_transf.y; }
            if (t_transf.y < tmin.y) { tmin.y = t_transf.y; }
        }
        glm::vec2 tscale((tmax.x - tmin.x)/ 2.0, (tmax.y - tmin.y)/2.0);
        xy_distance[i] = tscale.x * tscale.y;
        glm::vec2 toffset(0.5f * (tmax.x + tmin.x), 0.5f * (tmax.y + tmin.y));
        glm::mat4 scale_move = glm::mat4(1.0);
        scale_move[0][0] = tscale.x;
        scale_move[1][1] = tscale.y;
        scale_move[0][3] = toffset.x;
        scale_move[1][3] = toffset.y;
        scale_move = glm::inverse(glm::transpose(scale_move));//注意glm按列储存，实际矩阵要转置,视锥缩小相当于视口放大，用逆矩阵
        //保存视锥切片的光投影矩阵
        light_orth_matrix[i] = scale_move * light_orth;
        //保存世界坐标到光空间变换的矩阵(每个视锥切片保存一个自己的)
        light_projection_matrix[i] = scale_move * light_orth * lightView;
    }

}


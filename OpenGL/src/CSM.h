#pragma once
#include "Camera.h"
class Frustum {
private:
    float fov;
    float ratio;//��߱�
    float near;
    float far;
public:
    std::vector<glm::vec3> m_points = std::vector<glm::vec3>(8);//ƽ��ͷ��8����
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
    int splitNum;//��׶��Ƭ��
    float lambda;//����Ȩ��
    std::vector<Frustum> camera_frustums = std::vector<Frustum>(splitNum);
    std::vector<float> z_distance = std::vector<float>(splitNum);
    std::vector<float> xy_distance = std::vector<float>(splitNum);
    float far_plane_distance;
public:
    CSM_Dirlight(Camera& camera, int splitnum, float weight);
    ~CSM_Dirlight();
    glm::mat4 lightView;//��ռ�view����
    std::vector<glm::mat4> light_orth_matrix = std::vector<glm::mat4>(splitNum);//��Ƭ��ռ�任����(ͶӰ*�۲�)
    std::vector<glm::mat4> light_projection_matrix = std::vector<glm::mat4>(splitNum);//��ռ�ͶӰ����
    //����CSM�������Դ(��Ƭ��ռ�任�����ۿռ䵽��ռ�任����)
    void split_camera_frustum(Camera& camera);
    float Get_frustum_far(int i)
    {
        return camera_frustums[i].Getfar();
    }
    //��������ռ��е������׶��Ƭ�߽��
    void camera_frustum_points(Camera& camera);
    //���ɹ���׶��Ƭ�Ĺ�ռ�任����
    void light_projection();
    void Get_light_projection(Camera& camera, const glm::vec3& DirlightPosition);
    float Get_z_distance(int i) { return z_distance[i]; }
    float Get_xy_distance(int i) { return xy_distance[i]; }
    void Set_far_plane(float fp) { far_plane_distance = fp; }
    float Get_far_plane() { return far_plane_distance; }
};
CSM_Dirlight::CSM_Dirlight(Camera& camera, int splitnum = 4, float weight = 0.9f) :splitNum(splitnum), lambda(weight)
{
    float camera_fov = camera.Zoom;
    float ratio = 1.0;
    //��΢���ƫ��(����Ϊ0.2)�������ڱ߿���α��
    for (int i = 0; i < splitNum; i++) {
        camera_frustums[i].Setfov(camera_fov);
        camera_frustums[i].Setratio(ratio);
    }
    //������������߿ռ���ÿ��ƽ��ͷ����Ƭ�Ľ������Զ����
    
}
CSM_Dirlight::~CSM_Dirlight()
{}

//����CSM�������Դ(��Ƭ��ռ�任�����ۿռ䵽��ռ�任����(���ƹ�����))
void CSM_Dirlight::Get_light_projection(Camera& camera, const glm::vec3& DirlightPosition) 
{ 
    lightView = glm::lookAt(DirlightPosition, camera.Position, glm::vec3(0.0f, 1.0f, 0.0f));
    split_camera_frustum(camera);
    //�����������׶�ֿ������ռ�λ��(8������)
    camera_frustum_points(camera);
    //���ɹ�ռ���������Ƭ��Χ��ͶӰ����
    light_projection();
}
//������������߿ռ���ÿ��ƽ��ͷ����Ƭ�Ľ������Զ����
void CSM_Dirlight::split_camera_frustum(Camera& camera) {
    float near_plane = camera.near_plane;
    float far_plane = min(far_plane_distance, 200.0f);
    float ratio = far_plane / near_plane;
    camera_frustums[0].Setnear(near_plane);
    for (int i = 1; i < splitNum; i++) {
        float si = i / (float)splitNum;
        float t_near = lambda * near_plane * powf(ratio, si) + (1 - lambda) * (near_plane + (far_plane - near_plane) * si);
        float t_far = t_near * 1.5f;//��΢�����غϣ��������
        camera_frustums[i].Setnear(t_near);
        camera_frustums[i - 1].Setfar(t_far);
    }
    camera_frustums[splitNum - 1].Setfar(far_plane);
}
//�����������׶�ֿ������ռ�λ��(8������)
void CSM_Dirlight::camera_frustum_points(Camera& camera) {
    glm::vec3 center;
    if (camera.third_view)
        center = camera.Position - glm::vec3(3.0f) * camera.Front;
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
        // ���㵱ǰ��Ƭ�Ľ�ƽ���Զƽ���ߵ�һ��
        float near_height = tan(t_frustum.Getfov() / 2.0f) * t_frustum.Getnear();
        float near_width = near_height * t_frustum.Getratio();
        float far_height = tan(t_frustum.Getfov() / 2.0f) * t_frustum.Getfar();
        float far_width = far_height * t_frustum.Getratio();

        //��¼����׶8������
        t_frustum.m_points[0] = nc - up * near_height - right * near_width;//���º�
        t_frustum.m_points[1] = nc + up * near_height - right * near_width;//���Ϻ�
        t_frustum.m_points[2] = nc + up * near_height + right * near_width;//����ǰ
        t_frustum.m_points[3] = nc - up * near_height + right * near_width;//����ǰ
        t_frustum.m_points[4] = fc - up * far_height - right * far_width;
        t_frustum.m_points[5] = fc + up * far_height - right * far_width;
        t_frustum.m_points[6] = fc + up * far_height + right * far_width;
        t_frustum.m_points[7] = fc - up * far_height + right * far_width;
    }
}

//���ɹ���׶��Ƭ�Ĺ�ռ�任����(lightViewΪ��۲����)
void CSM_Dirlight::light_projection()
{
    float distance[4]{};
    for (int i = 0; i < splitNum; i++) {
        Frustum& t_frustum = camera_frustums[i];
        glm::vec3 tmax(-1000.0f, -1000.0f, 0.0f);
        glm::vec3 tmin(1000.0f, 1000.0f, 0.0f);
        //Ѱ�ҹ�ռ�˸�����������Сzֵ
        glm::vec4 t_transf = lightView * glm::vec4(t_frustum.m_points[0], 1.0f);
        tmin.z = t_transf.z;//ע��z�Ǹ�����nearֵ�Ƚϴ�
        tmax.z = t_transf.z;

        for (int j = 1; j < 8; j++) {
            t_transf = lightView * glm::vec4(t_frustum.m_points[j], 1.0f);
            if (t_transf.z > tmax.z) { tmax.z = t_transf.z; }
            if (t_transf.z < tmin.z) { tmin.z = t_transf.z; }
        }
        //�����ھ����Դ�Ͻ��������ڵ���(��֪����ɶֵ��������һ��İ�)
        tmax.z += 1000;//���û�������������۲�ʱ��ʧȥ��Ӱ������Ҫ
        //tmin.z -= 10;
        distance[i] = abs(tmax.z - tmin.z);
        z_distance[i] = distance[i] / distance[0];
        //���ɹ������ͶӰ����(�����õ�λ1��ÿ����׶��Ƭ�Ĺ�����ͶӰ�������ͨ���õ�λ���ͶӰ��������ƽ�ƺ�õ�)
        glm::mat4 light_orth = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -tmax.z, -tmin.z);
        //�ҵ��ڹ�ռ�����׶��Ƭ��(x,y)��Χ
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
        scale_move = glm::inverse(glm::transpose(scale_move));//ע��glm���д��棬ʵ�ʾ���Ҫת��,��׶��С�൱���ӿڷŴ��������
        //������׶��Ƭ�Ĺ�ͶӰ����
        light_orth_matrix[i] = scale_move * light_orth;
        //�����������굽��ռ�任�ľ���(ÿ����׶��Ƭ����һ���Լ���)
        light_projection_matrix[i] = scale_move * light_orth * lightView;
    }

}


#include "Camera.h"

Camera::Camera()
{
    lookAt(0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0);
    calcViewPort(60, 600, 600);
}

void Camera::lookAt(float eye_x, float eye_y, float eye_z,
    float center_x, float center_y, float center_z,
    float up_x, float up_y, float up_z)
{
    origin = Point3f(eye_x, eye_y, eye_z);
    direction = normalize(Vec3f(center_x - eye_x, center_y - eye_y, center_z - eye_z));

    up = Vec3f(up_x, up_y, up_z);

    //����õ���������ϵ
    right = normalize(cross(direction, up));
    up = normalize(cross(right, direction));
}

void Camera::calcViewPort(float _fov, int _width, int _height)
{
    fov = _fov;
    width = _width;
    height = _height;

    float aspect = (float)height / (float)width;

    //�������߼���λ��
    view_x = right * 2 * tan(fov * PI / 360);//��֤�ӿ��е�ray�Ƕ����Ӿ���fovy�Ƕ���
    view_y = up * 2 * tan(fov * aspect * PI / 360);
    view_z = direction;
}


//���Ӿ����ڸ�����������λ�ã�������Ӧ��ray
Ray Camera::getRay(float x, float y)//����0 =< x <= 1, 0 =< y <= 1
{
    Vec3f& direction = view_z + (x - 0.5f) * view_x
        + (y - 0.5f) * view_y;       //�м�Ϊ�ӵ㷽�����Լ�0.5
    return Ray(origin, direction);
}


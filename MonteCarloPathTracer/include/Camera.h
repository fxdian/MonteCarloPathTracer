#ifndef __CAMERA_H
#define __CAMERA_H

#include "Vec.h"
#include "Ray.h"

class Camera
{
public:
    int width, height;//�ӿڵĴ�С/���ڴ�С

    Point3f origin;
    Vec3f direction;

    float fov;//��ֱ�����filed of view
    Vec3f front, up, right;//��������÷���
    Vec3f view_x, view_y, view_z;//���ߵļ��޷���

    Camera();

    //ȷ��Camera�ڷ�λ�úͷ���
    void lookAt(float eye_x, float eye_y, float eye_z,
        float center_x, float center_y, float center_z,
        float up_x, float up_y, float up_z);

    //�������߼���λ��
    void calcViewPort(float _fov, int _width, int _height);

    //���Ӿ����ڸ�����������λ�ã�������Ӧ��ray
    Ray getRay(float x, float y); 

    


};

#endif
#include "PathTracer.h"
#include <omp.h>
#include <iostream>
#include <string>
#include <ctime>

using namespace std;

static const Color3f BLACK = Color3f(0.0f, 0.0f, 0.0f);

float getFresnelIndex(double ni, double nt, double cosTheta)
{
    //fresnel����ϵ����schlick����
    float f0 = (ni - nt) / (ni + nt);
    f0 *= f0;
    float schlick = f0 + (1 - f0)* pow(1.0 - cosTheta, 5);

    return schlick;
}

PathTracer::PathTracer(Scene* _pScene)
{
    //srand(0);
    srand(int(time(NULL)));//�̶���������ڵ���
    pScene = _pScene;
    iterations = 0;
    px_sample_num = 1;
    mc_sample_num = 100;
    max_rescursive_depth = 5;
    use_direct_light = true;
    ambient = Color3f(0.2, 0.2, 0.2);
}

Scene* PathTracer::getScene()
{
    return pScene;
}

// cosine-weighted importance sampling
Vec3f PathTracer::importanceSample(Vec3f up, float n)
{
    //srand(0);
    double phi, theta;
    double r1 = (double)rand() / RAND_MAX, r2 = (double)rand() / RAND_MAX;

    phi = r1 * 2 * PI;
    theta = n<0 ? asin(sqrt(r2)) : acos(pow(r2, 1 / (n + 1)));
    Vec3f sample(sin(theta)*cos(phi), cos(theta), sin(theta)*sin(phi));

    //�õ��Ĳ���sample��Ҫ�뷴��ⷽ����������Ӧ
    //Ҳ����˵��sample.y��Ӧup����ĳ��ȣ���������
    Vec3f front, right;//

    if (fabs(up.x) > fabs(up.y))
        front = Vec3f(up.z, 0, -up.x);
    else
        front = Vec3f(0, -up.z, up.y);

    front = normalize(front);
    right = cross(up, front);

    return normalize(sample.x*right + sample.y*up + sample.z*front);
}

bool PathTracer::russianRoulette(float probability, float& survival)
{
    //srand(0);
    survival = (double)rand() / RAND_MAX;
    if (survival > probability) return true;
    return false;
}

Ray PathTracer::monteCarloSample(Ray& ray, Intersection& intersection)
{
    //srand(0);
    Material& material = intersection.pGeometry->material;
    Reflectance& ref = intersection.pGeometry->material.ref;

    Vec3f direction;
    float num[2];
    num[0] = dot(ref.kd, Vec3f(1, 1, 1));
    num[1] = dot(ref.ks, Vec3f(1, 1, 1)) + num[0];

    if (num[1] <= 0) return Ray(intersection.point, direction);

    if (material.bUseFresnel && material.refractiveIndex != 1.0)
    {
        float ni, nt;
        float cosTheta = dot(ray.direction, intersection.normal);

        //�����䷨�ߣ��볡�����巨��ͬ�����
        Vec3f normal = cosTheta <= 0.0 ? intersection.normal : -intersection.normal;

        if (cosTheta>0.0) //ȷ��ray�Ǵ��ڵ��⻹�Ǵ��⵽��
        {
            ni = material.refractiveIndex;
            nt = 1.0;
            //ni = 1.8;
        }
        else
        {
            ni = 1.0;
            nt = material.refractiveIndex;
            //nt = 1.8;
        }


        //Reflection or refraction?
        float fresnelIndex = getFresnelIndex(ni, nt, abs(cosTheta));
        float transmissonSurvival;

        /*double Tr = 1 - fresnelIndex;
        double P = 0.25 + 0.5*fresnelIndex;
        double RP = fresnelIndex / P;
        double TP = Tr / (1 - P);*/

        if (russianRoulette(fresnelIndex, transmissonSurvival))
        {
            if (ray.refract(normal, ni / nt, direction))
                return Ray(intersection.point, direction, SOURCE::TRANSMISSON);
            else
            {
                direction = ray.reflect(normal);
                return Ray(intersection.point, direction, SOURCE::SPECULAR_REFLECT);
            }
        }
    }

    //If reflection : diffuse or specular?
    float specularSurvival;
    if (russianRoulette(num[0] / num[1], specularSurvival))
    {
        Vec3f& prefectReflectDirection = ray.reflect(intersection.normal);
        direction = importanceSample(prefectReflectDirection, material.shiness);
        return Ray(intersection.point, direction, SOURCE::SPECULAR_REFLECT);
    }
    else
    {
        direction = importanceSample(intersection.normal);
        return Ray(intersection.point, direction, SOURCE::DIFFUSE_REFLECT);
    }
}

Color3f PathTracer::trace(Ray& ray, int cur_depth)
{
    Intersection intersection;
    bool is_intersected = pScene->intersect(ray, intersection);
    if (!is_intersected) return BLACK;//�ݹ���ray��pScene���ཻ����
    else
    {
        Material& material = intersection.pGeometry->material;

        if (cur_depth >= max_rescursive_depth) //���ߵݹ��ڴﵽ���ݹ����ʱ����
            return material.emission;

        //ע����һ��֮���н�ɢЧ��
        //�����ǰ�Ǽ�ӹⲢ��ʹ����ֱ�ӹ��գ�һ��hit��һ����Դʱ������Ӧ�÷��ؿգ��������Ա����ֱ�ӹ��յĹ�����
        //if (cur_depth == 0 && use_direct_light&&material.emission != BLACK) return BLACK;
        //if (cur_depth&& use_direct_light&&material.emission != BLACK) return BLACK;

        Color3f indirectIllumination, directIllumination;

        Reflectance& ref = material.ref;
        Ray& newRay = monteCarloSample(ray, intersection);

        if (newRay.source != SOURCE::NONE)
        {
            //indirectIllumination��return attr.emission + directIllumination + indirectIllumination;�õ��ۼ�����/��ɫ
            indirectIllumination = trace(newRay, cur_depth + 1);

            Vec3f view_direction = normalize(pScene->pCamera->origin - intersection.point);
            Vec3f reflect_direction = normalize(newRay.direction - intersection.normal * 2 * dot(newRay.direction, intersection.normal));
            

            switch (newRay.source)
            {
            case SOURCE::DIFFUSE_REFLECT:
                indirectIllumination = ref.kd*indirectIllumination/**dot(newRay.direction, intersection.normal)*/;
                break;
            case SOURCE::SPECULAR_REFLECT:
                indirectIllumination = ref.ks*indirectIllumination/**pow(dot(view_direction, reflect_direction), material.shiness)*/;
                break;
            case SOURCE::TRANSMISSON:
                indirectIllumination = indirectIllumination;
                break;
            }
        }

        if (use_direct_light) directIllumination = pScene->directIllumination(intersection, ray);

        return material.emission + directIllumination + indirectIllumination + ref.ka*ambient;
        //return material.emission + indirectIllumination + ref.ka*ambient;
    }
}

float* PathTracer::render()
{
    ++iterations;

    if (iterations>mc_sample_num)//render()���Ա�����mc_sample_num�Σ�Ҳ����reder mc_sample_num��
    {
        return pScene->colors;
    }

    cout << "��" + to_string(iterations) + "�ε���" << endl;

    int width = pScene->getWidth(), height = pScene->getHeight();
    //ֻ��Releaseģʽ�¿�����OMP֧��
#pragma omp parallel for schedule(dynamic, 1)
    for (int y = 0; y<height; ++y)
    {
        for (int x = 0; x<width; ++x)
        {
            Color3f color;
            Ray* rays = pScene->getRays(x, y, px_sample_num);//��x��y�ӽǸ����������һϵ�й���

            for (int k = 0; k < px_sample_num; ++k)
            {
                Color3f result = trace(rays[k]);
                if (result.r < 0.0f)result.r = 0.0f;
                if (result.b < 0.0f)result.b = 0.0f;
                if (result.g < 0.0f)result.g = 0.0f;

                if (result.r > 1.0f)result.r = 1.0f;
                if (result.b > 1.0f)result.b = 1.0f;
                if (result.g > 1.0f)result.g = 1.0f;


                color += result;
            }

            color = color / px_sample_num;
            delete[] rays;

            int index = y*width * 3 + x * 3;
            pScene->colors[index] = (pScene->colors[index] * (iterations - 1) + color.r) / iterations;
            pScene->colors[index + 1] = (pScene->colors[index + 1] * (iterations - 1) + color.g) / iterations;
            pScene->colors[index + 2] = (pScene->colors[index + 2] * (iterations - 1) + color.b) / iterations;
        }
    }

    return pScene->colors;
}



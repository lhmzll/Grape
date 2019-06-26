#ifndef __JAVERNN_RANDOM_H__
#define __JAVERNN_RANDOM_H__

#include <random>

namespace javernn{
    class Random{
    public:
        static Random GetInstance();
        Random();
        ~Random();
        int GetUniformInt(int from, int to);
        float GetUniformFloat(float from, float to);
        float GetNormalFloat(float mean, float stddev);
        void SetUniformInt(int* data, int size, int from, int to);
        void SetUniformFloat(float* data, int size, float from, float to);
        void SetNormalFloat(float* data, int size, float mean, float stddev);
    private:
        std::default_random_engine generator_;
    };
}


#endif
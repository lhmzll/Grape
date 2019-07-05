
#include <memory>
#include <assert.h>
#include "javernn/op/fc.h"
#include "javernn/log.h"
#include "javernn/util/random.h"
#include "javernn/util/blas.h"
#include "javernn/util/gemm.h"
#include "javernn/util/activations.h"

namespace javernn{
    static std::string TAG = "Fc";
    Fc::Fc(uint32_t batch_size,uint32_t in_dim,uint32_t out_dim,bool has_bias)
    : Op({DATA,WEIGHTS,BIAS}, {DATA}),
    batch_size_(batch_size),
    in_dim_(in_dim),
    out_dim_(out_dim),
    has_bias_(has_bias)
    {
        next_[0] = std::make_shared<Tensor>(static_cast<Op *>(this),
        Shape({batch_size_,out_dim_}),DATA,gNetMode);
    }

    Fc::~Fc()
    {
    }

    void Fc::Setup()
    {
        Log::v(TAG," Setup");
        //create input tensor,only weights and bias
        if(prev_.size()==0){
            Log::v(TAG,"skip init weights");
        }else{
            //Log::v(TAG,"create weights");
            assert(prev_[0].get() != nullptr);
            prev_[1] = std::make_shared<Tensor>(static_cast<Op *>(this),
            Shape({out_dim_,in_dim_}),DATA,gNetMode);
            Random::GetInstance().SetNormalFloat((float *)prev_[1]->mutable_cpu_data(),
            prev_[1]->shape().count(),0,1);
            // uint32_t n = prev_[1]->shape().count(); 
            // float *data = (float *)prev_[1]->mutable_cpu_data();
            // if(n<1000){
            //     std::cout<<"w update after"<<std::endl;
            //     for(int i=0;i<n;i++){
            //         std::cout<<data[i]<<" ";
            //     }
            //     std::cout<<std::endl;
            // }
            //fill_cpu(prev_[1]->shape().count(),0.1,(float *)prev_[1]->cpu_data(),1);
#ifdef GPU
            if(gNetMode == GPU_MODE){
                prev_[1]->data_to_gpu();
            }
#endif
            if(has_bias_){
                //Log::v(TAG,"create bias");
                prev_[2] = std::make_shared<Tensor>(static_cast<Op *>(this),
                Shape({out_dim_}),DATA,gNetMode);
                fill_cpu(prev_[2]->shape().count(),0,(float *)prev_[2]->cpu_data(),1);
#ifdef GPU
                if(gNetMode == GPU_MODE){
                    prev_[2]->data_to_gpu();
                }
#endif
            }
        }
    }

    void Fc::ForwardCpu()
    {
        Log::v(TAG,"ForwardCpu");
        //get data
        Tensor* data_tensor = prev_[0].get();
        Tensor* weight_tensor = prev_[1].get();
        Tensor* bias_tensor = prev_[2].get();
        Tensor* out_data_tensor = next_[0].get();
        int m = batch_size_;
        int k = in_dim_;
        int n = out_dim_;
        assert(data_tensor != nullptr);
        assert(weight_tensor != nullptr);
        assert(out_data_tensor != nullptr);
        float *a = (float *)data_tensor->mutable_cpu_data();
        float *b = (float *)weight_tensor->mutable_cpu_data();
        float *c = (float *)out_data_tensor->mutable_cpu_data();
        fill_cpu(batch_size_*out_dim_,0,c,1);
        gemm(0,1,m,n,k,1,a,k,b,k,1,c,n);
        if(has_bias_){
            float *bias_data = (float *)bias_tensor->mutable_cpu_data();
            add_bias(c, bias_data, batch_size_, out_dim_, 1);
        }
        // std::cout<<"a"<<std::endl;
        // for(int i=0;i<in_dim_;i++){
        //     std::cout<<a[i]<<" ";
        // }
        // std::cout<<std::endl;
        // std::cout<<"b"<<std::endl;
        // for(int i=0;i<in_dim_*out_dim_;i++){
        //     std::cout<<c[i]<<" ";
        // }
        // std::cout<<std::endl;
        // std::cout<<"c"<<std::endl;
        // for(int i=0;i<out_dim_;i++){
        //     std::cout<<c[i]<<" ";
        // }
        // std::cout<<std::endl;
        activate_array(c,batch_size_*out_dim_,RELU);
        // std::cout<<"activate_array after"<<std::endl;
        // for(int i=0;i<out_dim_;i++){
        //     std::cout<<c[i]<<" ";
        // }
        // std::cout<<std::endl;
    } 

    void Fc::BackwardCpu()
    {
        Log::v(TAG,"BackwardCpu");
        //get data
        Tensor* data_tensor = prev_[0].get();
        Tensor* weight_tensor = prev_[1].get();
        Tensor* out_data_tensor = next_[0].get();
        Tensor* bias_tensor = prev_[2].get();

        float *output_data = (float *)out_data_tensor->mutable_cpu_data();
        float *input_diff = (float *)out_data_tensor->mutable_cpu_diff();
        // for(int i=0;i<out_dim_;i++){
        //     std::cout<<output_data[i]<<" ";
        // }
        // std::cout<<std::endl;
        // for(int i=0;i<out_dim_;i++){
        //     std::cout<<input_diff[i]<<" ";
        // }
        // std::cout<<std::endl;
        // gradient_array(output_data,batch_size_*out_dim_,RELU,input_diff);
        // for(int i=0;i<out_dim_;i++){
        //     std::cout<<input_diff[i]<<" ";
        // }
        // std::cout<<std::endl;
        int m = out_dim_;
        int k = batch_size_;
        int n = in_dim_;
        float *a = (float *)out_data_tensor->mutable_cpu_diff();
        float *b = (float *)data_tensor->mutable_cpu_data();
        float *c = (float *)weight_tensor->mutable_cpu_diff();

        fill_cpu(in_dim_*out_dim_,0,c,1);
        gemm(1,0,m,n,k,1,a,m,b,n,1,c,n);
        if(has_bias_) {
            float *bias_diff = (float *)bias_tensor->mutable_cpu_data();
            backward_bias(bias_diff, a, batch_size_, out_dim_, 1);
        }

        m = batch_size_;
        k = out_dim_;
        n = in_dim_;

        a = (float *)out_data_tensor->mutable_cpu_diff();
        b = (float *)weight_tensor->mutable_cpu_data();
        c = (float *)data_tensor->mutable_cpu_diff();
        fill_cpu(batch_size_*in_dim_,0,c,1);
        gemm(0,0,m,n,k,1,a,k,b,n,1,c,n);
    }

    void Fc::UpdateWeightsCpu(Optimizer &opt)
    {
        Log::v(TAG,"UpdateWeightsCpu");
        opt.UpdateCpu( prev_[1].get());
        if(has_bias_){
            //opt.UpdateCpu( prev_[2].get());
        }
    }

#ifdef GPU

    void Fc::ForwardGpu()
    {

    }

    void Fc::BackwardGpu()
    {

    }

    void Fc::UpdateWeightsGpu(Optimizer &opt)
    {

    }
#endif
}
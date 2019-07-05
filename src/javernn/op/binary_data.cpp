#include "javernn/op/binary_data.h"
#include "javernn/util/blas.h"
#include "javernn/log.h"

namespace javernn
{
    static std::string TAG = "BinaryData";
    BinaryData::BinaryData(std::string file_path, uint32_t batch_size, uint32_t in_dim,uint32_t out_dim,
    uint32_t data_offset, bool one_hot):
    Input(batch_size,out_dim),
    file_path_(file_path),
    data_offset_(data_offset),
    one_hot_(one_hot),
    in_dim_(in_dim),
    out_dim_(out_dim)
    {
        tmp_data_.reserve(in_dim);
        file_in_.open(file_path_,std::ios::binary);
        if(!file_in_.is_open()){
            Log::v(TAG,"file open error, path is "+file_path_);
        }
        Log::v(TAG,"file open sucess");
        file_in_.seekg(0,std::ios::end);
        file_size_ = file_in_.tellg();
        file_in_.seekg(data_offset_,std::ios::beg);
    }

    BinaryData::~BinaryData()
    {
        file_in_.close();
    }

    void BinaryData::Setup()
    {
        Log::v(TAG,"Setup");

    }

    void BinaryData::ForwardCpu()
    {
        Log::v(TAG,"ForwardCpu");
        auto out_tensor = GetOutputTensor();
        float *cpu_data = (float *)out_tensor->mutable_cpu_data();
        fill_cpu(batch_size_*out_dim_,0,cpu_data,1);
        // std::cout.setf(std::ios::fixed);//固定小数位数
        // std::cout.setf(std::ios::showpoint);//显示小数点
        // std::cout.precision(0);//设置输出精度为9位小数
        for(int i=0;i<batch_size_;i++){
            if(one_hot_){
                file_in_.read(tmp_data_.data(), in_dim_);
                for(int ii=0;ii<in_dim_;ii++){
                    int tmp = (int)(tmp_data_.data()[ii]&0xff);
//                    Log::v(TAG,std::to_string(tmp));
                    for(int j=0;j<out_dim_;j++){
                        if(tmp == j)
                            cpu_data[i*in_dim_*out_dim_+ii*out_dim_+j] = 1;
                        else 
                            cpu_data[i*in_dim_*out_dim_+ii*out_dim_+j] = 0;
                        //std::cout<<cpu_data[i*in_dim_*out_dim_+ii*out_dim_+j]<<" ";
                    }
                    //std::cout<<std::endl;
                }
            }else{
                file_in_.read(tmp_data_.data(), out_dim_);
                for(int i=0;i<out_dim_;i++){
                    cpu_data[i] = ((float)(tmp_data_.data()[i]&0xff))/255.0;
                    // std::cout<<(cpu_data[i]?1:0)<<" ";
                    // if((i+1)%28==0){
                    //     std::cout<<std::endl;
                    // }
                }
                // std::cout<<std::endl;
            }
        }

    } 

    void BinaryData::BackwardCpu()
    {
        Log::v(TAG,"BackwardCpu");
    }

    void BinaryData::UpdateWeightsCpu(Optimizer &opt)
    {

    }

#ifdef GPU
    void BinaryData::ForwardGpu()
    {
        auto out_tensor = GetOutputTensor();
        float *cpu_data = (float *)out_tensor->mutable_cpu_data();
        fill_cpu(batch_size_*out_dim_,0,cpu_data,1);
        for(int i=0;i<batch_size_;i++){
            file_in_.read(tmp_data_.data(), out_dim_);
            for(int i=0;i<out_dim_;i++){
                cpu_data[i] = (float)(tmp_data_.data()[i]&0xff);
                //Log::v("",std::to_string(cpu_data[i]));
            }
        }
        out_tensor->data_to_gpu();
    } 

    void BinaryData::BackwardGpu()
    {

    }

    void BinaryData::UpdateWeightsGpu(Optimizer &opt)
    {

    }
#endif
} // namespace javernnvoid Input::Setup()
  
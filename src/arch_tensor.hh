#include <stdio.h>
#include <sstream>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <memory>

#include "microarch_tensor.hh"

using namespace std;

const int ndim = 5;

class archTensor {
  public:
    struct archTensorDescriptor{
      unsigned int baseAddr;
      unsigned int size[ndim];
      unsigned int stride[ndim];
    };

    struct subArchTensorDescriptor{
      unsigned int coords[ndim];
      unsigned int range[ndim];
      unsigned int traversal_stride[ndim];
    };

    archTensorDescriptor tensorDesc;
    subArchTensorDescriptor subTensorDesc;
    microarchTensor* fatherMicroarchTensor;
    microarchTensor* subMicroarchTensor;

    archTensor(){
      tensorDesc.baseAddr  = genRandomData(100) * 64;
      tensorDesc.size[4]   = genRandomData(64);
      tensorDesc.size[3]   = genRandomData(64);
      tensorDesc.size[2]   = genRandomData(64);
      tensorDesc.size[1]   = genRandomData(64);
      tensorDesc.size[0]   = genRandomData(64);

      tensorDesc.stride[0]  = 1<<static_cast<int>(ceil(log2(tensorDesc.size[0])));
      tensorDesc.stride[1]  = tensorDesc.size[1] * tensorDesc.stride[0];
      tensorDesc.stride[2]  = tensorDesc.size[2] * tensorDesc.stride[1];
      tensorDesc.stride[3]  = tensorDesc.size[3] * tensorDesc.stride[2];
      tensorDesc.stride[4]  = tensorDesc.size[4] * tensorDesc.stride[3];
    }

    ~archTensor(){
      delete fatherMicroarchTensor;
      delete subMicroarchTensor;
    }

    unsigned int genRandomData(unsigned int maxValue=10){
      std::srand(static_cast<unsigned int>(std::time(0)));
      unsigned int randomNum = std::rand() % maxValue + 1;

      return randomNum;
    }

    void stringTensor(archTensorDescriptor myTensorDesc){
      cout << "baseAddr: " << myTensorDesc.baseAddr << endl;
      cout <<"myTensorDesc.size[0]:" << myTensorDesc.size[0] <<
           ", myTensorDesc.size[1]:" << myTensorDesc.size[1] <<
           ", myTensorDesc.size[2]:" << myTensorDesc.size[2] <<
           ", myTensorDesc.size[3]:" << myTensorDesc.size[3] <<
           ", myTensorDesc.size[4]:" << myTensorDesc.size[4] << endl;

      cout <<"myTensorDesc.stride[0]:" << myTensorDesc.stride[0] <<
           ", myTensorDesc.stride[1]:" << myTensorDesc.stride[1] <<
           ", myTensorDesc.stride[2]:" << myTensorDesc.stride[2] <<
           ", myTensorDesc.stride[3]:" << myTensorDesc.stride[3] <<
           ", myTensorDesc.stride[4]:" << myTensorDesc.stride[4] << endl;
      cout << "" << endl;
    }

    void genSubTensor(){
      subTensorDesc.coords[0] = genRandomData(tensorDesc.size[0]);
      subTensorDesc.coords[1] = genRandomData(tensorDesc.size[1]);
      subTensorDesc.coords[2] = genRandomData(tensorDesc.size[2]);
      subTensorDesc.coords[3] = genRandomData(tensorDesc.size[3]);
      subTensorDesc.coords[4] = genRandomData(tensorDesc.size[4]);

      subTensorDesc.range[0]  = genRandomData(tensorDesc.size[0]-subTensorDesc.coords[0]);
      subTensorDesc.range[1]  = genRandomData(tensorDesc.size[1]-subTensorDesc.coords[1]);
      subTensorDesc.range[2]  = genRandomData(tensorDesc.size[2]-subTensorDesc.coords[2]);
      subTensorDesc.range[3]  = genRandomData(tensorDesc.size[3]-subTensorDesc.coords[3]);
      subTensorDesc.range[4]  = genRandomData(tensorDesc.size[4]-subTensorDesc.coords[4]);

      subTensorDesc.traversal_stride[0] = 1;
      subTensorDesc.traversal_stride[1] = 1;
      subTensorDesc.traversal_stride[2] = 1;
      subTensorDesc.traversal_stride[3] = 1;
      subTensorDesc.traversal_stride[4] = 1;

      cout <<"tensor descriptor:" << endl;
      stringTensor(tensorDesc);
    }

    void convertTesor2MicroarchTensorDescriptor(){
      int baseAddr = 0;

      int byteNum  = 0;
      int unitNum  = 0;
      int sliceNum = 0;
      int planeNum = 0;
      int cubeNum  = 0;

      int unitSkip  = 0;
      int sliceSkip = 0;
      int planeSkip = 0;
      int cubeSkip  = 0;
      this->fatherMicroarchTensor = new microarchTensor (baseAddr, byteNum, unitNum, sliceNum, planeNum, cubeNum, unitSkip, sliceSkip, planeSkip, cubeSkip);
    }

    void convertSubTesor2MicroarchTensorDescriptor(){
      int baseAddr = 0;

      int byteNum  = 0;
      int unitNum  = 0;
      int sliceNum = 0;
      int planeNum = 0;
      int cubeNum  = 0;

      int unitSkip  = 0;
      int sliceSkip = 0;
      int planeSkip = 0;
      int cubeSkip  = 0;

      this->subMicroarchTensor = new microarchTensor (baseAddr, byteNum, unitNum, sliceNum, planeNum, cubeNum, unitSkip, sliceSkip, planeSkip, cubeSkip);
    }
};


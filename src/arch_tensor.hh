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
      unsigned int dimension[ndim];
      unsigned int stride[ndim];
    };

    struct subArchTensorDescriptor{
      unsigned int coords[ndim];
      unsigned int range[ndim];
      unsigned int traversalStride[ndim];
    };

    archTensorDescriptor tensorDesc;
    subArchTensorDescriptor subTensorDesc;
    microarchTensor* fatherMicroarchTensor;
    microarchTensor* subMicroarchTensor;

    archTensor(){
      tensorDesc.baseAddr  = genRandomData(100) * 64;
      tensorDesc.dimension[0]   = genRandomData(64);
      tensorDesc.dimension[1]   = genRandomData(40);
      tensorDesc.dimension[2]   = genRandomData(10);
      tensorDesc.dimension[3]   = genRandomData(20);
      tensorDesc.dimension[4]   = genRandomData(6);

      tensorDesc.stride[0]  = 1<<static_cast<int>(ceil(log2(tensorDesc.dimension[0])));
      tensorDesc.stride[1]  = tensorDesc.dimension[1] * tensorDesc.stride[0];
      tensorDesc.stride[2]  = tensorDesc.dimension[2] * tensorDesc.stride[1];
      tensorDesc.stride[3]  = tensorDesc.dimension[3] * tensorDesc.stride[2];
      tensorDesc.stride[4]  = tensorDesc.dimension[4] * tensorDesc.stride[3];
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
      cout <<"myTensorDesc.dimension[0]:" << myTensorDesc.dimension[0] <<
        ", myTensorDesc.dimension[1]:" << myTensorDesc.dimension[1] <<
        ", myTensorDesc.dimension[2]:" << myTensorDesc.dimension[2] <<
        ", myTensorDesc.dimension[3]:" << myTensorDesc.dimension[3] <<
        ", myTensorDesc.dimension[4]:" << myTensorDesc.dimension[4] << endl;

      cout <<"myTensorDesc.stride[0]:" << myTensorDesc.stride[0] <<
        ", myTensorDesc.stride[1]:" << myTensorDesc.stride[1] <<
        ", myTensorDesc.stride[2]:" << myTensorDesc.stride[2] <<
        ", myTensorDesc.stride[3]:" << myTensorDesc.stride[3] <<
        ", myTensorDesc.stride[4]:" << myTensorDesc.stride[4] << endl;
      cout << "" << endl;
    }

    unsigned int max(unsigned int x, unsigned int y) {
      return (x > y) ? x : y;
    }

    void genSubTensor(){
      subTensorDesc.coords[0] = genRandomData(tensorDesc.dimension[0]);
      subTensorDesc.coords[1] = genRandomData(tensorDesc.dimension[1]);
      subTensorDesc.coords[2] = genRandomData(tensorDesc.dimension[2]);
      subTensorDesc.coords[3] = genRandomData(tensorDesc.dimension[3]);
      subTensorDesc.coords[4] = genRandomData(tensorDesc.dimension[4]);

      subTensorDesc.range[0]  = genRandomData(max(1, tensorDesc.dimension[0] - subTensorDesc.coords[0]));
      subTensorDesc.range[1]  = genRandomData(max(1, tensorDesc.dimension[1] - subTensorDesc.coords[1]));
      subTensorDesc.range[2]  = genRandomData(max(1, tensorDesc.dimension[2] - subTensorDesc.coords[2]));
      subTensorDesc.range[3]  = genRandomData(max(1, tensorDesc.dimension[3] - subTensorDesc.coords[3]));
      subTensorDesc.range[4]  = genRandomData(max(1, tensorDesc.dimension[4] - subTensorDesc.coords[4]));

      subTensorDesc.traversalStride[0] = 1;
      subTensorDesc.traversalStride[1] = 1;
      subTensorDesc.traversalStride[2] = 1;
      subTensorDesc.traversalStride[3] = 1;
      subTensorDesc.traversalStride[4] = 1;
    }

    void convertTesor2MicroarchTensorDescriptor(){
      int baseAddr = tensorDesc.baseAddr;

      int byteNum  = tensorDesc.dimension[0];
      int unitNum  = tensorDesc.dimension[1];
      int sliceNum = tensorDesc.dimension[2];
      int planeNum = tensorDesc.dimension[3];
      int cubeNum  = tensorDesc.dimension[4];

      int unitSkip  = tensorDesc.stride[0];
      int sliceSkip = tensorDesc.stride[1];
      int planeSkip = tensorDesc.stride[2];
      int cubeSkip  = tensorDesc.stride[3];
      this->fatherMicroarchTensor = new microarchTensor(baseAddr, byteNum, unitNum, sliceNum, planeNum, cubeNum, unitSkip, sliceSkip, planeSkip, cubeSkip);
    }

    void convertSubTesor2MicroarchTensorDescriptor(){
      int baseAddr = subTensorDesc.coords[0] * tensorDesc.stride[0] +
        subTensorDesc.coords[1] * tensorDesc.stride[1] +
        subTensorDesc.coords[2] * tensorDesc.stride[2] +
        subTensorDesc.coords[3] * tensorDesc.stride[3] +
        subTensorDesc.coords[4] * tensorDesc.stride[4];

      int byteNum  = subTensorDesc.range[0];
      int unitNum  = subTensorDesc.range[1];
      int sliceNum = subTensorDesc.range[2];
      int planeNum = subTensorDesc.range[3];
      int cubeNum  = subTensorDesc.range[4];

      int unitSkip  = tensorDesc.stride[0] * subTensorDesc.traversalStride[0];
      int sliceSkip = tensorDesc.stride[1] * subTensorDesc.traversalStride[1];
      int planeSkip = tensorDesc.stride[2] * subTensorDesc.traversalStride[2];
      int cubeSkip  = tensorDesc.stride[3] * subTensorDesc.traversalStride[3];

      this->subMicroarchTensor = new microarchTensor(baseAddr, byteNum, unitNum, sliceNum, planeNum, cubeNum, unitSkip, sliceSkip, planeSkip, cubeSkip);
    }
};


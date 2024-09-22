#include<stdio.h>
#include<sstream>
#include<iostream>
#include<math.h>

using namespace std;

const int ndim = 5;

class tensor {
  public:
    struct archTensorDescriptor{
      unsigned int coords[ndim];
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

    tensor(){
    }

    ~tensor(){
    }

    int *traversalTensorAddrPoint(archTensorDescriptor myTensorDesc){
      int *traversalAddr = new int[100];
      return traversalAddr;
    }

    void genMicroTensorDescriptor(){
    }
};


int main(){
}

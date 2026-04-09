#include <stdio.h>
#include <sstream>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <memory>

#include "arch_tensor.hh"

int main(){
  archTensor myTensor;

  myTensor.convertTesor2MicroarchTensorDescriptor();
  myTensor.fatherMicroarchTensor->traversalTensorAddrPoint(myTensor.fatherMicroarchTensor->tensorDesc);

  myTensor.genSubTensor();
  myTensor.convertSubTesor2MicroarchTensorDescriptor();
  myTensor.subMicroarchTensor->traversalTensorAddrPoint(myTensor.subMicroarchTensor->tensorDesc);

  return 0;
}

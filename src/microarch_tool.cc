#include <stdio.h>
#include <sstream>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <memory>

#include "microarch_tensor.hh"

struct microarchTensorDescriptor{
  int baseAddr;
  int byteNum;
  int unitNum;
  int sliceNum;
  int planeNum;
  int cubeNum;
  int unitSkip;
  int sliceSkip;
  int planeSkip;
  int cubeSkip;
};

unsigned int genRandomData(unsigned int maxValue=10){
  std::srand(static_cast<unsigned int>(std::time(0)));
  unsigned int randomNum = std::rand() % maxValue + 1;

  return randomNum;
}

int main(){
  microarchTensorDescriptor tensorDesc;

  tensorDesc.baseAddr  = genRandomData(100) * 64;
  tensorDesc.byteNum   = genRandomData(64);
  tensorDesc.unitNum   = genRandomData(40);
  tensorDesc.sliceNum  = genRandomData(10);
  tensorDesc.planeNum  = genRandomData(20);
  tensorDesc.cubeNum   = genRandomData(6);

  tensorDesc.unitSkip  = 1<<static_cast<int>(ceil(log2(tensorDesc.byteNum)));
  tensorDesc.sliceSkip = tensorDesc.unitNum  * tensorDesc.unitSkip;
  tensorDesc.planeSkip = tensorDesc.sliceNum * tensorDesc.sliceSkip;
  tensorDesc.cubeSkip  = tensorDesc.planeNum * tensorDesc.planeSkip;

  microarchTensor myTensor(tensorDesc.baseAddr, tensorDesc.byteNum, tensorDesc.unitNum, tensorDesc.sliceNum, tensorDesc.planeNum, tensorDesc.cubeNum, tensorDesc.unitSkip, tensorDesc.sliceSkip, tensorDesc.planeSkip, tensorDesc.cubeSkip);

  int *traversalAddr = myTensor.traversalTensorAddrPoint(myTensor.tensorDesc);

  int tensorAddrPointNum = myTensor.getTraversalAddrPointNum(myTensor.tensorDesc);
  cout <<"tensorAddrPointNum : " << tensorAddrPointNum << endl;

  return 0;
}


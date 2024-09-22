#include <stdio.h>
#include <sstream>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <memory>

#include "microarch_tensor.hh"

int main(){

  microarchTensor myTensor;
  int *traversalAddr = myTensor.traversalTensorAddrPoint(myTensor.tensorDesc);
  int tensorAddrPointNum = myTensor.getTraversalAddrPointNum(myTensor.tensorDesc);
  myTensor.genSubTensor();
  int subTensorAddrPointNum = myTensor.getTraversalAddrPointNum(myTensor.subTensorDesc);

  // for(int i=0;i<addrPointNum;i++){
  //   cout << "traversalAddr = " << traversalAddr[i] << endl;
  // }

  return 0;
}


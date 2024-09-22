#include <stdio.h>
#include <sstream>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <memory>

using namespace std;

class microarchTensor {
  public:
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

    microarchTensorDescriptor tensorDesc;
    microarchTensorDescriptor subTensorDesc;

    microarchTensor(){
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
    }

    microarchTensor(int baseAddr, int byteNum, int unitNum, int sliceNum, int planeNum, int cubeNum, int unitSkip, int sliceSkip, int planeSkip, int cubeSkip){
      tensorDesc.baseAddr  = baseAddr;
      tensorDesc.byteNum   = byteNum;
      tensorDesc.unitNum   = unitNum;
      tensorDesc.sliceNum  = sliceNum;
      tensorDesc.planeNum  = planeNum;
      tensorDesc.cubeNum   = cubeNum;

      tensorDesc.unitSkip  = unitSkip;
      tensorDesc.sliceSkip = sliceSkip;
      tensorDesc.planeSkip = planeSkip;
      tensorDesc.cubeSkip  = cubeSkip;
    }

    ~microarchTensor(){
    }

    unsigned int genRandomData(unsigned int maxValue=10){
      std::srand(static_cast<unsigned int>(std::time(0)));
      unsigned int randomNum = std::rand() % maxValue + 1;

      return randomNum; }

    void stringTensor(microarchTensorDescriptor myTensorDesc){
      cout <<"baseAddr: " << myTensorDesc.baseAddr << endl;
      cout <<"byteNum: "  << myTensorDesc.byteNum  <<
           ", unitNum: "  << myTensorDesc.unitNum  <<
           ", sliceNum: " << myTensorDesc.sliceNum <<
           ", planeNum: " << myTensorDesc.planeNum <<
           ", cubeNum: "  << myTensorDesc.cubeNum << endl;

      cout <<"unitSkip : "<< myTensorDesc.unitSkip  << endl;
      cout <<"sliceSkip: "<< myTensorDesc.sliceSkip << endl;
      cout <<"planeSkip: "<< myTensorDesc.planeSkip << endl;
      cout <<"cubeSkip:  "<< myTensorDesc.cubeSkip  << endl;
      cout <<"" << endl;
    }

    void genSubTensor(){
      subTensorDesc.baseAddr  = tensorDesc.baseAddr;
      subTensorDesc.byteNum   = genRandomData(tensorDesc.byteNum);
      subTensorDesc.unitNum   = genRandomData(tensorDesc.unitNum);
      subTensorDesc.sliceNum  = genRandomData(tensorDesc.sliceNum);
      subTensorDesc.planeNum  = genRandomData(tensorDesc.planeNum);
      subTensorDesc.cubeNum   = genRandomData(tensorDesc.cubeNum);

      subTensorDesc.unitSkip  = tensorDesc.unitSkip;
      subTensorDesc.sliceSkip = tensorDesc.sliceSkip;
      subTensorDesc.planeSkip = tensorDesc.planeSkip;
      subTensorDesc.cubeSkip  = tensorDesc.cubeSkip;

      cout <<"tensor descriptor:" << endl;
      stringTensor(tensorDesc);

      cout <<"sub tensor descriptor:" << endl;
      stringTensor(subTensorDesc);
    }

    int getTraversalAddrPointNum(microarchTensorDescriptor myTensorDesc){
      return myTensorDesc.unitNum * myTensorDesc.sliceNum * myTensorDesc.planeNum * myTensorDesc.cubeNum;
    }

    int getSkipAddr(int baseAddr, int skip){
      int rawAddr = baseAddr + skip;
      int skipAddr = rawAddr & (~(-1<<22));

      return skipAddr;
    }

    int *traversalTensorAddrPoint(microarchTensorDescriptor myTensorDesc){
      int *traversalAddr = new int[getTraversalAddrPointNum(myTensorDesc)];

      stringTensor(myTensorDesc);

      int cubeAddr  = myTensorDesc.baseAddr;
      int planeAddr = cubeAddr;
      int sliceAddr = cubeAddr;
      int unitAddr  = cubeAddr;

      for(int cubeCnt=0; cubeCnt<myTensorDesc.cubeNum;cubeCnt++){
        cout << "cubeCnt= " << cubeCnt << endl;
        planeAddr = cubeAddr;
        cubeAddr = getSkipAddr(cubeAddr, myTensorDesc.cubeSkip);

        for(int planeCnt=0; planeCnt<myTensorDesc.planeNum;planeCnt++){
          cout << "planeCnt = " << planeCnt << endl;
          sliceAddr = planeAddr;
          planeAddr = getSkipAddr(planeAddr, myTensorDesc.planeSkip);

          for(int sliceCnt=0; sliceCnt<myTensorDesc.sliceNum;sliceCnt++){
            cout << "sliceCnt = " << sliceCnt << endl;
            unitAddr  = sliceAddr;
            sliceAddr = getSkipAddr(sliceAddr, myTensorDesc.sliceSkip);

            for(int unitCnt=0; unitCnt<myTensorDesc.unitNum;unitCnt++){
              cout << "unitCnt = " << unitCnt << endl;
              int idx = cubeCnt*myTensorDesc.planeNum*myTensorDesc.sliceNum*myTensorDesc.unitNum + planeCnt*myTensorDesc.sliceNum*myTensorDesc.unitNum + sliceCnt*myTensorDesc.unitNum + unitCnt;

              cout << "cubeCnt= " << cubeCnt << ", planeCnt = " << planeCnt << ", sliceCnt = " << sliceCnt << ", unitCnt = " << unitCnt << endl;
              cout << "idx = " << idx << ", unitAddr = " << dec << unitAddr << endl;
              traversalAddr[idx] = unitAddr;
              unitAddr += myTensorDesc.unitSkip;
            }
            cout << "***** slice done ******" << endl;
            cout << "" << endl;
          }
          cout << "----- plane done ------" << endl;
          cout << "" << endl;
        }
        cout << "===== cube done ======" << endl;
        cout << "" << endl;
      }

      return traversalAddr;
    }
};

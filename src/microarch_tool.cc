#include <stdio.h>
#include <sstream>
#include <iostream>
#include <fstream>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <memory>
#include <nlohmann/json.hpp>

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

nlohmann::json readJSONFile(std::string fileName){
  std::ifstream jsonFile(fileName);
  nlohmann::json result;

  if (jsonFile.is_open()) {
    try {
      result = nlohmann::json::parse(jsonFile);
    } catch (const nlohmann::json::parse_error& ex) {
      std::cerr << "JSON parse error: " << ex.what() << std::endl;
    }
    jsonFile.close();
  } else {
    std::cerr << "Unable to open file." << std::endl;
  }

  return result;
}

int main(){
  microarchTensorDescriptor tensorDesc;

  nlohmann::json configJSON = readJSONFile("../configs/demo.json");

  tensorDesc.baseAddr  = configJSON["baseAddr"];
  tensorDesc.byteNum   = configJSON["byteNum"];
  tensorDesc.unitNum   = configJSON["unitNum"];
  tensorDesc.sliceNum  = configJSON["sliceNum"];
  tensorDesc.planeNum  = configJSON["planeNum"];
  tensorDesc.unitSkip  = configJSON["unitSkip"];
  tensorDesc.sliceSkip = configJSON["sliceSkip"];
  tensorDesc.planeSkip = configJSON["planeSkip"];
  tensorDesc.cubeSkip  = configJSON["cubeSkip"];

  microarchTensor myTensor(tensorDesc.baseAddr, tensorDesc.byteNum, tensorDesc.unitNum, tensorDesc.sliceNum, tensorDesc.planeNum, tensorDesc.cubeNum, tensorDesc.unitSkip, tensorDesc.sliceSkip, tensorDesc.planeSkip, tensorDesc.cubeSkip);

  int *traversalAddr = myTensor.traversalTensorAddrPoint(myTensor.tensorDesc);

  int tensorAddrPointNum = myTensor.getTraversalAddrPointNum(myTensor.tensorDesc);
  cout <<"tensorAddrPointNum : " << tensorAddrPointNum << endl;

  return 0;
}


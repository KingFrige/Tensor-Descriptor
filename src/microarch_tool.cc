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

  nlohmann::json configJSON = readJSONFile("../configs/demo.json");

  for (const auto& item: configJSON) {
    microarchTensor myTensor(item["baseAddr"], item["byteNum"], item["unitNum"], item["sliceNum"], item["planeNum"], item["cubeNum"], item["unitSkip"], item["sliceSkip"], item["planeSkip"], item["cubeSkip"]);

    int *traversalAddr = myTensor.traversalTensorAddrPoint(myTensor.tensorDesc);

    int tensorAddrPointNum = myTensor.getTraversalAddrPointNum(myTensor.tensorDesc);
    cout <<"tensorAddrPointNum : " << tensorAddrPointNum << endl;
  }

  return 0;
}


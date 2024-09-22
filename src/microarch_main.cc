#include <stdio.h>
#include <sstream>
#include <iostream>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <memory>

using namespace std;

class tensor {
  public:
    struct micrarch_tensor_descriptor{
      int base_addr;
      int unit_num;
      int slice_num;
      int plane_num;
      int cube_num;
      int unit_skip;
      int slice_skip;
      int plane_skip;
      int cube_skip;
    };

    micrarch_tensor_descriptor tensor_desc;
    micrarch_tensor_descriptor sub_tensor_desc;

    tensor(){
      tensor_desc.base_addr  = gen_random_data(100) * 64;
      tensor_desc.unit_num   = gen_random_data(40);
      tensor_desc.slice_num  = gen_random_data(10);
      tensor_desc.plane_num  = gen_random_data(20);
      tensor_desc.cube_num   = gen_random_data(6);

      tensor_desc.unit_skip  = 1<<static_cast<int>(ceil(log2(gen_random_data(64))));
      tensor_desc.slice_skip = tensor_desc.unit_num  * tensor_desc.unit_skip;
      tensor_desc.plane_skip = tensor_desc.slice_num * tensor_desc.slice_skip;
      tensor_desc.cube_skip  = tensor_desc.plane_num * tensor_desc.plane_skip;
    }

    ~tensor(){
    }

    unsigned int gen_random_data(unsigned int max_value=10){
      std::srand(static_cast<unsigned int>(std::time(0)));
      unsigned int randomNum = std::rand() % max_value + 1;

      return randomNum;
    }

    void stringTensor(micrarch_tensor_descriptor myTensorDesc){
      cout <<"unit_num:"   << myTensorDesc.unit_num <<", slice_num:" << myTensorDesc.slice_num <<", plane_num:" << myTensorDesc.plane_num <<", cube_num:"<< myTensorDesc.cube_num << endl;
      cout <<"unit_skip : "<< myTensorDesc.unit_skip  << endl;
      cout <<"slice_skip: "<< myTensorDesc.slice_skip << endl;
      cout <<"plane_skip: "<< myTensorDesc.plane_skip << endl;
      cout <<"cube_skip:  "<< myTensorDesc.cube_skip  << endl;
      cout <<"" << endl;
    }

    void gen_sub_tensor(){
      sub_tensor_desc.base_addr  = tensor_desc.base_addr;
      sub_tensor_desc.unit_num   = gen_random_data(tensor_desc.unit_num);
      sub_tensor_desc.slice_num  = gen_random_data(tensor_desc.slice_num);
      sub_tensor_desc.plane_num  = gen_random_data(tensor_desc.plane_num);
      sub_tensor_desc.cube_num   = gen_random_data(tensor_desc.cube_num);

      sub_tensor_desc.unit_skip  = tensor_desc.unit_skip;
      sub_tensor_desc.slice_skip = tensor_desc.slice_skip;
      sub_tensor_desc.plane_skip = tensor_desc.plane_skip;
      sub_tensor_desc.cube_skip  = tensor_desc.cube_skip;

      cout <<"tensor descriptor:" << endl;
      stringTensor(tensor_desc);

      cout <<"sub tensor descriptor:" << endl;
      stringTensor(sub_tensor_desc);
    }

    int get_traversal_addr_point_num(micrarch_tensor_descriptor my_tensor_desc){
      return my_tensor_desc.unit_num * my_tensor_desc.slice_num * my_tensor_desc.plane_num * my_tensor_desc.cube_num;
    }

    int get_skip_addr(int base_addr, int skip){
      int raw_addr = base_addr + skip;
      int skip_addr = raw_addr & (~(-1<<22));

      return skip_addr;
    }

    int *traversal_tensor_addr_point(micrarch_tensor_descriptor my_tensor_desc){
      int *traversalAddr = new int[get_traversal_addr_point_num(my_tensor_desc)];

      stringTensor(my_tensor_desc);

      int cube_addr  = my_tensor_desc.base_addr;
      int plane_addr = cube_addr;
      int slice_addr = cube_addr;
      int unit_addr  = cube_addr;

      for(int cubeCnt=0; cubeCnt<my_tensor_desc.cube_num;cubeCnt++){
        cout << "cubeCnt= " << cubeCnt << endl;
        plane_addr = cube_addr;
        cube_addr = get_skip_addr(cube_addr, my_tensor_desc.cube_skip);

        for(int planeCnt=0; planeCnt<my_tensor_desc.plane_num;planeCnt++){
          cout << "planeCnt = " << planeCnt << endl;
          slice_addr = plane_addr;
          plane_addr = get_skip_addr(plane_addr, my_tensor_desc.plane_skip);

          for(int sliceCnt=0; sliceCnt<my_tensor_desc.slice_num;sliceCnt++){
            cout << "sliceCnt = " << sliceCnt << endl;
            unit_addr  = slice_addr;
            slice_addr = get_skip_addr(slice_addr, my_tensor_desc.slice_skip);

            for(int unitCnt=0; unitCnt<my_tensor_desc.unit_num;unitCnt++){
              cout << "unitCnt = " << unitCnt << endl;
              int idx = cubeCnt*my_tensor_desc.plane_num*my_tensor_desc.slice_num*my_tensor_desc.unit_num + planeCnt*my_tensor_desc.slice_num*my_tensor_desc.unit_num + sliceCnt*my_tensor_desc.unit_num + unitCnt;

              cout << "cubeCnt= " << cubeCnt << "planeCnt = " << planeCnt << "sliceCnt = " << sliceCnt << "unitCnt = " << unitCnt << endl;
              cout << "idx = " << idx << ", unit_addr = " << dec << unit_addr << endl;
              traversalAddr[idx] = unit_addr;
              unit_addr += my_tensor_desc.unit_skip;
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

int main(){

  tensor myTensor;
  int *traversalAddr = myTensor.traversal_tensor_addr_point(myTensor.tensor_desc);
  int tensor_addr_point_num = myTensor.get_traversal_addr_point_num(myTensor.tensor_desc);
  myTensor.gen_sub_tensor();
  int sub_tensor_addr_point_num = myTensor.get_traversal_addr_point_num(myTensor.sub_tensor_desc);

  // for(int i=0;i<addr_point_num;i++){
  //   cout << "traversalAddr = " << traversalAddr[i] << endl;
  // }

  return 0;
}


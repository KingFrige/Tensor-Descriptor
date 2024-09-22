# README

## addr info

```
[5:0]   - byteIdx
[19:6]  - entryIdx
[22:20] - bankIdx
```


## function

- feature cube address generate 


## Tensor descriptor

### num/dim

  * byte number:  byteNum = 0 < byte number << 64
  * unit number:  unitNum
  * slice number: sliceNum
  * plane number: planeNum
  * cube number:  cubeNum

### skip/stride

  * unitSkip  = `1<<static_cast<int>(ceil(log2(tensorDesc.byteNum)))`
  * sliceSkip = unitNum  \* unitSkip;
  * planeSkip = sliceNum \* sliceSkip;
  * cubeSkip  = planeNum \* planeSkip;


![](docs/cube.svg)


## run

```bash
$ make
```

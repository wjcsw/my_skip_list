# my_skip_list
跳表是常见的非关系数据存储引擎的一种实现方式。在学习跳表的过程，学习开发了本项目。  
本项目采用C++实现了基于跳表的轻量级键值型存储引擎，包括插入数据、删除数据、查询数据、数据展示、数据存盘、文件加载数据，以及数据库大小显示等功能。  
基于随机读写压力测试结果，在跳表高度为15的情况下，该项目每秒可处理请求数（QPS）: 56.73w，每秒可处理读请求数（QPS）: 54.85w。  

## 项目结构
- skiplist.h 跳表结构和数据操作实现
- skiplist.cpp 使用跳表进行数据操作的实例
- stress_test.sh 随机读写压力测试脚本
- stress 压力测试目录
  - stress.cpp 压力测试程序代码
  - stress_test 生成的压力测试程序
- build Cmake 生成文件
- CMakeLists.txt Cmake配置文件

## 提供接口
- insert 插入数据
- erase 删除数据
- search 查询数据
- display 展示已存数据
- save 数据存盘
- load 加载数据
- size 返回存储数据条数

## 项目配置
`cd build`
`cmake ..`
`make`
`./skiplist`

## 压力测试
`sh stress_test.sh  //自动化执行 分别测试随机读写情况下10万条、50万条和100万条数据的耗时`
`stress/stress_test n`  //手动指定数据规模执行 n为测试数据条数

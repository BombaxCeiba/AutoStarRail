# 一、构建


###### 1.下载项目

```
git clone git@github.com:BombaxCeiba/AutoStarRail.git
```

###### 2.使用CMake配置

建议使用Visual Studio或安装了CMake插件的Visual Studio Code或CLion打开源码目录，这几个软件会自动开始配置项目。

在没有任何配置的情况下，不会生成任何其它语言的接口，也不会需要SWIG。项目会自动寻找环境中的Boost和OpenCV，若没有找到，会自动下载并配置为从源码编译。如果需要构建GUI，则需要Qt5，找不到则会直接报错。在任何情况下，都会从Github下载spdlog、nlohmann_json等库，因此需要有良好的网络环境。

以下是项目中CMake变量的定义

| 名称                    | 默认值             | 说明                                        |
| ----------------------- | ------------------ | ------------------------------------------- |
| EXPORT_JAVA             | OFF                | 是否导出Java接口                            |
| EXPORT_CSHARP           | OFF                | 是否导出C#接口                              |
| EXPORT_PYTHON           | OFF                | 是否导出Python接口                          |
| ASR_SPDLOG_USE_STD_FMT  | ON                 | 是否将项目自带的spdlog设置为使用标准库的fmt |
| BUILD_TEST              | OFF                | 是否构建测试                                |
| BUILD_QT5_GUI           | ON                 | 是否构建GUI                                 |
| USE_LLD                 | OFF                | 是否添加编译选项（非MSVC时）-fuse-ld=lld    |
| ASR_USE_CCACHE          | OFF                | 是否使用ccache                              |
| ASR_USE_BUNDLIED_BOOST  | OFF                | 是否下载Boost源码编译                       |
| ASR_USE_BUNDLIED_OPENCV | OFF                | 是否下载OpenCV源码编译                      |
| GITHUB_MIRROR_URL       | https://github.com |                                             |

###### 3.配置SWIG

需要导出其它语言接口时，需要使用SWIG，找不到SWIG时则会报错。可以通过-DSWIG_EXECUTABLE:Path=/Path/to/your/swig/executable来方便CMake寻找SWIG。

###### 4.备注

SWIG使用Windows版时，请勿使用MinGW版的CMake。CMake会选择错误的SWIG的lib路径，导致出现swig.swg not found的问题。

MSYS的GCC 13.1无法使用spdlog自带的fmt库通过编译

在Visual Studio Code中，可以通过编辑工作区内settings.json中的"cmake.configureArgs"来设置这些CMake变量。

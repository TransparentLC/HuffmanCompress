# HuffmanCompress

学习《数据结构》二叉树部分时，用 C 语言编写的一个使用 Huffman 编码对文件进行压缩和解压的小程序。

在 Visual Studio 2017 Community 下编译通过，没有使用预编译头 `pch.h`。打开工程文件的时候也许会因为这个而不能进行编译。

### 使用方法

压缩文件：

```
HuffmanCompress compress <InputFile> <OutputFile>
HuffmanCompress c <InputFile> <OutputFile>
```

解压文件：

```
HuffmanCompress decompress <InputFile> <OutputFile>
HuffmanCompress d <InputFile> <OutputFile>
```
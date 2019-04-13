#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//使用此项可以输出压缩和解压时用到的Huffman树等信息
#define DEBUG

//对位进行相关操作
//这里的unsigned char对于位是0或1，对于偏移是0到7
#pragma region Bit 

unsigned char ReadBit(unsigned char byte, unsigned char offset) {
    return (byte & (1 << offset)) >> offset;
}

void WriteBit(unsigned char *byte, unsigned char offset, unsigned char write) {
    *byte |= write << offset;
}

#pragma endregion

#pragma region Huffman

//动态分配数组存储H树
typedef struct {
    unsigned int weight; //字符的权
    unsigned int parent; //双亲的地址
    unsigned int lchild; //左子树的地址
    unsigned int rchild; //右子树的地址
} HTNode, *HuffmanTree;

//动态分配数组存储H树编码表
typedef char **HuffmanCode;

//在HT[1..range]的范围中找到parent=0，权最小的两项，下标分别为s1和s2，且s1<s2
void Select(HuffmanTree HT, unsigned int range, unsigned int *s1, unsigned int *s2) {
    unsigned int v1 = 4294967295, v2 = 4294967295; //unsigned int的最大值
    *s1 = 0;
    *s2 = 0;
    for (unsigned int i = 1; i <= range; i++) {
        if (HT[i].parent) {
            continue;
        }
        if (HT[i].weight < v1) {
            v2 = v1;
            v1 = HT[i].weight;
            *s2 = *s1;
            *s1 = i;
        } else if (HT[i].weight < v2) {
            v2 = HT[i].weight;
            *s2 = i;
        }
    }
    //保证s1<s2
    v1 = *s1;
    v2 = *s2;
    *s1 = (v1 < v2) ? v1 : v2;
    *s2 = (v1 > v2) ? v1 : v2;
}

//构造H树
void InitHuffmanTree(HuffmanTree *HT, unsigned int *w, unsigned int n) {
    unsigned int m = 2 * n - 1; //H树一共有2n-1个节点
    *HT = malloc((m + 1) * sizeof(HTNode)); //留一个0号结点表示指向空树，相当于二叉树中的NULL

    //为每个字符构建只有根结点，没有左右子树的二叉树
    (*HT)[0].weight = 0;
    (*HT)[0].parent = 0;
    (*HT)[0].lchild = 0;
    (*HT)[0].rchild = 0;
    HTNode *p = *HT + 1; //从1号结点开始
    unsigned int i = 1;
    while (i <= n) {
        (*p).weight = *w;
        (*p).parent = 0;
        (*p).lchild = 0;
        (*p).rchild = 0;
        i++;
        p++;
        w++;
    }
    //剩下的树是空树
    while (i <= m) {
        (*p).weight = 0;
        (*p).parent = 0;
        (*p).lchild = 0;
        (*p).rchild = 0;
        i++;
        p++;
    }

    //开始生成H树
    unsigned int s1, s2;
    for (i = n + 1; i <= m; i++) {
        Select(*HT, i - 1, &s1, &s2);
        (*HT)[s1].parent = i;
        (*HT)[s2].parent = i;
        (*HT)[i].lchild = s1;
        (*HT)[i].rchild = s2;
        (*HT)[i].weight = (*HT)[s1].weight + (*HT)[s2].weight;
    }
}

//根据H树求每个字符的编码
void InitHuffmanCode(HuffmanCode *HC, HuffmanTree HT, unsigned int n) {
    *HC = malloc((n + 1) * sizeof(char*)); //HC相当于一个由字符数组组成的数组，里面的元素类型是char*
    char *cd = malloc(n * sizeof(char)); //求编码的工作区，每个字符的编码长度一定不会超过n
    cd[n - 1] = '\0'; //字符数组以\0结尾
    unsigned int start; //表示一个编码从cd的哪一项开始
    unsigned int c, f; //c指当前指向的树，f是c的双亲

    //对每个字符逆向求编码
    for (unsigned int i = 1; i <= n; i++) {
        start = n - 1;
        //求编码从叶子结点开始，到c指向根结点为止，此时f为0
        for (c = i, f = HT[i].parent; f != 0; c = f, f = HT[f].parent) {
            start--; //向前移一位
            if (HT[f].lchild == c) {
                cd[start] = '0'; //左子树的路径用0表示
            } else {
                cd[start] = '1'; //右子树的路径用1表示
            }
        }
        //从cd复制到HC
        (*HC)[i] = malloc((n - start) * sizeof(char));
        strcpy_s((*HC)[i], n - start + 1, &cd[start]);
    }
    free(cd);
}

//HT存放构造的H树
//HC存放构造的编码
//w指向存放了每个字符的权的数组
//n是字符数量
//把建树和编码的部分分开了
void HuffmanCoding(HuffmanTree *HT, HuffmanCode *HC, unsigned int *w, unsigned int n) {
    void PrintHuffmanTree(HuffmanTree HT, unsigned int n);
    //只有一个或者没有字符还有什么编码的必要吗？
    if (n <= 1) {
        return;
    }

    InitHuffmanTree(HT, w, n);
    InitHuffmanCode(HC, *HT, n);
}

#pragma endregion

#ifdef DEBUG
//以表的形式输出对n个字符进行编码的HT
void PrintHuffmanTree(HuffmanTree HT, unsigned int n) {
    for (unsigned int i = 0; i <= 2 * n - 1; i++) {
        printf("%3d w:%10d p:%3d lc:%3d rc:%3d\n", i, HT[i].weight, HT[i].parent, HT[i].lchild, HT[i].rchild);
    }
}
#endif

//从文件中读取字节集信息
//字节种类数存在size，每个字节和它的出现次数存在set和weight
void LoadByteSet(FILE *fp, unsigned int *size, unsigned char **set, unsigned int **weight) {
    //先为0x00到0xFF的字节分配计数用存储空间，下标就是对应字节在文件中出现的次数
    unsigned int *FullWeight = calloc(256, sizeof(unsigned int));

    //获取文件大小
    fseek(fp, 0L, SEEK_END);
    unsigned long long FileSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    //不能用feof()作为判定条件，否则读到文件结尾会多出一个EOF(0xFFFFFFFF -> 0xFF)
    for (unsigned long long i = 0; i < FileSize; i++) {
        //fgetc()的类型居然是int？！
        //不转换类型的话，最后free()的时候就会出现HEAP CORRUPTION DETECTED before normal block...这种事情（doge）
        FullWeight[fgetc(fp)]++;
    }

    //求出一共出现了多少种字节
    *size = 0;
    for (unsigned int i = 0x00; i <= 0xFF; i++) {
        if (FullWeight[i]) {
            (*size)++;
        }
    }

    //输出字节和计数数据
    *set = malloc(*size * sizeof(unsigned char));
    *weight = malloc(*size * sizeof(unsigned int));
    unsigned char *setP = *set;
    unsigned int *weightP = *weight;
    for (unsigned int i = 0x00; i <= 0xFF; i++) {
        if (FullWeight[i]) {
            *setP = i;
            *weightP = FullWeight[i];
            setP++;
            weightP++;
        }
    }

    free(FullWeight);
}

int main(int argc, char *argv[]) {
    puts("Compress/Decompress a single file via Huffman Coding.");
    puts("https://github.com/TransparentLC/HuffmanCompress");
    puts("");

    HuffmanTree HT = NULL;
    HuffmanCode HC = NULL;

    unsigned int ByteSetSize; //字节集大小
    unsigned char *ByteSet = NULL; //字节
    unsigned int *ByteWeight = NULL; //每个字节对应的权
    FILE *fpComp = NULL, *fpDeComp = NULL; //已压缩和未压缩的文件
    unsigned long long FileSize, CodeSize; //进行编码时源文件的大小和编码的位数
    unsigned int Index; //指向的编码表索引
    unsigned char Offset, Byte, Read; //读写二进制位用
    unsigned char *pCode = NULL; //读取编码表用
    unsigned long long InputSize, OutputSize; //输入和输出文件的大小

    char *CompFileName;
    char *DeCompFileName;

    //argv[0]：程序本体的名字
    //argv[1]：操作类型：c/compress为压缩，d/decompress为解压
    //argv[2]：输入文件的路径
    //argv[3]：输出文件的路径
    //argv[1~3]不完整则直接输出使用说明

    if (argv[1] && argv[2] && argv[3] && (!strcmp(argv[1], "c") || !strcmp(argv[1], "compress"))) {
        //压缩部分
        //压缩步骤：建树，建表，存字符集，存树，存数据
        DeCompFileName = argv[2];
        CompFileName = argv[3];

        fopen_s(&fpDeComp, DeCompFileName, "rb");
        if (!fpDeComp) {
            puts("Input file does not exist.");
            return 0;
        }
        fopen_s(&fpComp, CompFileName, "wb");

        LoadByteSet(fpDeComp, &ByteSetSize, &ByteSet, &ByteWeight);

#ifdef DEBUG
        for (unsigned int i = 0; i < ByteSetSize; i++) {
            printf("0x%02X %d\n", ByteSet[i], ByteWeight[i]);
        }
#endif

        /* 建树建表 */
        HuffmanCoding(&HT, &HC, ByteWeight, ByteSetSize);

#ifdef DEBUG
        PrintHuffmanTree(HT, ByteSetSize);
        for (unsigned int i = 0; i < ByteSetSize; i++) {
            printf("0x%02X %s\n", ByteSet[i], HC[i + 1]);
        }
#endif

        /* 存字符集 */
        fwrite(&ByteSetSize, sizeof(ByteSetSize), 1, fpComp);
        fwrite(ByteSet, sizeof(unsigned char), ByteSetSize, fpComp);
        fwrite(ByteWeight, sizeof(unsigned int), ByteSetSize, fpComp);

        /* 存树 */
        fwrite(HT, sizeof(HTNode), 2 * ByteSetSize, fpComp);

        /* 存数据 */
        fseek(fpDeComp, 0L, SEEK_END);
        FileSize = ftell(fpDeComp);
        fseek(fpDeComp, 0L, SEEK_SET);
        CodeSize = 0;
        Offset = 7;
        Byte = 0;
        //对TextFile中的每个字符进行编码
        for (unsigned long long i = 0; i < FileSize; i++) {
            fread(&Read, sizeof(char), 1, fpDeComp);
            //从编码表中查找对应的编码
            for (unsigned int j = 0; j < ByteSetSize; j++) {
                if (ByteSet[j] == Read) {
                    pCode = HC[j + 1];
                    while (*pCode) {
                        CodeSize++;
                        WriteBit(&Byte, Offset, *pCode - '0');
                        if (Offset) {
                            //字节没有写满，offset-1
                            Offset--;
                        } else {
                            //字节写满了就写入文件，然后清空字节
                            Offset = 7;
                            fwrite(&Byte, sizeof(unsigned char), 1, fpComp);
                            Byte = 0;
                        }
                        pCode++;
                    }
                    break;
                }
            }
        }
        //最后一个字节没有写满也要写入，没有写满的位都是0
        if (Offset != 7) {
            fwrite(&Byte, sizeof(unsigned char), 1, fpComp);
        }
        //在文件最后写入编码位数
        fwrite(&CodeSize, sizeof(unsigned long long), 1, fpComp);

        /* 输出文件大小，统计压缩比 */
        fseek(fpDeComp, 0L, SEEK_END);
        InputSize = ftell(fpDeComp);
        fseek(fpComp, 0L, SEEK_END);
        OutputSize = ftell(fpComp);
        printf("Input (Source): %lld Bytes\nOutput (Compressed): %lld Bytes\nCompression ratio: %.2lf%%", InputSize, OutputSize, (double)OutputSize / (double)InputSize * (double)100);

        fclose(fpDeComp);
        fclose(fpComp);
    } else if (argv[1] && argv[2] && argv[3] && (!strcmp(argv[1], "d") || !strcmp(argv[1], "decompress"))) {
        //解压部分
        //解压步骤：读字符集，读树，建表，读数据
        CompFileName = argv[2];
        DeCompFileName = argv[3];

        fopen_s(&fpComp, CompFileName, "rb");
        if (!fpComp) {
            puts("Input file does not exist.");
            return 0;
        }
        fopen_s(&fpDeComp, DeCompFileName, "wb");

        /* 读字符集 */
        fread(&ByteSetSize, sizeof(unsigned int), 1, fpComp);
        ByteSet = malloc(ByteSetSize * sizeof(unsigned char));
        ByteWeight = malloc(ByteSetSize * sizeof(unsigned int));
        fread(ByteSet, sizeof(unsigned char), ByteSetSize, fpComp);
        fread(ByteWeight, sizeof(unsigned int), ByteSetSize, fpComp);

#ifdef DEBUG
        for (unsigned int i = 0; i < ByteSetSize; i++) {
            printf("0x%02X %d\n", ByteSet[i], ByteWeight[i]);
        }
#endif

        /* 读树 */
        HT = malloc(2 * ByteSetSize * sizeof(HTNode));
        fread(HT, sizeof(HTNode), 2 * ByteSetSize, fpComp);

        /* 建表 */
        InitHuffmanCode(&HC, HT, ByteSetSize);

#ifdef DEBUG
        PrintHuffmanTree(HT, ByteSetSize);
        for (unsigned int i = 0; i < ByteSetSize; i++) {
            printf("0x%02X %s\n", ByteSet[i], HC[i + 1]);
        }
#endif

        /* 读数据 */
        Index = 2 * ByteSetSize - 1;
        //读取编码位数
        fseek(fpComp, -1L * (long)sizeof(unsigned long long), SEEK_END);
        fread(&CodeSize, sizeof(unsigned long long), 1, fpComp);
        //跳过文件头部保存的字符集和H树的信息
        fseek(fpComp, (long)(sizeof(ByteSetSize) + ByteSetSize * (sizeof(unsigned char) + sizeof(unsigned int)) + 2 * ByteSetSize * sizeof(HTNode)), SEEK_SET);

        Offset = 7;
        fread(&Byte, sizeof(unsigned char), 1, fpComp); //先读取一个字节
        //对CodeFile中的每个位进行读取
        for (unsigned long long i = 0; i < CodeSize; i++) {
            Read = ReadBit(Byte, Offset);
            if (Read) {
                //读到1就移向右子树
                Index = HT[Index].rchild;
            } else {
                //读到0就移向右子树
                Index = HT[Index].lchild;
            }
            //移到叶子结点就写入字符，同时移回根结点
            if (!HT[Index].lchild && !HT[Index].rchild) {
                fwrite(&ByteSet[Index - 1], sizeof(unsigned char), 1, fpDeComp);
                Index = 2 * ByteSetSize - 1;
            }
            if (Offset) {
                //字节没有读完，offset-1
                Offset--;
            } else {
                //字节读完了就读下一个字节
                Offset = 7;
                fread(&Byte, sizeof(unsigned char), 1, fpComp);
            }
        }

        /* 输出文件大小，统计压缩比 */
        fseek(fpComp, 0L, SEEK_END);
        InputSize = ftell(fpComp);
        fseek(fpDeComp, 0L, SEEK_END);
        OutputSize = ftell(fpDeComp);
        printf("Input (Source): %lld Bytes\nOutput (Decompressed): %lld Bytes\nCompression ratio: %.2lf%%", InputSize, OutputSize, (double)InputSize / (double)OutputSize * (double)100);

        fclose(fpComp);
        fclose(fpDeComp);
    } else {
        //参数不完整则输出使用说明
        puts("Compress:");
        puts("HuffmanCompress compress <InputPath> <OutputPath>");
        puts("HuffmanCompress c <InputPath> <OutputPath>");
        puts("");
        puts("Decompress:");
        puts("HuffmanCompress decompress <InputPath> <OutputPath>");
        puts("HuffmanCompress d <InputPath> <OutputPath>");
    }
    return 0;
}
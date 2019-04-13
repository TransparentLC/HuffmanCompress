#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//ʹ�ô���������ѹ���ͽ�ѹʱ�õ���Huffman������Ϣ
#define DEBUG

//��λ������ز���
//�����unsigned char����λ��0��1������ƫ����0��7
#pragma region Bit 

unsigned char ReadBit(unsigned char byte, unsigned char offset) {
    return (byte & (1 << offset)) >> offset;
}

void WriteBit(unsigned char *byte, unsigned char offset, unsigned char write) {
    *byte |= write << offset;
}

#pragma endregion

#pragma region Huffman

//��̬��������洢H��
typedef struct {
    unsigned int weight; //�ַ���Ȩ
    unsigned int parent; //˫�׵ĵ�ַ
    unsigned int lchild; //�������ĵ�ַ
    unsigned int rchild; //�������ĵ�ַ
} HTNode, *HuffmanTree;

//��̬��������洢H�������
typedef char **HuffmanCode;

//��HT[1..range]�ķ�Χ���ҵ�parent=0��Ȩ��С������±�ֱ�Ϊs1��s2����s1<s2
void Select(HuffmanTree HT, unsigned int range, unsigned int *s1, unsigned int *s2) {
    unsigned int v1 = 4294967295, v2 = 4294967295; //unsigned int�����ֵ
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
    //��֤s1<s2
    v1 = *s1;
    v2 = *s2;
    *s1 = (v1 < v2) ? v1 : v2;
    *s2 = (v1 > v2) ? v1 : v2;
}

//����H��
void InitHuffmanTree(HuffmanTree *HT, unsigned int *w, unsigned int n) {
    unsigned int m = 2 * n - 1; //H��һ����2n-1���ڵ�
    *HT = malloc((m + 1) * sizeof(HTNode)); //��һ��0�Ž���ʾָ��������൱�ڶ������е�NULL

    //Ϊÿ���ַ�����ֻ�и���㣬û�����������Ķ�����
    (*HT)[0].weight = 0;
    (*HT)[0].parent = 0;
    (*HT)[0].lchild = 0;
    (*HT)[0].rchild = 0;
    HTNode *p = *HT + 1; //��1�Ž�㿪ʼ
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
    //ʣ�µ����ǿ���
    while (i <= m) {
        (*p).weight = 0;
        (*p).parent = 0;
        (*p).lchild = 0;
        (*p).rchild = 0;
        i++;
        p++;
    }

    //��ʼ����H��
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

//����H����ÿ���ַ��ı���
void InitHuffmanCode(HuffmanCode *HC, HuffmanTree HT, unsigned int n) {
    *HC = malloc((n + 1) * sizeof(char*)); //HC�൱��һ�����ַ�������ɵ����飬�����Ԫ��������char*
    char *cd = malloc(n * sizeof(char)); //�����Ĺ�������ÿ���ַ��ı��볤��һ�����ᳬ��n
    cd[n - 1] = '\0'; //�ַ�������\0��β
    unsigned int start; //��ʾһ�������cd����һ�ʼ
    unsigned int c, f; //cָ��ǰָ�������f��c��˫��

    //��ÿ���ַ����������
    for (unsigned int i = 1; i <= n; i++) {
        start = n - 1;
        //������Ҷ�ӽ�㿪ʼ����cָ������Ϊֹ����ʱfΪ0
        for (c = i, f = HT[i].parent; f != 0; c = f, f = HT[f].parent) {
            start--; //��ǰ��һλ
            if (HT[f].lchild == c) {
                cd[start] = '0'; //��������·����0��ʾ
            } else {
                cd[start] = '1'; //��������·����1��ʾ
            }
        }
        //��cd���Ƶ�HC
        (*HC)[i] = malloc((n - start) * sizeof(char));
        strcpy_s((*HC)[i], n - start + 1, &cd[start]);
    }
    free(cd);
}

//HT��Ź����H��
//HC��Ź���ı���
//wָ������ÿ���ַ���Ȩ������
//n���ַ�����
//�ѽ����ͱ���Ĳ��ַֿ���
void HuffmanCoding(HuffmanTree *HT, HuffmanCode *HC, unsigned int *w, unsigned int n) {
    void PrintHuffmanTree(HuffmanTree HT, unsigned int n);
    //ֻ��һ������û���ַ�����ʲô����ı�Ҫ��
    if (n <= 1) {
        return;
    }

    InitHuffmanTree(HT, w, n);
    InitHuffmanCode(HC, *HT, n);
}

#pragma endregion

#ifdef DEBUG
//�Ա����ʽ�����n���ַ����б����HT
void PrintHuffmanTree(HuffmanTree HT, unsigned int n) {
    for (unsigned int i = 0; i <= 2 * n - 1; i++) {
        printf("%3d w:%10d p:%3d lc:%3d rc:%3d\n", i, HT[i].weight, HT[i].parent, HT[i].lchild, HT[i].rchild);
    }
}
#endif

//���ļ��ж�ȡ�ֽڼ���Ϣ
//�ֽ�����������size��ÿ���ֽں����ĳ��ִ�������set��weight
void LoadByteSet(FILE *fp, unsigned int *size, unsigned char **set, unsigned int **weight) {
    //��Ϊ0x00��0xFF���ֽڷ�������ô洢�ռ䣬�±���Ƕ�Ӧ�ֽ����ļ��г��ֵĴ���
    unsigned int *FullWeight = calloc(256, sizeof(unsigned int));

    //��ȡ�ļ���С
    fseek(fp, 0L, SEEK_END);
    unsigned long long FileSize = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    //������feof()��Ϊ�ж���������������ļ���β����һ��EOF(0xFFFFFFFF -> 0xFF)
    for (unsigned long long i = 0; i < FileSize; i++) {
        //fgetc()�����;�Ȼ��int����
        //��ת�����͵Ļ������free()��ʱ��ͻ����HEAP CORRUPTION DETECTED before normal block...�������飨doge��
        FullWeight[fgetc(fp)]++;
    }

    //���һ�������˶������ֽ�
    *size = 0;
    for (unsigned int i = 0x00; i <= 0xFF; i++) {
        if (FullWeight[i]) {
            (*size)++;
        }
    }

    //����ֽںͼ�������
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

    unsigned int ByteSetSize; //�ֽڼ���С
    unsigned char *ByteSet = NULL; //�ֽ�
    unsigned int *ByteWeight = NULL; //ÿ���ֽڶ�Ӧ��Ȩ
    FILE *fpComp = NULL, *fpDeComp = NULL; //��ѹ����δѹ�����ļ�
    unsigned long long FileSize, CodeSize; //���б���ʱԴ�ļ��Ĵ�С�ͱ����λ��
    unsigned int Index; //ָ��ı��������
    unsigned char Offset, Byte, Read; //��д������λ��
    unsigned char *pCode = NULL; //��ȡ�������
    unsigned long long InputSize, OutputSize; //���������ļ��Ĵ�С

    char *CompFileName;
    char *DeCompFileName;

    //argv[0]�������������
    //argv[1]���������ͣ�c/compressΪѹ����d/decompressΪ��ѹ
    //argv[2]�������ļ���·��
    //argv[3]������ļ���·��
    //argv[1~3]��������ֱ�����ʹ��˵��

    if (argv[1] && argv[2] && argv[3] && (!strcmp(argv[1], "c") || !strcmp(argv[1], "compress"))) {
        //ѹ������
        //ѹ�����裺�������������ַ�����������������
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

        /* �������� */
        HuffmanCoding(&HT, &HC, ByteWeight, ByteSetSize);

#ifdef DEBUG
        PrintHuffmanTree(HT, ByteSetSize);
        for (unsigned int i = 0; i < ByteSetSize; i++) {
            printf("0x%02X %s\n", ByteSet[i], HC[i + 1]);
        }
#endif

        /* ���ַ��� */
        fwrite(&ByteSetSize, sizeof(ByteSetSize), 1, fpComp);
        fwrite(ByteSet, sizeof(unsigned char), ByteSetSize, fpComp);
        fwrite(ByteWeight, sizeof(unsigned int), ByteSetSize, fpComp);

        /* ���� */
        fwrite(HT, sizeof(HTNode), 2 * ByteSetSize, fpComp);

        /* ������ */
        fseek(fpDeComp, 0L, SEEK_END);
        FileSize = ftell(fpDeComp);
        fseek(fpDeComp, 0L, SEEK_SET);
        CodeSize = 0;
        Offset = 7;
        Byte = 0;
        //��TextFile�е�ÿ���ַ����б���
        for (unsigned long long i = 0; i < FileSize; i++) {
            fread(&Read, sizeof(char), 1, fpDeComp);
            //�ӱ�����в��Ҷ�Ӧ�ı���
            for (unsigned int j = 0; j < ByteSetSize; j++) {
                if (ByteSet[j] == Read) {
                    pCode = HC[j + 1];
                    while (*pCode) {
                        CodeSize++;
                        WriteBit(&Byte, Offset, *pCode - '0');
                        if (Offset) {
                            //�ֽ�û��д����offset-1
                            Offset--;
                        } else {
                            //�ֽ�д���˾�д���ļ���Ȼ������ֽ�
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
        //���һ���ֽ�û��д��ҲҪд�룬û��д����λ����0
        if (Offset != 7) {
            fwrite(&Byte, sizeof(unsigned char), 1, fpComp);
        }
        //���ļ����д�����λ��
        fwrite(&CodeSize, sizeof(unsigned long long), 1, fpComp);

        /* ����ļ���С��ͳ��ѹ���� */
        fseek(fpDeComp, 0L, SEEK_END);
        InputSize = ftell(fpDeComp);
        fseek(fpComp, 0L, SEEK_END);
        OutputSize = ftell(fpComp);
        printf("Input (Source): %lld Bytes\nOutput (Compressed): %lld Bytes\nCompression ratio: %.2lf%%", InputSize, OutputSize, (double)OutputSize / (double)InputSize * (double)100);

        fclose(fpDeComp);
        fclose(fpComp);
    } else if (argv[1] && argv[2] && argv[3] && (!strcmp(argv[1], "d") || !strcmp(argv[1], "decompress"))) {
        //��ѹ����
        //��ѹ���裺���ַ���������������������
        CompFileName = argv[2];
        DeCompFileName = argv[3];

        fopen_s(&fpComp, CompFileName, "rb");
        if (!fpComp) {
            puts("Input file does not exist.");
            return 0;
        }
        fopen_s(&fpDeComp, DeCompFileName, "wb");

        /* ���ַ��� */
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

        /* ���� */
        HT = malloc(2 * ByteSetSize * sizeof(HTNode));
        fread(HT, sizeof(HTNode), 2 * ByteSetSize, fpComp);

        /* ���� */
        InitHuffmanCode(&HC, HT, ByteSetSize);

#ifdef DEBUG
        PrintHuffmanTree(HT, ByteSetSize);
        for (unsigned int i = 0; i < ByteSetSize; i++) {
            printf("0x%02X %s\n", ByteSet[i], HC[i + 1]);
        }
#endif

        /* ������ */
        Index = 2 * ByteSetSize - 1;
        //��ȡ����λ��
        fseek(fpComp, -1L * (long)sizeof(unsigned long long), SEEK_END);
        fread(&CodeSize, sizeof(unsigned long long), 1, fpComp);
        //�����ļ�ͷ��������ַ�����H������Ϣ
        fseek(fpComp, (long)(sizeof(ByteSetSize) + ByteSetSize * (sizeof(unsigned char) + sizeof(unsigned int)) + 2 * ByteSetSize * sizeof(HTNode)), SEEK_SET);

        Offset = 7;
        fread(&Byte, sizeof(unsigned char), 1, fpComp); //�ȶ�ȡһ���ֽ�
        //��CodeFile�е�ÿ��λ���ж�ȡ
        for (unsigned long long i = 0; i < CodeSize; i++) {
            Read = ReadBit(Byte, Offset);
            if (Read) {
                //����1������������
                Index = HT[Index].rchild;
            } else {
                //����0������������
                Index = HT[Index].lchild;
            }
            //�Ƶ�Ҷ�ӽ���д���ַ���ͬʱ�ƻظ����
            if (!HT[Index].lchild && !HT[Index].rchild) {
                fwrite(&ByteSet[Index - 1], sizeof(unsigned char), 1, fpDeComp);
                Index = 2 * ByteSetSize - 1;
            }
            if (Offset) {
                //�ֽ�û�ж��꣬offset-1
                Offset--;
            } else {
                //�ֽڶ����˾Ͷ���һ���ֽ�
                Offset = 7;
                fread(&Byte, sizeof(unsigned char), 1, fpComp);
            }
        }

        /* ����ļ���С��ͳ��ѹ���� */
        fseek(fpComp, 0L, SEEK_END);
        InputSize = ftell(fpComp);
        fseek(fpDeComp, 0L, SEEK_END);
        OutputSize = ftell(fpDeComp);
        printf("Input (Source): %lld Bytes\nOutput (Decompressed): %lld Bytes\nCompression ratio: %.2lf%%", InputSize, OutputSize, (double)InputSize / (double)OutputSize * (double)100);

        fclose(fpComp);
        fclose(fpDeComp);
    } else {
        //���������������ʹ��˵��
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
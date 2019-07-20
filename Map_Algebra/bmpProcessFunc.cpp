#include "pch.h"
#include <Windows.h>
#include "bmpProcessFunc.h"
#include<stdio.h>
#include <assert.h>
#include <vector>
#define MaxFloat 9999999999.999999
#define MinFloat 0.000001
using namespace std;

/***   1.反色处理   ***/
bool BmpReverse(const char* SrcBmpName, const char* DestBmpName)
{
	// 1. 打开源文件
	FILE *file = fopen(SrcBmpName, "rb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}

	// 2. 读取位图基本信息
	BITMAPFILEHEADER BmpFileHeader;
	BITMAPINFOHEADER BmpInfoHeader;
	unsigned char ClrTab[256 * 4];

	fread(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	fread(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);
	fread(ClrTab, 1024, 1, file);

	unsigned MtxWidth, MtxHeight, BmpWidth;
	BmpWidth = BmpInfoHeader.biWidth;
	MtxWidth = (BmpWidth + 3) / 4 * 4;
	MtxHeight = BmpInfoHeader.biHeight;

	// 3. 读取位图矩阵数据
	unsigned char * LineBuf = new unsigned char[MtxWidth]; // 读数据行缓冲

	unsigned char ** MtxBuf = new unsigned char *[MtxHeight]; // 写数据矩阵缓冲
	for (int i = 0; i < MtxHeight; i++)
	{
		MtxBuf[i] = new unsigned char[MtxWidth];
		for (int j = 0; j < MtxWidth; j++)
		{
			MtxBuf[i][j] = 0xFF;
		}
	}


	// 4. 反色处理
	for (int i = 0; i < MtxHeight; i++)
	{
		fread(LineBuf, sizeof(unsigned char), MtxWidth, file);
		for (int j = 0; j < MtxWidth; j++)
		{
			if (LineBuf[j] == 0xFF) continue;
			MtxBuf[i][j] = 255 - LineBuf[j];
		}
	}
	// 5. 新建结果位图文件
	FILE *file1 = fopen(DestBmpName, "wb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}
	fwrite(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file1);
	fwrite(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file1);
	fwrite(ClrTab, 1024, 1, file1);

	// 6. 写入结果
	for (int i = 0; i < MtxHeight; i++)
	{
		fwrite(MtxBuf[i], MtxWidth, 1, file1);
	}

	// 7. 清理内存、关闭文件
	fclose(file);
	fclose(file1);
	for (int i = 0; i < MtxHeight; i++)
	{
		delete[] MtxBuf[i];
		MtxBuf[i] = NULL;
	}
	delete[] MtxBuf;
	MtxBuf = NULL;

	delete[] LineBuf;
	LineBuf = NULL;

	return true;
};

/***   2.叠置处理   ***/
bool BmpOverlay(const char* SrcBmpName1, const char* SrcBmpName2, const char* DestBmpName) 
{
	// 1 打开源文件
	FILE *file1 = fopen(SrcBmpName1, "rb");
	FILE *file2 = fopen(SrcBmpName2, "rb");
	if (file1 == NULL||file2 == NULL)
	{
		printf("file not found!");
		return false;
	}

	// 2.1 读取位图1基本信息
	BITMAPFILEHEADER BmpFileHeader1;
	BITMAPINFOHEADER BmpInfoHeader1;
	unsigned char ClrTab1[256 * 4]; //调色板

	fread(&BmpFileHeader1, sizeof(BITMAPFILEHEADER), 1, file1);
	fread(&BmpInfoHeader1, sizeof(BITMAPINFOHEADER), 1, file1);
	fread(ClrTab1, 1024, 1, file1);
	unsigned MtxWidth1, MtxHeight1, BmpWidth1;
	BmpWidth1 = BmpInfoHeader1.biWidth;
	MtxWidth1 = (BmpWidth1 + 3) / 4 * 4;
	MtxHeight1 = BmpInfoHeader1.biHeight;

	//2.2  读取位图2基本信息

	BITMAPFILEHEADER BmpFileHeader2;
	BITMAPINFOHEADER BmpInfoHeader2;
	unsigned char ClrTab2[256 * 4]; //调色板

	fread(&BmpFileHeader2, sizeof(BITMAPFILEHEADER), 1, file2);
	fread(&BmpInfoHeader2, sizeof(BITMAPINFOHEADER), 1, file2);
	fread(ClrTab2, 1024, 1, file2);
	unsigned MtxWidth2, MtxHeight2, BmpWidth2;
	BmpWidth2 = BmpInfoHeader2.biWidth;
	MtxWidth2 = (BmpWidth2 + 3) / 4 * 4;
	MtxHeight2 = BmpInfoHeader2.biHeight;

	//3.  叠置
	if (MtxHeight1 == MtxHeight2 && BmpWidth1 == BmpWidth2)
	{

		unsigned char *LineBuf1 = new unsigned char[MtxWidth1];
		unsigned char *LineBuf2 = new unsigned char[MtxWidth1];
		unsigned char **destBmpMtx = new unsigned char *[MtxHeight1];

		unsigned i, j;
		for (i = 0; i < MtxHeight1; i++)
		{
			fread(LineBuf1, MtxWidth1, 1, file1);
			fread(LineBuf2, MtxWidth1, 1, file2);

			destBmpMtx[i] = new unsigned char[MtxWidth1];

			for (j = 0; j < MtxWidth1; j++)
			{
				if (LineBuf1[j] == 0xFF && LineBuf2[j] == 0xFF)
					destBmpMtx[i][j] = 0xFF;
				else if (LineBuf1[j] != 0xFF && LineBuf2[j] == 0xFF)
					destBmpMtx[i][j] = LineBuf1[j];
				else if (LineBuf1[j] == 0xFF && LineBuf2[j] != 0xFF)
					destBmpMtx[i][j] = LineBuf2[j];
				else
					destBmpMtx[i][j] = LineBuf1[j] + LineBuf2[j];
			}
		}

	// 5. 新建结果位图文件
	FILE *file3 = fopen(DestBmpName, "wb");
	if (file3 == NULL)
	{
		printf("file not found!");
		return false;
	}
	fwrite(&BmpFileHeader1, sizeof(BITMAPFILEHEADER), 1, file3);
	fwrite(&BmpInfoHeader1, sizeof(BITMAPINFOHEADER), 1, file3);
	fwrite(ClrTab1, 1024, 1, file3);

	// 6. 写入结果

	for (i = 0; i < MtxHeight1; i++)
	{
		fwrite(destBmpMtx[i], MtxWidth1, 1, file3);
		delete[]destBmpMtx[i];
		destBmpMtx[i] = NULL;
	}

	// 7. 清理内存、关闭文件
	delete[]destBmpMtx;
	delete[]LineBuf1;
	LineBuf1 = NULL;
	delete[]LineBuf2;
	LineBuf2 = NULL;

	fclose(file3);
	}
	fclose(file1);
	fclose(file2);

	return true;

};

/***   3.平滑处理   ***/
bool BmpSmooth(const char* SrcBmpName, CDistanceTemplet *pTemplet, const char* DestBmpName)
{
	// 1. 打开源文件
	FILE *file = fopen(SrcBmpName, "rb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}

	// 2. 读取位图基本信息
	BITMAPFILEHEADER BmpFileHeader;
	BITMAPINFOHEADER BmpInfoHeader;
	unsigned char ClrTab[256 * 4];

	fread(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	fread(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);
	fread(ClrTab, 1024, 1, file);

	unsigned MtxWidth, MtxHeight, BmpWidth;
	BmpWidth = BmpInfoHeader.biWidth;
	MtxWidth = (BmpWidth + 3) / 4 * 4;
	MtxHeight = BmpInfoHeader.biHeight;

	// 3. 读取位图矩阵数据
	unsigned char * LineBuf = new unsigned char[MtxWidth]; // 读数据行缓冲

	unsigned char ** MtxBuf = new unsigned char *[MtxHeight]; // 写数据矩阵缓冲

	unsigned char ** MtxBuf2 = new unsigned char *[MtxHeight]; 

	for (int i = 0; i < MtxHeight; i++)
	{
		MtxBuf[i]  = new unsigned char[MtxWidth];
		MtxBuf2[i] = new unsigned char[MtxWidth];
		fread(LineBuf, sizeof(unsigned char), MtxWidth, file);
		for (int j = 0; j < MtxWidth; j++)
		{
			MtxBuf[i][j]  = LineBuf[j];
			MtxBuf2[i][j] = 0xff;
		}
	}

	// 4. 平滑处理
	for (int y = 0; y < MtxHeight; y++) // 2重循环，对每一个栅格
	{
		for (int x = 0; x < MtxWidth; x++)
		{
			if (MtxBuf[y][x]== 0xff) continue;        
			unsigned char tmp = 0;
			for (unsigned k = 0; k < pTemplet->GetSize(); k++) // 对上半模板逐个元素循环
			{
				char offx = pTemplet->Get0ffX(k);
				char offy = pTemplet->Get0ffY(k);
				float tmpDis = pTemplet->GetDis(k);     // 去除模板要素，x,y偏移及对应模板相对距离

				int CurrX = x + offx;              // 计算模板元素对应的栅格位置
				int CurrY = y + offy;
				if ((CurrX < 0) || (CurrX > MtxWidth - 1)) continue;  // 判断该位置是否越界
				if ((CurrY < 0) || (CurrY > MtxHeight - 1)) continue;
				unsigned char MtxValue = MtxBuf[CurrY][CurrX];
				tmp += MtxValue * tmpDis;
			}
			MtxBuf2[y][x] = tmp / pTemplet->GetSize();
		}
	}

	// 5. 新建结果位图文件
	FILE *file1 = fopen(DestBmpName, "wb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}
	fwrite(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file1);
	fwrite(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file1);
	fwrite(ClrTab, 1024, 1, file1);

	// 6. 写入结果
	for (int i = 0; i < MtxHeight; i++)
	{
		fwrite(MtxBuf2[i], 1, MtxWidth, file1);
	}

	// 7. 清理内存、关闭文件
	fclose(file);
	fclose(file1);
	for (int i = 0; i < MtxHeight; i++)
	{
		delete[] MtxBuf[i];
		delete[] MtxBuf2[i];
		MtxBuf[i]  = NULL;
		MtxBuf2[i] = NULL;
	}
	delete[] MtxBuf;
	delete[] MtxBuf2;
	MtxBuf  = NULL;
	MtxBuf2 = NULL;

	delete[] LineBuf;
	LineBuf = NULL;

	return true;


};


int Bmp256to32b(const char* SourceFileName, const char* IdxFileName)  //将源256位图转为 32bit 位图
{
	unsigned i, j; // 循环变量
	// *1 检验源文件
	assert(SourceFileName != NULL);

	FILE* SrcFileStream = fopen(SourceFileName, "rb");
	if (SrcFileStream == NULL) return -1; // 出错代码1，文件不存在

	BITMAPFILEHEADER SrcFileHead;
	BITMAPINFOHEADER SrcFileInfo;
	unsigned char ColorIdx[1024];

	if (fread(&SrcFileHead, sizeof(BITMAPFILEHEADER), 1, SrcFileStream) != 1)
	{
		fclose(SrcFileStream);
		return -1;
	}
	if (fread(&SrcFileInfo, sizeof(BITMAPINFOHEADER), 1, SrcFileStream) != 1)
	{
		fclose(SrcFileStream);
		return -1;
	}
	if (fread(ColorIdx, 1024, 1, SrcFileStream) != 1)
	{
		fclose(SrcFileStream);
		return -1;
	}

	if (SrcFileInfo.biBitCount != 8) // 256色图处理
	{
		fclose(SrcFileStream);
		return -2; // 出错代码2，文件格式不合要求
	}


	// *2 编制实体索引
	BITMAPFILEHEADER IdxFileHead;
	BITMAPINFOHEADER IdxFileInfo;
	int LineBytes = (SrcFileInfo.biWidth + 3) / 4 * 4;
	int LineBytes2 = SrcFileInfo.biWidth;

	IdxFileHead.bfType = SrcFileHead.bfType;
	IdxFileHead.bfSize = 54 + LineBytes2 * 4 * SrcFileInfo.biHeight;//====================================
	IdxFileHead.bfReserved1 = 0;
	IdxFileHead.bfReserved2 = 0;
	IdxFileHead.bfOffBits = 54;  //====================================

	IdxFileInfo.biSize = 40;
	IdxFileInfo.biWidth = SrcFileInfo.biWidth;  //====================================
	IdxFileInfo.biHeight = SrcFileInfo.biHeight;//====================================
	IdxFileInfo.biPlanes = 1;
	IdxFileInfo.biBitCount = 32;             //====================================
	IdxFileInfo.biCompression = 0;
	IdxFileInfo.biSizeImage = 0;
	IdxFileInfo.biXPelsPerMeter = 0;
	IdxFileInfo.biYPelsPerMeter = 0;
	IdxFileInfo.biClrUsed = 0;
	IdxFileInfo.biClrImportant = 0;


	// *2.1 构造原始矩阵
	unsigned char * pSrcCellLineBuf = new unsigned char[LineBytes];   // 源文件行缓冲
	unsigned **pIdxCellMatixBuf = new unsigned*[IdxFileInfo.biHeight]; //索引矩阵
	for (i = 0; i < IdxFileInfo.biHeight; i++)
	{
		fseek(SrcFileStream, SrcFileHead.bfOffBits + i * sizeof(char)*LineBytes, 0);
		fread(pSrcCellLineBuf, sizeof(char), LineBytes, SrcFileStream);
		pIdxCellMatixBuf[i] = new unsigned[LineBytes2];
		for (j = 0; j < LineBytes2; j++)
			if (pSrcCellLineBuf[j] == 0xFF) pIdxCellMatixBuf[i][j] = 0xFFFFFFFF;
			else
			{
				int k = pSrcCellLineBuf[j];
				unsigned uintTmp = k;
				/*  取对应颜色*/

				uintTmp = ColorIdx[k * 4];
				uintTmp <<= 8;
				uintTmp += ColorIdx[k * 4 + 1];
				uintTmp <<= 8;
				uintTmp += ColorIdx[k * 4 + 2];
				uintTmp <<= 8;
				uintTmp += ColorIdx[k * 4 + 3];

				pIdxCellMatixBuf[i][j] = uintTmp;
			}
	}

	FILE *ObjIdxFile = fopen(IdxFileName, "wb");
	if (ObjIdxFile == NULL) return -3; // 出错代码3，实体索引矩阵文件无法创建
	fseek(ObjIdxFile, 0, 0);
	fwrite(&IdxFileHead, sizeof(BITMAPFILEHEADER), 1, ObjIdxFile);
	fwrite(&IdxFileInfo, sizeof(BITMAPINFOHEADER), 1, ObjIdxFile);
	for (i = 0; i < IdxFileInfo.biHeight; i++)
	{
		fseek(ObjIdxFile, 54 + i * sizeof(unsigned)*LineBytes2, 0);
		fwrite(pIdxCellMatixBuf[i], sizeof(unsigned), LineBytes2, ObjIdxFile);
		delete[] pIdxCellMatixBuf[i];
		pIdxCellMatixBuf[i] = NULL;
	}

	delete[] pIdxCellMatixBuf;
	delete[] pSrcCellLineBuf;

	fclose(ObjIdxFile);
	fclose(SrcFileStream);
	return 1; // 返回成功代码1
};


int TmpDistanceTransform32b(const char *SrcBmpName, CDistanceTemplet *pTemplet, const char *DisBmpName, const char *LocBmpName, unsigned char MaxColor)
{
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	char ColorTAB[1024];

	FILE *bmpFile = NULL;
	bmpFile = fopen(SrcBmpName, "rb");
	fseek(bmpFile, 0, SEEK_SET);

	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, bmpFile);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, bmpFile);
	fread(ColorTAB, 1024, 1, bmpFile);

	unsigned Height, Width;
	Height = infoHeader.biHeight;
	Width = infoHeader.biWidth;
	unsigned widthFile = (Width + 3) / 4 * 4;

	// 3. 建立初始 距离场、 分配场
	unsigned char **LocMatix = NULL;
	LocMatix = new unsigned char *[Height];

	float **DisMatix = NULL;
	DisMatix = new float *[Height];
	for (unsigned i = 0; i < Height; i++)
	{
		LocMatix[i] = new unsigned char[widthFile];
		// 3.1 分配场数据直接从位图读取
		fread(LocMatix[i], 1, widthFile, bmpFile);
		DisMatix[i] = new float[Width];
		// 3.2 根据分配场的实体像素分布初始化距离场
		for (unsigned j = 0; j < Width; j++)
		{
			if (LocMatix[i][j] == MaxColor) DisMatix[i][j] = MaxFloat;// 无穷大
			else DisMatix[i][j] = 0;
		}
	}
	fclose(bmpFile);

	// 4. 模板距离变换
	// 4.1 从左下到右上方向
	for (int y = 0; y < Height; y++) // 2重循环，对每一个栅格
	{
		for (int x = 0; x < Width; x++)
		{
			float tmpMin = DisMatix[y][x] + 0; // 搜索模板最小值初始化
			if (fabs(tmpMin) < MinFloat) continue;             // 为0，表示黑点无需计算
			for (unsigned k = 0; k < pTemplet->GetSize() / 2; k++) // 对上半模板逐个元素循环
			{
				char offx = pTemplet->Get0ffX(k);
				char offy = pTemplet->Get0ffY(k);
				float tmpDis = pTemplet->GetDis(k);     // 去除模板要素，x,y偏移及对应模板相对距离

				int CurrX = x + offx;              // 计算模板元素对应的栅格位置
				int CurrY = y + offy;
				if ((CurrX < 0) || (CurrX > Width - 1)) continue;  // 判断该位置是否越界
				if ((CurrY < 0) || (CurrY > Height - 1)) continue;

				if (fabs(DisMatix[CurrY][CurrX] - MaxFloat) < MinFloat) continue; // 该位置如果是无穷大不可计算

				float dis1 = DisMatix[CurrY][CurrX] + tmpDis; // 当前栅格距离+模板相对距离
				if (dis1 < tmpMin)  // 求最小值
				{
					tmpMin = dis1;
					DisMatix[y][x] = dis1;                   // 最小距离填入距离矩阵
					LocMatix[y][x] = LocMatix[CurrY][CurrX]; // 最小距离来源填入分配矩阵
				}
			}
		}
	}
	// 4.2 从右上到左下方向
	for (int y = Height - 1; y > 0; y--) // 2重循环，对每一个栅格
	{
		for (int x = Width - 1; x > 0; x--)
		{
			float tmpMin = DisMatix[y][x] + 0; // 搜索模板最小值初始化
			if (fabs(tmpMin) < MinFloat) continue;             // 为0，表示黑点无需计算
			for (unsigned k = pTemplet->GetSize(); k > pTemplet->GetSize() / 2; k--) // 对下半模板逐个元素循环
			{
				char offx = pTemplet->Get0ffX(k);
				char offy = pTemplet->Get0ffY(k);
				float tmpDis = pTemplet->GetDis(k);     // 去除模板要素，x,y偏移及对应模板相对距离

				int CurrX = x + offx;              // 计算模板元素对应的栅格位置
				int CurrY = y + offy;
				if ((CurrX < 0) || (CurrX > Width - 1)) continue;  // 判断该位置是否越界
				if ((CurrY < 0) || (CurrY > Height - 1)) continue;

				if (fabs(DisMatix[CurrY][CurrX] - MaxFloat) < MinFloat) continue; // 该位置如果是无穷大不可计算

				float dis1 = DisMatix[CurrY][CurrX] + tmpDis; // 当前栅格距离+模板相对距离
				if (dis1 < tmpMin)  // 求最小值
				{
					tmpMin = dis1;
					DisMatix[y][x] = dis1;                   // 最小距离填入距离矩阵
					LocMatix[y][x] = LocMatix[CurrY][CurrX]; // 最小距离来源填入分配矩阵
				}
			}
		}
	}

	// 5. 写入结果文件
	// 5.1 写入距离矩阵32位float型
	BITMAPFILEHEADER IdxFileHead; // 32位位图头结构
	BITMAPINFOHEADER IdxFileInfo;

	IdxFileHead.bfType = fileHeader.bfType;  //位图类别
	IdxFileHead.bfSize = 54 + infoHeader.biWidth * 4 * infoHeader.biHeight;  //BMP图像文件的大小
	IdxFileHead.bfReserved1 = 0;  //位图文件保留字，必须为0(6-7字节) 
	IdxFileHead.bfReserved2 = 0;  //位图文件保留字，必须为0(7-9字节)
	IdxFileHead.bfOffBits = 54;  //BMP图像数据的地址

	IdxFileInfo.biSize = 40;  //本结构的大小，根据不同的操作系统而不同，在Windows中，此字段的值总为28h字节=40字节
	IdxFileInfo.biWidth = infoHeader.biWidth;  //BMP图像的宽度
	IdxFileInfo.biHeight = infoHeader.biHeight;  //BMP图像的高度
	IdxFileInfo.biPlanes = 1;  //目标设备的级别，必须为1
	IdxFileInfo.biBitCount = 32;  //BMP图像的色深，即一个像素用多少位表示，常见有1、4、8、16、24和32
	IdxFileInfo.biCompression = 0;  //压缩方式，0表示不压缩，1表示RLE8压缩，2表示RLE4压缩，3表示每个像素值由指定的掩码决定
	IdxFileInfo.biSizeImage = 0;  //BMP图像数据大小，必须是4的倍数，图像数据大小不是4的倍数时用0填充补足
	IdxFileInfo.biXPelsPerMeter = 0;  //水平分辨率，单位像素/m
	IdxFileInfo.biYPelsPerMeter = 0;  //垂直分辨率，单位像素/m
	IdxFileInfo.biClrUsed = 0;  //BMP图像使用的颜色，0表示使用全部颜色
	IdxFileInfo.biClrImportant = 0;  //重要的颜色数，此值为0时所有颜色都重要

	FILE * bmpWrite = fopen(DisBmpName, "wb");
	fwrite(&IdxFileHead, sizeof(BITMAPFILEHEADER), 1, bmpWrite);
	fwrite(&IdxFileInfo, sizeof(BITMAPINFOHEADER), 1, bmpWrite);
	for (int y = 0; y < Height; y++)
	{
		fwrite(DisMatix[y], sizeof(float), Width, bmpWrite);
	}
	fclose(bmpWrite);

	// 5.2 写入分配场矩阵8位unsigned char型
	FILE * bmpWrite1 = fopen(LocBmpName, "wb");
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, bmpWrite1);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, bmpWrite1);
	fwrite(ColorTAB, 1024, 1, bmpWrite1);
	for (int y = 0; y < Height; y++)
	{
		fwrite(LocMatix[y], 1, widthFile, bmpWrite1);
	}
	fclose(bmpWrite1);



	// 6. 清理，释放内存
	for (unsigned j = 0; j < Height; j++)
	{
		delete[]LocMatix[j];
		delete[]DisMatix[j];
	}
	delete[]DisMatix;
	delete[]LocMatix;


	return 1;

};

bool BmpSmooth2(const char* SrcBmpName, const char* DestBmpName)
{
	// 1. 打开源文件
	FILE *file = fopen(SrcBmpName, "rb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}

	// 2. 读取位图基本信息
	BITMAPFILEHEADER BmpFileHeader;
	BITMAPINFOHEADER BmpInfoHeader;
	unsigned char ClrTab[256 * 4];

	fread(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	fread(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);
	fread(ClrTab, 1024, 1, file);

	unsigned MtxWidth, MtxHeight, BmpWidth;
	BmpWidth = BmpInfoHeader.biWidth;
	MtxWidth = (BmpWidth + 3) / 4 * 4;
	MtxHeight = BmpInfoHeader.biHeight;

	// 3. 读取位图矩阵数据
	unsigned char * LineBuf = new unsigned char[MtxWidth]; // 读数据行缓冲

	unsigned char ** MtxBuf = new unsigned char *[MtxHeight]; // 写数据矩阵缓冲

	unsigned char ** MtxBuf2 = new unsigned char *[MtxHeight];
	unsigned i, j;
	for (i = 0; i < MtxHeight; i++)
	{
		MtxBuf[i] = new unsigned char[MtxWidth];
		fread(MtxBuf[i], MtxWidth, 1, file);
		MtxBuf2[i] = new unsigned char[MtxWidth];
		memcpy(MtxBuf2[i], MtxBuf[i], MtxWidth);
		
	}

	// 4. 平滑处理
	unsigned mid = 0;
	for (i = 1; i < MtxHeight - 1; i++)
	{
		for (j = 1; j < MtxWidth - 1; j++)//（i，j）is the center of the window
		{
			mid = (MtxBuf[i - 1][j - 1] + MtxBuf[i - 1][j]
				+ MtxBuf[i - 1][j + 1] + MtxBuf[i][j - 1]
				+ MtxBuf[i - 1][j - 1] + MtxBuf[i - 1][j + 1]
				+ MtxBuf[i + 1][j - 1] + MtxBuf[i + 1][j]
				+ MtxBuf[i + 1][j + 1]) / 9;

			MtxBuf2[i][j] = mid;
		}
	}

	
	unsigned k;
	for (k = 0; k < MtxWidth; k++)
		MtxBuf2[0][k] = MtxBuf[0][k];
	for (k = 0; k < MtxWidth; k++)
		MtxBuf2[MtxHeight - 1][k] = MtxBuf[MtxHeight - 1][k];
	for (k = 0; k < MtxHeight; k++)
		MtxBuf2[k][0] = MtxBuf[k][0];
	for (k = 0; k < MtxHeight; k++)
		MtxBuf2[k][MtxWidth - 1] = MtxBuf[k][MtxWidth - 1];

	// 5. 新建结果位图文件
	FILE *file1 = fopen(DestBmpName, "wb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}
	fwrite(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file1);
	fwrite(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file1);
	fwrite(ClrTab, 1024, 1, file1);

	for (i = 0; i < MtxHeight; i++)
	{
		fwrite(MtxBuf2[i], MtxWidth, 1, file1);
		delete[]MtxBuf2[i];
		MtxBuf2[i] = NULL;
	}
	delete[]MtxBuf2;

	for (i = 0; i < MtxHeight; i++)
	{
		delete[]MtxBuf[i];
		MtxBuf[i] = NULL;
	}
	delete[]MtxBuf;

	fclose(file);
	fclose(file1);

	return true;
};

int getVoronoi(const char* SrcBmpName, const char* DestBmpName)
{
	// 1. 打开源文件
	FILE *file = fopen(SrcBmpName, "rb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}

	// 2. 读取位图基本信息
	BITMAPFILEHEADER BmpFileHeader;
	BITMAPINFOHEADER BmpInfoHeader;
	unsigned char ClrTab[256 * 4];

	fread(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	fread(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);
	fread(ClrTab, 1024, 1, file);

	unsigned MtxWidth, MtxHeight, BmpWidth;
	BmpWidth = BmpInfoHeader.biWidth;
	MtxWidth = (BmpWidth + 3) / 4 * 4;
	MtxHeight = BmpInfoHeader.biHeight;

	// 3. 读取位图矩阵数据
	unsigned char * LineBuf = new unsigned char[MtxWidth]; // 读数据行缓冲

	unsigned char ** MtxBuf = new unsigned char *[MtxHeight]; // 写数据矩阵缓冲
	for (int i = 0; i < MtxHeight; i++)
	{
		MtxBuf[i] = new unsigned char[BmpWidth];
		fread(LineBuf, sizeof(unsigned char), MtxWidth, file);
		for (int j = 0; j < BmpWidth; j++)
		{
			MtxBuf[i][j] = LineBuf[j];
		}
	}

	// 4. 创建颜色边界矩阵，即Voronoi图
	unsigned char **  vMtx = new unsigned char *[MtxHeight];
	for (int i = 0; i < MtxHeight; i++)
	{
		vMtx[i] = new unsigned char[MtxWidth];
		for (int j = 0; j < MtxWidth; j++)
		{
			vMtx[i][j] = 0xff;
		}
	}

	for (int i = 0; i < MtxHeight; i++)
	{
		for (int j = 0; j < MtxWidth; j++)
		{
			if (i == 0 || j == 0 || i == MtxHeight - 1 || j == MtxWidth - 1)
			{
				vMtx[i][j] = 0xff;
			}
			else if ((MtxBuf[i-1][j] != MtxBuf[i + 1][j])||(MtxBuf[i][j+1] != MtxBuf[i][j-1]))
				vMtx[i][j] = 0;
			else vMtx[i][j] = 0xff;
		}
	}

	// 5. 写入Voronoi矩阵
	FILE * vfile = fopen(DestBmpName, "wb");
	fwrite(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, vfile);
	fwrite(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, vfile);
	fwrite(ClrTab, 1024, 1, vfile);
	for (int i = 0; i < MtxHeight; i++)
	{
		fwrite(vMtx[i], 1, MtxWidth, vfile);
	}
	fclose(file);
	fclose(vfile);


	// 6. 清理，释放内存
	for (int i = 0; i < MtxHeight; i++)
	{
		delete[] MtxBuf[i];
		MtxBuf[i] = NULL;
	}
	delete[]MtxBuf;
	MtxBuf = NULL;
	delete[]LineBuf;
	LineBuf = NULL;
	for (int i = 0; i < MtxHeight; i++)
	{
		delete[]vMtx[i];
		vMtx[i] = NULL;
	}
	delete[]vMtx;
	vMtx = NULL;
	return 1;
};

int getBuffer(const char* SrcBmpDisName, const char* SrcBmpName, float radius, const char* DestBmpName)
{
	// 1. 打开源文件
	FILE *file = fopen(SrcBmpDisName, "rb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}

	// 2. 读取位图基本信息
	BITMAPFILEHEADER BmpFileHeader;
	BITMAPINFOHEADER BmpInfoHeader;

	fread(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	fread(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);


	unsigned MtxWidth, MtxHeight, BmpWidth;
	BmpWidth = BmpInfoHeader.biWidth;
	MtxWidth = (BmpWidth + 3) / 4 * 4;
	MtxHeight = BmpInfoHeader.biHeight;

	// 3. 读取位图矩阵数据
	float * LineBuf = new float[BmpWidth]; // 读数据行缓冲

	float ** MtxBuf = new float *[MtxHeight]; // 写数据矩阵缓冲
	for (int i = 0; i < MtxHeight; i++)
	{
		MtxBuf[i] = new float[BmpWidth];
		fread(LineBuf, sizeof(float), BmpWidth, file);
		for (int j = 0; j < BmpWidth; j++)
		{
			MtxBuf[i][j] = LineBuf[j];
		}
	}

	// 4. 创建缓冲区矩阵
	unsigned char  **  BufMtx = new unsigned char *[MtxHeight];
	
	for (int i = 0; i < MtxHeight; i++)
	{
		BufMtx[i] = new unsigned char[MtxWidth];
		for (int j = 0; j < MtxWidth; j++)
		{
			BufMtx[i][j] = 0xff;
		}
	}
	

	for (int i = 0; i < MtxHeight; i++)
	{
		for (int j = 0; j < MtxWidth; j++)
		{
			if (MtxBuf[i][j] <= radius) BufMtx[i][j] = MtxBuf[i][j];
			else BufMtx[i][j] = MaxFloat;
		}
	}
    

	FILE * bmpWrite = fopen("processBMP/Distest.bmp", "wb");
	unsigned char ClrTab2[256 * 4];
	fwrite(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, bmpWrite);
	fwrite(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, bmpWrite);
	fread(ClrTab2, 1024, 1, bmpWrite);
	for (int y = 0; y < MtxHeight; y++)
	{
		fwrite(BufMtx[y], 1, MtxWidth, bmpWrite);
	}

	// 5. 写入Buffer矩阵	
	FILE *file2 = fopen(SrcBmpName, "rb");
	BITMAPFILEHEADER IdxFileHeader;
	BITMAPINFOHEADER IdxInfoHeader;
	unsigned char ClrTab[256 * 4];
	fread(&IdxFileHeader, sizeof(BITMAPFILEHEADER), 1, file2);
	fread(&IdxInfoHeader, sizeof(BITMAPINFOHEADER), 1, file2);
	fread(ClrTab, 1024, 1, file2);

	FILE * bufFile = fopen(DestBmpName, "wb");
	fwrite(&IdxFileHeader, sizeof(BITMAPFILEHEADER), 1, bufFile);
	fwrite(&IdxInfoHeader, sizeof(BITMAPINFOHEADER), 1, bufFile);
	fwrite(ClrTab, 1024, 1, bufFile);
	for (int i = 0; i < MtxHeight; i++)
	{
		fwrite(BufMtx[i], 1, MtxWidth, bufFile);
	}
	fclose(bmpWrite);
	fclose(file);
	fclose(file2);
	fclose(bufFile);


	// 6. 清理，释放内存
	for (int i = 0; i < MtxHeight; i++)
	{
		delete[] MtxBuf[i];
		MtxBuf[i] = NULL;
	}
	delete[]MtxBuf;
	MtxBuf = NULL;
	delete[]LineBuf;
	LineBuf = NULL;
	for (int i = 0; i < MtxHeight; i++)
	{
		delete[]BufMtx[i];
		BufMtx[i] = NULL;
	}
	delete[]BufMtx;
	BufMtx = NULL;
	return 1;
};

int AdhesionTransform(const char * SrcBmpName, CDistanceTemplet *pTemplet,float outRadius, float inRadius, const char* DestBmpName)
{
	//粘连变换：先生成外距填补凹陷，再生成内距，保凸减凹

	//外距
	TmpDistanceTransform32b(SrcBmpName, pTemplet, "processBMP/out_dis.bmp", "processBMP/out_loc.bmp",0xFF);
	getBuffer("processBMP/out_dis.bmp", SrcBmpName, outRadius, "processBMP/out_buffer.bmp");

	//内距
	TmpDistanceTransform32b("processBMP/out_buffer.bmp", pTemplet, "processBMP/in_dis.bmp", "processBMP/in_loc.bmp",0);
	getBuffer("processBMP/in_dis.bmp", SrcBmpName, inRadius, DestBmpName);
	return true;
};
int getAxis(const char* SrcBmpName, CDistanceTemplet *pTemplet,const char* DestBmpName)
{
	TmpDistanceTransform32b(SrcBmpName, pTemplet, "processBMP/Axis_dis.bmp", "processBMP/Axis_loc.bmp",0);
	getVoronoi("processBMP/Axis_loc.bmp", DestBmpName);
	return true;
};

void DDAline(int x0, int y0, int xEnd, int yEnd, unsigned char ** dMtx)
{
	int dx = xEnd - x0, dy = yEnd - y0, steps, k;
	float xIncrement, yIncrement, x = x0, y = y0;
	if (fabs(dx) > fabs(dy))
	{
		steps = fabs(dx);
	}
	else
	{
		steps = fabs(dy);
	}
	xIncrement = float(dx) / float(steps);
	yIncrement = float(dy) / float(steps);
	dMtx[int(x)][int(y)] = 0;
	for (k = 0; k < steps; k++)
	{
		x += xIncrement;
		y += yIncrement;
		dMtx[int(x)][int(y)] = 0;
	}
};


int getDelauney(const char* SrcBmpName, CDistanceTemplet *pTemplet, const char* DestBmpName)
{
	// 1. 读取bmp
	unsigned i, j; // 循环变量
	FILE* SrcFileStream = fopen(SrcBmpName, "rb");
	if (SrcFileStream == NULL) return -1; // 出错代码1，文件不存在

	BITMAPFILEHEADER SrcFileHead;
	BITMAPINFOHEADER SrcFileInfo;
	unsigned char ColorIdx[1024];

	if (fread(&SrcFileHead, sizeof(BITMAPFILEHEADER), 1, SrcFileStream) != 1)
	{
		fclose(SrcFileStream);
		return -1;
	}
	if (fread(&SrcFileInfo, sizeof(BITMAPINFOHEADER), 1, SrcFileStream) != 1)
	{
		fclose(SrcFileStream);
		return -1;
	}
	if (fread(ColorIdx, 1024, 1, SrcFileStream) != 1)
	{
		fclose(SrcFileStream);
		return -1;
	}

	if (SrcFileInfo.biBitCount != 8) // 不是8bit位图
	{
		fclose(SrcFileStream);
		return -2; // 出错代码2，文件格式不合要求
	}

	int Height = SrcFileInfo.biHeight;
	int Width = SrcFileInfo.biWidth;
	int LineBytes = (SrcFileInfo.biWidth + 3) / 4 * 4;

	// 2. 距离变换
	
	

	// 1. 建立初始距离场、分配场，并提取点坐标
	unsigned char ** LocMtx;
	float ** DisMtx;
	LocMtx = new unsigned char *[Height];
	DisMtx = new float *[Height];
	vector<Point* > points;
	for (int j = 0; j < Height; j++)
	{
		LocMtx[j] = new unsigned char[LineBytes];
		DisMtx[j] = new float[Width];
		Point* pt = new Point();
		fread(LocMtx[j], sizeof(char), LineBytes, SrcFileStream);
		for (int i = 0; i < Width; i++)
		{
			if (LocMtx[j][i] == 255) DisMtx[j][i] = MaxFloat;
			else
			{
				DisMtx[j][i] = 0;
				pt->x = j;
				pt->y = i;
				pt->color = LocMtx[j][i];
				points.push_back(pt);
			}
		}
	}

	// 2. 模板距离变换
	// 2.1 从左下到右上方向
	for (int j = 0; j < Height; j++)
		for (int i = 0; i < Width; i++)
		{
			if (fabs(DisMtx[j][i]) < MinFloat) continue;
			float MinDis = DisMtx[j][i];

			for (int k = 0; k < pTemplet->GetSize() / 2 + 1; k++)
			{
				int OffX = pTemplet->Get0ffX(k);
				int OffY = pTemplet->Get0ffY(k);
				float TmpDis = pTemplet->GetDis(k);

				int x = i + OffX;
				int y = j + OffY;

				if (x < 0 || y < 0 || x > Width - 1 || y > Height - 1) continue;

				if (fabs(DisMtx[y][x] - MaxFloat) < MinFloat) continue; // 该位置如果是无穷大不可计算

				if (MinDis > DisMtx[y][x] + TmpDis)
				{
					MinDis = DisMtx[y][x] + TmpDis;
					DisMtx[j][i] = MinDis;
					LocMtx[j][i] = LocMtx[y][x];
				}
			}
		}
	// 2.2 从右上到左下方向
	for (int j = Height - 1; j > 0; j--)
		for (int i = Width - 1; i > 0; i--)
		{
			if (fabs(DisMtx[j][i]) < MinFloat) continue;
			float MinDis = DisMtx[j][i];

			for (int k = pTemplet->GetSize() / 2; k < pTemplet->GetSize(); k++)
			{
				int OffX = pTemplet->Get0ffX(k);
				int OffY = pTemplet->Get0ffY(k);
				float TmpDis = pTemplet->GetDis(k);

				int x = i + OffX;
				int y = j + OffY;

				if (x < 0 || y < 0 || x > Width - 1 || y > Height - 1) continue;

				if (fabs(DisMtx[y][x] - MaxFloat) < MinFloat) continue; // 该位置如果是无穷大不可计算

				if (MinDis > DisMtx[y][x] + TmpDis)
				{
					MinDis = DisMtx[y][x] + TmpDis;
					DisMtx[j][i] = MinDis;
					LocMtx[j][i] = LocMtx[y][x];
				}
			}
		}


	// 3. 提取相邻颜色
	vector<vector<unsigned char> >colors;
	size_t k;
	for (j = 0; j < Height; j++)
	{
		for (i = 0; i < LineBytes; i++)
		{
			if (i == 0 || j == 0 || i == LineBytes - 1 || j == Height - 1) continue;
			if (LocMtx[j][i] != LocMtx[j - 1][i - 1] || LocMtx[j][i] != LocMtx[j - 1][i]
				|| LocMtx[j][i] != LocMtx[j - 1][i + 1] || LocMtx[j][i] != LocMtx[j][i - 1]
				|| LocMtx[j][i] != LocMtx[j][i + 1] || LocMtx[j][i] != LocMtx[j + 1][i - 1]
				|| LocMtx[j][i] != LocMtx[j + 1][i] || LocMtx[j][i] != LocMtx[j + 1][i + 1])
			{
				if (LocMtx[j][i] != LocMtx[j - 1][i - 1] && LocMtx[j][i] != 0 && LocMtx[j - 1][i - 1] != 0)
				{
					vector<unsigned char> cls;
					unsigned char color1 = LocMtx[j][i];
					unsigned char color2 = LocMtx[j - 1][i - 1];
					cls.push_back(color1);
					cls.push_back(color2);
					for (k = 0; k < colors.size(); k++)
					{
						if (colors[k] == cls)
							break;
					}
					if (k == colors.size() && cls[0] != 255 && cls[1] != 255)
						colors.push_back(cls);
					else
						cls.clear();
				}

				if (LocMtx[j][i] != LocMtx[j - 1][i] && LocMtx[j][i] != 0 && LocMtx[j - 1][i] != 0)
				{
					vector<unsigned char> cls;
					unsigned char color1 = LocMtx[j][i];
					unsigned char color2 = LocMtx[j - 1][i];
					cls.push_back(color1);
					cls.push_back(color2);
					for (k = 0; k < colors.size(); k++)
					{
						if (colors[k] == cls)
							break;
					}
					if (k == colors.size() && cls[0] != 255 && cls[1] != 255)
						colors.push_back(cls);
					else
						cls.clear();
				}
				if (LocMtx[j][i] != LocMtx[j - 1][i + 1] && LocMtx[j][i] != 0 && LocMtx[j - 1][i + 1] != 0)
				{
					vector<unsigned char> cls;
					unsigned char color1 = LocMtx[j][i];
					unsigned char color2 = LocMtx[j - 1][i + 1];
					cls.push_back(color1);
					cls.push_back(color2);
					for (k = 0; k < colors.size(); k++)
					{
						if (colors[k] == cls)
							break;
					}
					if (k == colors.size() && cls[0] != 255 && cls[1] != 255)
						colors.push_back(cls);
					else
						cls.clear();
				}
				if (LocMtx[j][i] != LocMtx[j][i - 1] && LocMtx[j][i] != 0 && LocMtx[j][i - 1] != 0)
				{
					vector<unsigned char> cls;
					unsigned char color1 = LocMtx[j][i];
					unsigned char color2 = LocMtx[j][i - 1];
					cls.push_back(color1);
					cls.push_back(color2);
					for (k = 0; k < colors.size(); k++)
					{
						if (colors[k] == cls)
							break;
					}
					if (k == colors.size() && cls[0] != 255 && cls[1] != 255)
						colors.push_back(cls);
					else
						cls.clear();
				}
				if (LocMtx[j][i] != LocMtx[j][i + 1] && LocMtx[j][i] != 0 && LocMtx[j][i + 1] != 0)
				{
					vector<unsigned char> cls;
					unsigned char color1 = LocMtx[j][i];
					unsigned char color2 = LocMtx[j][i + 1];
					cls.push_back(color1);
					cls.push_back(color2);
					for (k = 0; k < colors.size(); k++)
					{
						if (colors[k] == cls)
							break;
					}
					if (k == colors.size() && cls[0] != 255 && cls[1] != 255)
						colors.push_back(cls);
					else
						cls.clear();
				}
				if (LocMtx[j][i] != LocMtx[j + 1][i - 1] && LocMtx[j][i] != 0 && LocMtx[j + 1][i - 1] != 0)
				{
					vector<unsigned char> cls;
					unsigned char color1 = LocMtx[j][i];
					unsigned char color2 = LocMtx[j + 1][i - 1];
					cls.push_back(color1);
					cls.push_back(color2);
					for (k = 0; k < colors.size(); k++)
					{
						if (colors[k] == cls)
							break;
					}
					if (k == colors.size() && cls[0] != 255 && cls[1] != 255)
						colors.push_back(cls);
					else
						cls.clear();
				}
				if (LocMtx[j][i] != LocMtx[j + 1][i] && LocMtx[j][i] != 0 && LocMtx[j + 1][i] != 0)
				{
					vector<unsigned char> cls;
					unsigned char color1 = LocMtx[j][i];
					unsigned char color2 = LocMtx[j + 1][i];
					cls.push_back(color1);
					cls.push_back(color2);
					for (k = 0; k < colors.size(); k++)
					{
						if (colors[k] == cls)
							break;
					}
					if (k == colors.size() && cls[0] != 255 && cls[1] != 255)
						colors.push_back(cls);
					else
						cls.clear();
				}
				if (LocMtx[j][i] != LocMtx[j + 1][i + 1] && LocMtx[j][i] != 0 && LocMtx[j + 1][i + 1] != 0)
				{
					vector<unsigned char> cls;
					unsigned char color1 = LocMtx[j][i];
					unsigned char color2 = LocMtx[j + 1][i + 1];
					cls.push_back(color1);
					cls.push_back(color2);
					for (k = 0; k < colors.size(); k++)
					{
						if (colors[k] == cls)
							break;
					}
					if (k == colors.size() && cls[0] != 255 && cls[1] != 255)
						colors.push_back(cls);
					else
						cls.clear();
				}
			}
		}
	}

	//4. 点对匹配
	vector<vector<Point*> >pts;
	for (vector<vector<unsigned char> >::iterator iter = colors.begin(); iter != colors.end(); iter++)
	{
		vector<Point*> p2;
		vector<unsigned char> vec = *iter;
		unsigned char color1 = vec[0];
		unsigned char color2 = vec[1];
		for (vector<Point* >::iterator it = points.begin(); it != points.end(); it++)
		{
			if ((*it)->color == color1)
				p2.push_back(*it);
			else if ((*it)->color == color2)
				p2.push_back(*it);
		}
		pts.push_back(p2);
	}
	pts.size();

	// 5. 点对连线
	unsigned char ** dMtx;
	dMtx = new unsigned char *[Height];
	for (j = 0; j < Height; j++)
	{
		dMtx[j] = new unsigned char[LineBytes];
		for (i = 0; i < LineBytes; i++)
			dMtx[j][i] = 255;
	}
	for (vector<vector<Point*> >::iterator iter = pts.begin(); iter != pts.end(); iter++)
	{
		vector<Point*> vec = *iter;
		int x0 = vec[0]->x;
		int y0 = vec[0]->y;
		int xend = vec[1]->x;
		int yend = vec[1]->y;
		DDAline(x0, y0, xend, yend, dMtx);
	}


	// 6. 写入Delaunay矩阵8bitfloat型
	FILE * dWrite = fopen(DestBmpName, "wb");
	fwrite(&SrcFileHead, sizeof(BITMAPFILEHEADER), 1, dWrite);
	fwrite(&SrcFileInfo, sizeof(BITMAPINFOHEADER), 1, dWrite);
	fwrite(ColorIdx, 1024, 1, dWrite);
	for (int y = 0; y < Height; y++)
	{
		//fwrite(dt->LocMtx[y], sizeof(char), LineBytes, dWrite);
		fwrite(dMtx[y], sizeof(char), LineBytes, dWrite);
	}
	fclose(dWrite);


	// 7. 清理，释放内存
	if (dMtx != 0x00)
		for (int j = 0; j < Height; j++)
		{
			delete[]dMtx[j];
		}
	delete[]dMtx;
	return 1;
}

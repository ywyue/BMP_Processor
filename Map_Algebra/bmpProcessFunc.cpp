#include "pch.h"
#include <Windows.h>
#include "bmpProcessFunc.h"
#include<stdio.h>
#include <assert.h>
#include <vector>
#define MaxFloat 9999999999.999999
#define MinFloat 0.000001
using namespace std;

/***   1.��ɫ����   ***/
bool BmpReverse(const char* SrcBmpName, const char* DestBmpName)
{
	// 1. ��Դ�ļ�
	FILE *file = fopen(SrcBmpName, "rb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}

	// 2. ��ȡλͼ������Ϣ
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

	// 3. ��ȡλͼ��������
	unsigned char * LineBuf = new unsigned char[MtxWidth]; // �������л���

	unsigned char ** MtxBuf = new unsigned char *[MtxHeight]; // д���ݾ��󻺳�
	for (int i = 0; i < MtxHeight; i++)
	{
		MtxBuf[i] = new unsigned char[MtxWidth];
		for (int j = 0; j < MtxWidth; j++)
		{
			MtxBuf[i][j] = 0xFF;
		}
	}


	// 4. ��ɫ����
	for (int i = 0; i < MtxHeight; i++)
	{
		fread(LineBuf, sizeof(unsigned char), MtxWidth, file);
		for (int j = 0; j < MtxWidth; j++)
		{
			if (LineBuf[j] == 0xFF) continue;
			MtxBuf[i][j] = 255 - LineBuf[j];
		}
	}
	// 5. �½����λͼ�ļ�
	FILE *file1 = fopen(DestBmpName, "wb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}
	fwrite(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file1);
	fwrite(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file1);
	fwrite(ClrTab, 1024, 1, file1);

	// 6. д����
	for (int i = 0; i < MtxHeight; i++)
	{
		fwrite(MtxBuf[i], MtxWidth, 1, file1);
	}

	// 7. �����ڴ桢�ر��ļ�
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

/***   2.���ô���   ***/
bool BmpOverlay(const char* SrcBmpName1, const char* SrcBmpName2, const char* DestBmpName) 
{
	// 1 ��Դ�ļ�
	FILE *file1 = fopen(SrcBmpName1, "rb");
	FILE *file2 = fopen(SrcBmpName2, "rb");
	if (file1 == NULL||file2 == NULL)
	{
		printf("file not found!");
		return false;
	}

	// 2.1 ��ȡλͼ1������Ϣ
	BITMAPFILEHEADER BmpFileHeader1;
	BITMAPINFOHEADER BmpInfoHeader1;
	unsigned char ClrTab1[256 * 4]; //��ɫ��

	fread(&BmpFileHeader1, sizeof(BITMAPFILEHEADER), 1, file1);
	fread(&BmpInfoHeader1, sizeof(BITMAPINFOHEADER), 1, file1);
	fread(ClrTab1, 1024, 1, file1);
	unsigned MtxWidth1, MtxHeight1, BmpWidth1;
	BmpWidth1 = BmpInfoHeader1.biWidth;
	MtxWidth1 = (BmpWidth1 + 3) / 4 * 4;
	MtxHeight1 = BmpInfoHeader1.biHeight;

	//2.2  ��ȡλͼ2������Ϣ

	BITMAPFILEHEADER BmpFileHeader2;
	BITMAPINFOHEADER BmpInfoHeader2;
	unsigned char ClrTab2[256 * 4]; //��ɫ��

	fread(&BmpFileHeader2, sizeof(BITMAPFILEHEADER), 1, file2);
	fread(&BmpInfoHeader2, sizeof(BITMAPINFOHEADER), 1, file2);
	fread(ClrTab2, 1024, 1, file2);
	unsigned MtxWidth2, MtxHeight2, BmpWidth2;
	BmpWidth2 = BmpInfoHeader2.biWidth;
	MtxWidth2 = (BmpWidth2 + 3) / 4 * 4;
	MtxHeight2 = BmpInfoHeader2.biHeight;

	//3.  ����
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

	// 5. �½����λͼ�ļ�
	FILE *file3 = fopen(DestBmpName, "wb");
	if (file3 == NULL)
	{
		printf("file not found!");
		return false;
	}
	fwrite(&BmpFileHeader1, sizeof(BITMAPFILEHEADER), 1, file3);
	fwrite(&BmpInfoHeader1, sizeof(BITMAPINFOHEADER), 1, file3);
	fwrite(ClrTab1, 1024, 1, file3);

	// 6. д����

	for (i = 0; i < MtxHeight1; i++)
	{
		fwrite(destBmpMtx[i], MtxWidth1, 1, file3);
		delete[]destBmpMtx[i];
		destBmpMtx[i] = NULL;
	}

	// 7. �����ڴ桢�ر��ļ�
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

/***   3.ƽ������   ***/
bool BmpSmooth(const char* SrcBmpName, CDistanceTemplet *pTemplet, const char* DestBmpName)
{
	// 1. ��Դ�ļ�
	FILE *file = fopen(SrcBmpName, "rb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}

	// 2. ��ȡλͼ������Ϣ
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

	// 3. ��ȡλͼ��������
	unsigned char * LineBuf = new unsigned char[MtxWidth]; // �������л���

	unsigned char ** MtxBuf = new unsigned char *[MtxHeight]; // д���ݾ��󻺳�

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

	// 4. ƽ������
	for (int y = 0; y < MtxHeight; y++) // 2��ѭ������ÿһ��դ��
	{
		for (int x = 0; x < MtxWidth; x++)
		{
			if (MtxBuf[y][x]== 0xff) continue;        
			unsigned char tmp = 0;
			for (unsigned k = 0; k < pTemplet->GetSize(); k++) // ���ϰ�ģ�����Ԫ��ѭ��
			{
				char offx = pTemplet->Get0ffX(k);
				char offy = pTemplet->Get0ffY(k);
				float tmpDis = pTemplet->GetDis(k);     // ȥ��ģ��Ҫ�أ�x,yƫ�Ƽ���Ӧģ����Ծ���

				int CurrX = x + offx;              // ����ģ��Ԫ�ض�Ӧ��դ��λ��
				int CurrY = y + offy;
				if ((CurrX < 0) || (CurrX > MtxWidth - 1)) continue;  // �жϸ�λ���Ƿ�Խ��
				if ((CurrY < 0) || (CurrY > MtxHeight - 1)) continue;
				unsigned char MtxValue = MtxBuf[CurrY][CurrX];
				tmp += MtxValue * tmpDis;
			}
			MtxBuf2[y][x] = tmp / pTemplet->GetSize();
		}
	}

	// 5. �½����λͼ�ļ�
	FILE *file1 = fopen(DestBmpName, "wb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}
	fwrite(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file1);
	fwrite(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file1);
	fwrite(ClrTab, 1024, 1, file1);

	// 6. д����
	for (int i = 0; i < MtxHeight; i++)
	{
		fwrite(MtxBuf2[i], 1, MtxWidth, file1);
	}

	// 7. �����ڴ桢�ر��ļ�
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


int Bmp256to32b(const char* SourceFileName, const char* IdxFileName)  //��Դ256λͼתΪ 32bit λͼ
{
	unsigned i, j; // ѭ������
	// *1 ����Դ�ļ�
	assert(SourceFileName != NULL);

	FILE* SrcFileStream = fopen(SourceFileName, "rb");
	if (SrcFileStream == NULL) return -1; // �������1���ļ�������

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

	if (SrcFileInfo.biBitCount != 8) // 256ɫͼ����
	{
		fclose(SrcFileStream);
		return -2; // �������2���ļ���ʽ����Ҫ��
	}


	// *2 ����ʵ������
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


	// *2.1 ����ԭʼ����
	unsigned char * pSrcCellLineBuf = new unsigned char[LineBytes];   // Դ�ļ��л���
	unsigned **pIdxCellMatixBuf = new unsigned*[IdxFileInfo.biHeight]; //��������
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
				/*  ȡ��Ӧ��ɫ*/

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
	if (ObjIdxFile == NULL) return -3; // �������3��ʵ�����������ļ��޷�����
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
	return 1; // ���سɹ�����1
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

	// 3. ������ʼ ���볡�� ���䳡
	unsigned char **LocMatix = NULL;
	LocMatix = new unsigned char *[Height];

	float **DisMatix = NULL;
	DisMatix = new float *[Height];
	for (unsigned i = 0; i < Height; i++)
	{
		LocMatix[i] = new unsigned char[widthFile];
		// 3.1 ���䳡����ֱ�Ӵ�λͼ��ȡ
		fread(LocMatix[i], 1, widthFile, bmpFile);
		DisMatix[i] = new float[Width];
		// 3.2 ���ݷ��䳡��ʵ�����طֲ���ʼ�����볡
		for (unsigned j = 0; j < Width; j++)
		{
			if (LocMatix[i][j] == MaxColor) DisMatix[i][j] = MaxFloat;// �����
			else DisMatix[i][j] = 0;
		}
	}
	fclose(bmpFile);

	// 4. ģ�����任
	// 4.1 �����µ����Ϸ���
	for (int y = 0; y < Height; y++) // 2��ѭ������ÿһ��դ��
	{
		for (int x = 0; x < Width; x++)
		{
			float tmpMin = DisMatix[y][x] + 0; // ����ģ����Сֵ��ʼ��
			if (fabs(tmpMin) < MinFloat) continue;             // Ϊ0����ʾ�ڵ��������
			for (unsigned k = 0; k < pTemplet->GetSize() / 2; k++) // ���ϰ�ģ�����Ԫ��ѭ��
			{
				char offx = pTemplet->Get0ffX(k);
				char offy = pTemplet->Get0ffY(k);
				float tmpDis = pTemplet->GetDis(k);     // ȥ��ģ��Ҫ�أ�x,yƫ�Ƽ���Ӧģ����Ծ���

				int CurrX = x + offx;              // ����ģ��Ԫ�ض�Ӧ��դ��λ��
				int CurrY = y + offy;
				if ((CurrX < 0) || (CurrX > Width - 1)) continue;  // �жϸ�λ���Ƿ�Խ��
				if ((CurrY < 0) || (CurrY > Height - 1)) continue;

				if (fabs(DisMatix[CurrY][CurrX] - MaxFloat) < MinFloat) continue; // ��λ�����������󲻿ɼ���

				float dis1 = DisMatix[CurrY][CurrX] + tmpDis; // ��ǰդ�����+ģ����Ծ���
				if (dis1 < tmpMin)  // ����Сֵ
				{
					tmpMin = dis1;
					DisMatix[y][x] = dis1;                   // ��С��������������
					LocMatix[y][x] = LocMatix[CurrY][CurrX]; // ��С������Դ����������
				}
			}
		}
	}
	// 4.2 �����ϵ����·���
	for (int y = Height - 1; y > 0; y--) // 2��ѭ������ÿһ��դ��
	{
		for (int x = Width - 1; x > 0; x--)
		{
			float tmpMin = DisMatix[y][x] + 0; // ����ģ����Сֵ��ʼ��
			if (fabs(tmpMin) < MinFloat) continue;             // Ϊ0����ʾ�ڵ��������
			for (unsigned k = pTemplet->GetSize(); k > pTemplet->GetSize() / 2; k--) // ���°�ģ�����Ԫ��ѭ��
			{
				char offx = pTemplet->Get0ffX(k);
				char offy = pTemplet->Get0ffY(k);
				float tmpDis = pTemplet->GetDis(k);     // ȥ��ģ��Ҫ�أ�x,yƫ�Ƽ���Ӧģ����Ծ���

				int CurrX = x + offx;              // ����ģ��Ԫ�ض�Ӧ��դ��λ��
				int CurrY = y + offy;
				if ((CurrX < 0) || (CurrX > Width - 1)) continue;  // �жϸ�λ���Ƿ�Խ��
				if ((CurrY < 0) || (CurrY > Height - 1)) continue;

				if (fabs(DisMatix[CurrY][CurrX] - MaxFloat) < MinFloat) continue; // ��λ�����������󲻿ɼ���

				float dis1 = DisMatix[CurrY][CurrX] + tmpDis; // ��ǰդ�����+ģ����Ծ���
				if (dis1 < tmpMin)  // ����Сֵ
				{
					tmpMin = dis1;
					DisMatix[y][x] = dis1;                   // ��С��������������
					LocMatix[y][x] = LocMatix[CurrY][CurrX]; // ��С������Դ����������
				}
			}
		}
	}

	// 5. д�����ļ�
	// 5.1 д��������32λfloat��
	BITMAPFILEHEADER IdxFileHead; // 32λλͼͷ�ṹ
	BITMAPINFOHEADER IdxFileInfo;

	IdxFileHead.bfType = fileHeader.bfType;  //λͼ���
	IdxFileHead.bfSize = 54 + infoHeader.biWidth * 4 * infoHeader.biHeight;  //BMPͼ���ļ��Ĵ�С
	IdxFileHead.bfReserved1 = 0;  //λͼ�ļ������֣�����Ϊ0(6-7�ֽ�) 
	IdxFileHead.bfReserved2 = 0;  //λͼ�ļ������֣�����Ϊ0(7-9�ֽ�)
	IdxFileHead.bfOffBits = 54;  //BMPͼ�����ݵĵ�ַ

	IdxFileInfo.biSize = 40;  //���ṹ�Ĵ�С�����ݲ�ͬ�Ĳ���ϵͳ����ͬ����Windows�У����ֶε�ֵ��Ϊ28h�ֽ�=40�ֽ�
	IdxFileInfo.biWidth = infoHeader.biWidth;  //BMPͼ��Ŀ��
	IdxFileInfo.biHeight = infoHeader.biHeight;  //BMPͼ��ĸ߶�
	IdxFileInfo.biPlanes = 1;  //Ŀ���豸�ļ��𣬱���Ϊ1
	IdxFileInfo.biBitCount = 32;  //BMPͼ���ɫ���һ�������ö���λ��ʾ��������1��4��8��16��24��32
	IdxFileInfo.biCompression = 0;  //ѹ����ʽ��0��ʾ��ѹ����1��ʾRLE8ѹ����2��ʾRLE4ѹ����3��ʾÿ������ֵ��ָ�����������
	IdxFileInfo.biSizeImage = 0;  //BMPͼ�����ݴ�С��������4�ı�����ͼ�����ݴ�С����4�ı���ʱ��0��䲹��
	IdxFileInfo.biXPelsPerMeter = 0;  //ˮƽ�ֱ��ʣ���λ����/m
	IdxFileInfo.biYPelsPerMeter = 0;  //��ֱ�ֱ��ʣ���λ����/m
	IdxFileInfo.biClrUsed = 0;  //BMPͼ��ʹ�õ���ɫ��0��ʾʹ��ȫ����ɫ
	IdxFileInfo.biClrImportant = 0;  //��Ҫ����ɫ������ֵΪ0ʱ������ɫ����Ҫ

	FILE * bmpWrite = fopen(DisBmpName, "wb");
	fwrite(&IdxFileHead, sizeof(BITMAPFILEHEADER), 1, bmpWrite);
	fwrite(&IdxFileInfo, sizeof(BITMAPINFOHEADER), 1, bmpWrite);
	for (int y = 0; y < Height; y++)
	{
		fwrite(DisMatix[y], sizeof(float), Width, bmpWrite);
	}
	fclose(bmpWrite);

	// 5.2 д����䳡����8λunsigned char��
	FILE * bmpWrite1 = fopen(LocBmpName, "wb");
	fwrite(&fileHeader, sizeof(BITMAPFILEHEADER), 1, bmpWrite1);
	fwrite(&infoHeader, sizeof(BITMAPINFOHEADER), 1, bmpWrite1);
	fwrite(ColorTAB, 1024, 1, bmpWrite1);
	for (int y = 0; y < Height; y++)
	{
		fwrite(LocMatix[y], 1, widthFile, bmpWrite1);
	}
	fclose(bmpWrite1);



	// 6. �����ͷ��ڴ�
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
	// 1. ��Դ�ļ�
	FILE *file = fopen(SrcBmpName, "rb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}

	// 2. ��ȡλͼ������Ϣ
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

	// 3. ��ȡλͼ��������
	unsigned char * LineBuf = new unsigned char[MtxWidth]; // �������л���

	unsigned char ** MtxBuf = new unsigned char *[MtxHeight]; // д���ݾ��󻺳�

	unsigned char ** MtxBuf2 = new unsigned char *[MtxHeight];
	unsigned i, j;
	for (i = 0; i < MtxHeight; i++)
	{
		MtxBuf[i] = new unsigned char[MtxWidth];
		fread(MtxBuf[i], MtxWidth, 1, file);
		MtxBuf2[i] = new unsigned char[MtxWidth];
		memcpy(MtxBuf2[i], MtxBuf[i], MtxWidth);
		
	}

	// 4. ƽ������
	unsigned mid = 0;
	for (i = 1; i < MtxHeight - 1; i++)
	{
		for (j = 1; j < MtxWidth - 1; j++)//��i��j��is the center of the window
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

	// 5. �½����λͼ�ļ�
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
	// 1. ��Դ�ļ�
	FILE *file = fopen(SrcBmpName, "rb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}

	// 2. ��ȡλͼ������Ϣ
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

	// 3. ��ȡλͼ��������
	unsigned char * LineBuf = new unsigned char[MtxWidth]; // �������л���

	unsigned char ** MtxBuf = new unsigned char *[MtxHeight]; // д���ݾ��󻺳�
	for (int i = 0; i < MtxHeight; i++)
	{
		MtxBuf[i] = new unsigned char[BmpWidth];
		fread(LineBuf, sizeof(unsigned char), MtxWidth, file);
		for (int j = 0; j < BmpWidth; j++)
		{
			MtxBuf[i][j] = LineBuf[j];
		}
	}

	// 4. ������ɫ�߽���󣬼�Voronoiͼ
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

	// 5. д��Voronoi����
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


	// 6. �����ͷ��ڴ�
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
	// 1. ��Դ�ļ�
	FILE *file = fopen(SrcBmpDisName, "rb");
	if (file == NULL)
	{
		printf("file not found!");
		return false;
	}

	// 2. ��ȡλͼ������Ϣ
	BITMAPFILEHEADER BmpFileHeader;
	BITMAPINFOHEADER BmpInfoHeader;

	fread(&BmpFileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	fread(&BmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, file);


	unsigned MtxWidth, MtxHeight, BmpWidth;
	BmpWidth = BmpInfoHeader.biWidth;
	MtxWidth = (BmpWidth + 3) / 4 * 4;
	MtxHeight = BmpInfoHeader.biHeight;

	// 3. ��ȡλͼ��������
	float * LineBuf = new float[BmpWidth]; // �������л���

	float ** MtxBuf = new float *[MtxHeight]; // д���ݾ��󻺳�
	for (int i = 0; i < MtxHeight; i++)
	{
		MtxBuf[i] = new float[BmpWidth];
		fread(LineBuf, sizeof(float), BmpWidth, file);
		for (int j = 0; j < BmpWidth; j++)
		{
			MtxBuf[i][j] = LineBuf[j];
		}
	}

	// 4. ��������������
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

	// 5. д��Buffer����	
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


	// 6. �����ͷ��ڴ�
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
	//ճ���任���������������ݣ��������ھ࣬��͹����

	//���
	TmpDistanceTransform32b(SrcBmpName, pTemplet, "processBMP/out_dis.bmp", "processBMP/out_loc.bmp",0xFF);
	getBuffer("processBMP/out_dis.bmp", SrcBmpName, outRadius, "processBMP/out_buffer.bmp");

	//�ھ�
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
	// 1. ��ȡbmp
	unsigned i, j; // ѭ������
	FILE* SrcFileStream = fopen(SrcBmpName, "rb");
	if (SrcFileStream == NULL) return -1; // �������1���ļ�������

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

	if (SrcFileInfo.biBitCount != 8) // ����8bitλͼ
	{
		fclose(SrcFileStream);
		return -2; // �������2���ļ���ʽ����Ҫ��
	}

	int Height = SrcFileInfo.biHeight;
	int Width = SrcFileInfo.biWidth;
	int LineBytes = (SrcFileInfo.biWidth + 3) / 4 * 4;

	// 2. ����任
	
	

	// 1. ������ʼ���볡�����䳡������ȡ������
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

	// 2. ģ�����任
	// 2.1 �����µ����Ϸ���
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

				if (fabs(DisMtx[y][x] - MaxFloat) < MinFloat) continue; // ��λ�����������󲻿ɼ���

				if (MinDis > DisMtx[y][x] + TmpDis)
				{
					MinDis = DisMtx[y][x] + TmpDis;
					DisMtx[j][i] = MinDis;
					LocMtx[j][i] = LocMtx[y][x];
				}
			}
		}
	// 2.2 �����ϵ����·���
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

				if (fabs(DisMtx[y][x] - MaxFloat) < MinFloat) continue; // ��λ�����������󲻿ɼ���

				if (MinDis > DisMtx[y][x] + TmpDis)
				{
					MinDis = DisMtx[y][x] + TmpDis;
					DisMtx[j][i] = MinDis;
					LocMtx[j][i] = LocMtx[y][x];
				}
			}
		}


	// 3. ��ȡ������ɫ
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

	//4. ���ƥ��
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

	// 5. �������
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


	// 6. д��Delaunay����8bitfloat��
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


	// 7. �����ͷ��ڴ�
	if (dMtx != 0x00)
		for (int j = 0; j < Height; j++)
		{
			delete[]dMtx[j];
		}
	delete[]dMtx;
	return 1;
}

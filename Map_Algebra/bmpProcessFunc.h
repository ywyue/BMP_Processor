#pragma once
#include"DisTemplate.h"

//反色处理：255-当前颜色
bool BmpReverse(const char* SrcBmpName, const char* DestBmpName);

//叠置处理：将两个256位图颜色相加
bool BmpOverlay(const char* SrcBmpName1, const char* SrcBmpName2, const char* DestBmpName);

//聚焦函数：平滑处理
bool BmpSmooth(const char* SrcBmpName, CDistanceTemplet *pTemplet, const char* DestBmpName);
bool BmpSmooth2(const char* SrcBmpName, const char* DestBmpName);

//格式转换：将源256位图转为32bit位图
int Bmp256to32b(const char* SourceFileName, const char* IdxFileName);

//距离转换：读入一幅256色位图，生成对应距离场及分配场
//输入：256色位图，模板  
//输出：2个位图：距离场32位，分配场8位
int TmpDistanceTransform32b(const char *SrcBmpName, CDistanceTemplet *pTemplet, const char *DisBmpName, const char *LocBmpName,unsigned char MaxColor);

//Voronoi图生成：在八边形距离变换的颜色分配图的矩阵基础下，求得颜色分配的边界， 即Voronoi图 
int getVoronoi(const char* SrcBmpName, const char* DestBmpName);

//缓冲区生成
int getBuffer(const char* SrcBmpDisName, const char* SrcBmpName, float radius, const char* DestBmpName);

//粘连变换
int AdhesionTransform(const char * SrcBmpName, CDistanceTemplet *pTemplet,float outRadius, float inRadius, const char* DestBmpName);

//中轴线提取
int getAxis(const char* SrcBmpName, CDistanceTemplet *pTemplet, const char* DestBmpName);

//Delauney三角网提取
int getDelauney(const char* SrcBmpName, CDistanceTemplet *pTemplet, const char* DestBmpName);

//DDA连线
void DDAline(int x0, int y0, int xEnd, int yEnd, unsigned char ** dMtx);
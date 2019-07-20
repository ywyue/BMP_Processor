// Map_Algebra.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "bmpProcessFunc.h"
#include "DisTemplate.h"
int main()
{
	//1.反色处理
	BmpReverse("inputBMP/bmp1.bmp", "outputBMP/ReverseBMP.bmp");

	//2.256位转32位
	Bmp256to32b("inputBMP/bmp1.bmp", "outputBMP/32index.bmp");

	//3.叠置处理
	BmpOverlay("inputBMP/bmp1.bmp", "inputBMP/bmp2.bmp", "outputBMP/overlay.bmp");

	//CManHattanTmp *mhtmp = new CManHattanTmp();   
	//CEuTmp13 *eutmp = new CEuTmp13();
	//CEu5Tmp *eu5tmp = new CEu5Tmp();
	COctagonTmp *octmp = new COctagonTmp();
	
	//4.距离变换
	TmpDistanceTransform32b("inputBMP/bmp1.bmp", octmp, "outputBMP/dis.bmp", "outputBMP/loc.bmp",0xFF);

	//5.平滑处理
	BmpSmooth2("inputBMP/bmp1.bmp", "outputBMP/Smoothbmp1.bmp");

	//6.Voronoi变换
	getVoronoi("outputBMP/loc.bmp", "outputBMP/Voronoi.bmp");

	//7.缓冲区变换
	getBuffer("outputBMP/dis.bmp", "inputBMP/bmp1.bmp", 30,"outputBMP/buffer.bmp");

	//8.粘连变换
	AdhesionTransform("inputBMP/bmp1.bmp", octmp, 20, 50, "outputBMP/adhesionTransformed.bmp");

	//9.中轴线提取
	getAxis("inputBMP/axtest.bmp", octmp,"outputBMP/Axis.bmp");

	//10.Delauney三角网提取
	TmpDistanceTransform32b("inputBMP/point.bmp", octmp, "processBMP/point_dis.bmp", "processBMP/point_loc.bmp", 0xFF);
	getDelauney("inputBMP/point.bmp",  octmp, "outputBMP/point_del.bmp");

	delete octmp;
	printf("succeed!");
}


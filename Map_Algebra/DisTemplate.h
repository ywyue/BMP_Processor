#pragma once
#include<math.h>
class CDistanceTemplet    //æ‡¿Îƒ£∞Âª˘¿‡
{
public:
	virtual char Get0ffX(unsigned i) = 0;
	virtual char Get0ffY(unsigned i) = 0;
	virtual float GetDis(unsigned i) = 0;
	virtual unsigned GetSize() = 0;
};
static char         ManhattonTmp0ffX[13] = { 0, -1, 0, 1,    -2,-1, 0, 1, 2,   -1,0,1,0 };
static char         ManhattonTmp0ffY[13] = { -2,-1,-1,-1,     0, 0, 0, 0, 0,    1,1,1,2 };
static unsigned char ManhattonTmpDis[13] = { 2, 2, 1, 2,      2, 1, 0, 1, 2,    2,1,2,2 };

class CManHattanTmp :public CDistanceTemplet
{
private:
	char *m_0ffX;//[13] = {0,  -1,0,1,  -2,-1,0,1,2,  -1,0,1,  0};
	char *m_0ffY;//[13] = {-2, -1,-1,-1,  0,0,0,0,0,  1,1,1, 2};
	unsigned char *m_Dis;//[13] = {2,  2,1,2,  2,1,0,1,2,  2,1,2,  2};

protected:
public:
	CManHattanTmp() {
		m_0ffX = ManhattonTmp0ffX;
		m_0ffY = ManhattonTmp0ffY;
		m_Dis = ManhattonTmpDis;
	}
	virtual char Get0ffX(unsigned i) { return m_0ffX[i]; };
	virtual char Get0ffY(unsigned i) { return m_0ffY[i]; };
	virtual float GetDis(unsigned i) { return m_Dis[i]; };
	virtual unsigned GetSize() { return 13; };
};

class CEuTmp13 :public CDistanceTemplet
{
private:
	float m_Dis[169];
public:
	CEuTmp13()
	{
		for (unsigned i = 0; i < 169; i++)
		{
			float x = Get0ffX(i);
			float y = Get0ffY(i);
			m_Dis[i] = sqrt(x*x + y * y);
		}
	};
	virtual char Get0ffX(unsigned i) { return i % 13 - 6; };
	virtual char Get0ffY(unsigned i) { return i / 13 - 6; };
	virtual float GetDis(unsigned i) { return m_Dis[i]; };
	virtual unsigned GetSize() { return 13; };
};

class CEu5Tmp : public CDistanceTemplet
{
private:
	float m_DisTmp[25];
public:
	CEu5Tmp()
	{
		for (unsigned i = 0; i < 25; i++)
		{
			float x = (float)Get0ffX(i);
			float y = (float)Get0ffY(i);
			m_DisTmp[i] = sqrt(x*x + y * y);
		}
	};
	virtual unsigned GetSize() { return 25; };
	virtual char Get0ffX(unsigned i) { return i % 5 - 2; };
	virtual char Get0ffY(unsigned i) { return i / 5 - 2; };
	virtual float GetDis(unsigned i) { return m_DisTmp[i]; };
};


static char            OctagonTemOffX[37] = { 1,  0,-1, 2,  1,0, -1, -2, 3, 2,  1, 0,-1,-2,-3,-3,-2,-1,0,1,2,3,3,2,1,0,-1,-2,-3,2,1,0,-1,-2,1,0,-1 };
static char            OctagonTemOffY[37] = { -3, -3,-3,-2, -2, -2, -2, -2, -1, -1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,1,1,1,1,1,1,1,2,2,2,2,2,3,3,3 };
static unsigned char    OctagonTemDis[37] = { 3,3,3,3,2,2,2,3,3,2,2,1,2,2,3,3,2,1,0,1,2,3,3,2,2,1,2,2,3,3,2,2,2,3,3,3,3 };

class COctagonTmp : public CDistanceTemplet
{
private:
	char *m_0ffX;
	char *m_0ffY;
	unsigned char *m_Dis;
protected:
public:
	COctagonTmp()
	{
		m_0ffX = OctagonTemOffX;
		m_0ffY = OctagonTemOffY;
		m_Dis = OctagonTemDis;
	}

	virtual char Get0ffX(unsigned i) { return m_0ffX[i]; };
	virtual char Get0ffY(unsigned i) { return m_0ffY[i]; };
	virtual float GetDis(unsigned i) { return m_Dis[i]; };
	virtual unsigned GetSize() { return 37; };
};


static char         Mean0ffX[9] = {  1,  0, -1,    -1, 0, 1,    1, 0, -1 };
static char         Mean0ffY[9] = { -1, -1, -1,     0, 0, 0,    1, 1,  1 };
static unsigned char MeanTmpDis[9] = { 1, 1, 1,     1, 1, 1,    1, 1,  1 };

class CMeanTmp :public CDistanceTemplet
{
private:
	char *m_0ffX;
	char *m_0ffY;
	unsigned char *m_Dis;

protected:
public:
	CMeanTmp() {
		m_0ffX = Mean0ffX;
		m_0ffY = Mean0ffY;
		m_Dis = MeanTmpDis;
	}
	virtual char Get0ffX(unsigned i) { return m_0ffX[i]; };
	virtual char Get0ffY(unsigned i) { return m_0ffY[i]; };
	virtual float GetDis(unsigned i) { return m_Dis[i]; };
	virtual unsigned GetSize() { return 9; };
};

class Point
{
public:
	int x;
	int y;
	unsigned char color;
};
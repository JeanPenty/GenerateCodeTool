// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"	
#include <helper/SMenuEx.h>

CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
}

CMainDlg::~CMainDlg()
{
}
// #include <iostream>
// template <class T> void f(T) {
// 	static int i = 0;
// 	int k = ++i;
// 	//std::cout << ++i;
// 
// 	int jjj = 0;
// }

#include <vector>
#include <set>
#include <algorithm>
#include <cmath>
using namespace std;

#define VERETXEPS 0.002
typedef struct tagStruVertex
{
	double x;
	double y;

	tagStruVertex(double x = 0, double y = 0) : x(x), y(y) {}

	// 重载运算符，用于排序和去重
	bool operator<(const tagStruVertex& other) const {
		if (x != other.x) return x < other.x;
		return y < other.y;
	}

	bool operator==(const tagStruVertex& other) const {
		return fabs(x - other.x) < 1e-9 && fabs(y - other.y) < 1e-9;
	}

	bool operator!=(const tagStruVertex& other) const {
		if (x != other.x || y != other.y) 
			return true;
		return false;
	}

	double distanceTo(
		const tagStruVertex& point) const
	{
		double disX = x - point.x;
		double disY = y - point.y;
		return sqrt(disX * disX + disY * disY);
	}
}Vertex;

typedef struct tagStruSegment
{
	Vertex start;
	Vertex end;

	tagStruSegment(Vertex p1, Vertex p2) : start(p1), end(p2) {}
	tagStruSegment() {}

	// 重载运算符，用于 set 去重
	bool operator<(const tagStruSegment& other) const {
		if (start == other.start) return end < other.end;
		return start < other.start;
	}

	bool operator==(const tagStruSegment& other) const {
		if ((start == other.start && end == other.end) || (start == other.end && end == other.start))
			return true;
		else
			return false;
	}
}Segment;

// 计算叉积
double cross(const Vertex& o, const Vertex& a, const Vertex& b) {
	return (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
}

// 判断点是否在线段上
bool IsPointOnSegment(const Vertex& p, const Segment& seg) {
	double crossProd = cross(seg.start, seg.end, p);
	if (fabs(crossProd) > 1e-9) return false; // 不在直线上
	double minX = min(seg.start.x, seg.end.x);
	double maxX = max(seg.start.x, seg.end.x);
	double minY = min(seg.start.y, seg.end.y);
	double maxY = max(seg.start.y, seg.end.y);
	return (p.x >= minX - 1e-9 && p.x <= maxX + 1e-9) &&
		(p.y >= minY - 1e-9 && p.y <= maxY + 1e-9);
}

// 判断两条线段是否相交
bool IsCross(const Segment& seg1, const Segment& seg2, Vertex& intersect) {
	Vertex a = seg1.start, b = seg1.end;
	Vertex c = seg2.start, d = seg2.end;

	double cross1 = cross(a, b, c);
	double cross2 = cross(a, b, d);
	double cross3 = cross(c, d, a);
	double cross4 = cross(c, d, b);

	// 判断是否相交
	if (((cross1 * cross2) < 0) && ((cross3 * cross4) < 0)) {
		// 计算交点
		double t = cross3 / (cross3 - cross4);
		intersect.x = a.x + t * (b.x - a.x);
		intersect.y = a.y + t * (b.y - a.y);
		return true;
	}

	// 处理共线情况
	if (fabs(cross1) < 1e-9 && IsPointOnSegment(c, seg1)) { intersect = c; return true; }
	if (fabs(cross2) < 1e-9 && IsPointOnSegment(d, seg1)) { intersect = d; return true; }
	if (fabs(cross3) < 1e-9 && IsPointOnSegment(a, seg2)) { intersect = a; return true; }
	if (fabs(cross4) < 1e-9 && IsPointOnSegment(b, seg2)) { intersect = b; return true; }

	return false;
}

// 打断线段
vector<Segment> AdjustSegment(const Segment& seg, const set<Vertex>& pts) {
	vector<Segment> vecSegments;
	vector<Vertex> points;

	// 将起点和终点加入
	points.push_back(seg.start);
	for (const auto& p : pts) {
		if (IsPointOnSegment(p, seg)) {
			points.push_back(p);
		}
	}
	points.push_back(seg.end);

	// 按x坐标排序（或按y坐标，取决于线段方向）
	sort(points.begin(), points.end());

	// 生成小线段
	for (int i = 1; i < points.size(); ++i) {
		vecSegments.push_back(Segment(points[i - 1], points[i]));
	}

	return vecSegments;
}

vector<Segment> AdjustSegments(const vector<Segment>& segments) {
	vector<Segment> vecSegments;

	for (int i = 0; i < segments.size(); ++i) {
		set<Vertex> intersections;

		// 查找当前线段与其他线段的交点
		for (int j = 0; j < segments.size(); ++j) {
			if (i == j) continue;
			Vertex intersect;
			if (IsCross(segments[i], segments[j], intersect)) {
				intersections.insert(intersect);
			}
		}

		// 打断当前线段
		vector<Segment> lines = AdjustSegment(segments[i], intersections);
		vecSegments.insert(vecSegments.end(), lines.begin(), lines.end());
	}
	// 使用 set 去重
	set<Segment> uniqueSegments(vecSegments.begin(), vecSegments.end());
	return vector<Segment>(uniqueSegments.begin(), uniqueSegments.end());
}


typedef struct _tagAssociatedLine
{
	Vertex pt;
	std::vector<Segment> vecLine;
	_tagAssociatedLine() : pt{ 0.,0. } {}
}AssociatedLine;

//获取下一条线段
Segment  GetNextLine(const Segment& currentLine, std::vector<AssociatedLine>& vecAssociated, bool& bValuable)
{
	bValuable = true;
	Segment nextLine = {};
	nextLine.start = { 0., 0., };
	nextLine.end = { 0., 0., };
	for (int i = 0; i < vecAssociated.size(); i++)
	{
		AssociatedLine& associatedLine = vecAssociated[i];
		if (currentLine.end == associatedLine.pt) //找到与当前线段终点相等的交点pt的关联对象
		{
			for (int j = 0; j < associatedLine.vecLine.size(); j++)  //遍历交点pt关联对象中的关联线段
			{
				std::vector<Segment>& lines = associatedLine.vecLine;
				if (lines[j].start ==  currentLine.end && lines[j].end == currentLine.start)
				{
					//从象限角排序数组中找到方向线段的下一个位置的线段
					if (j == (lines.size() - 1))
						nextLine = lines[0];
					else
						nextLine = lines[j + 1];
					return nextLine;
				}
			}
		}
	}
	bValuable = false;
	return nextLine;
}

void RemoveisolatedLine(const std::vector<Segment>& input, std::vector<Segment>& output)
{
	//先处理两端都孤立的线段
	std::vector<Segment> vecTmp;
	if (input.size() < 2)
		return;
	for (int i = 0; i < input.size(); i++)
	{
		for (int j = 0; j < input.size(); j++)
		{
			if (i == j)
				continue;
			Vertex point;
			if (IsCross(input[i], input[j], point)) {
				vecTmp.push_back(input[i]);
				break;
			}
		}
	}

	//再处理一端孤立的线段
	for (int i = 0; i < vecTmp.size(); i++)
	{
		std::vector<Vertex> crossPoint;
		for (int j = 0; j < vecTmp.size(); j++)
		{
			Vertex point;
			if (IsCross(vecTmp[i], vecTmp[j], point)) {
				crossPoint.push_back(point);
			}
		}

		bool bEqual = false;
		for (int m = 0; m < crossPoint.size(); m++)
		{
			if (vecTmp[i].start == crossPoint[m])
			{
				bEqual = true;
				break;
			}
		}
		if (bEqual)
		{
			for (int n = 0; n < crossPoint.size(); n++)
			{
				if (vecTmp[i].end == crossPoint[n])
				{
					output.push_back(vecTmp[i]);
					break;
				}
			}
		}
	}
}

void GetCrossPoints(const std::vector<Segment>& input, std::vector<Vertex>& vecCrossPoint)
{
	for (int i = 0; i < input.size(); i++)
	{
		vecCrossPoint.push_back(input[i].start);
		vecCrossPoint.push_back(input[i].end);
	}
	//去除重复的点
	int n = vecCrossPoint.size();
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < i; j++) {
			if (vecCrossPoint[i] == vecCrossPoint[j]) {
				n--;
				for (int k = i; k < n; k++) {
					vecCrossPoint[k] = vecCrossPoint[k + 1];
				}
				i--;
			}
		}
	}

	vecCrossPoint.erase(vecCrossPoint.begin() + n, vecCrossPoint.end());
}

void GetDirectedLine(const std::vector<Segment>& input, std::vector<Segment>& vecDirectedLine)
{
	for (int i = 0; i < input.size(); i++)
	{
		vecDirectedLine.push_back(input[i]);
		Segment temp;
		temp.start = input[i].end;
		temp.end = input[i].start;
		vecDirectedLine.push_back(temp);
	}
}

std::vector<Segment> SortLine(std::vector<Segment>& lines)
{
	if (lines.size() < 2)
		return lines;
	for (int i = 0; i < lines.size() - 1; i++)
	{
		int minIndex = i;
		double dxmin = lines[i].end.x - lines[i].start.x;
		double dymin = lines[i].end.y - lines[i].start.y;
		double dAnglemin = atan2(dymin, dxmin);
		dAnglemin = (180. / 3.141592652589793) * dAnglemin;
		if (abs(dAnglemin) <= 0.1)
			dAnglemin = 0.;
		if (dAnglemin < -0.1)
			dAnglemin += 360;
		for (size_t j = i + 1; j < lines.size(); j++)
		{
			double dx = lines[j].end.x - lines[j].start.x;
			double dy = lines[j].end.y - lines[j].start.y;
			double dAngle = atan2(dy, dx);
			dAngle = (180. / 3.141592652589793) * dAngle;
			if (abs(dAngle) <= 0.1)
				dAngle = 0.;
			if (dAngle < -0.1)
				dAngle += 360;
			if (dAngle < dAnglemin)
			{
				minIndex = j;
				dAnglemin = dAngle;
			}
		}
		std::swap(lines[minIndex], lines[i]);
	}
	return lines;
}

std::vector<Segment> GetAssociated(const std::vector<Segment>& DirectedLine, const Vertex& pt)
{
	std::vector<Segment> AssociatedLine;
	//找到与点pt关联的线段
	for (int j = 0; j < DirectedLine.size(); ++j)
	{
		if (DirectedLine[j].start == pt)
		{
			AssociatedLine.push_back(DirectedLine[j]);
		}
	}
	std::vector<Segment> vecSortedLine;//排序后的数组

	//对关联线段根据象限角大小进行排序
	vecSortedLine = SortLine(AssociatedLine);
	return vecSortedLine;
}

void GetAssociatedLine(const std::vector<Segment>& DirectedLine, const std::vector<Vertex>& crossPoint, std::vector<AssociatedLine>& vecAssociatedLine)
{
	//找到每一个交点的关联线段
	for (int i = 0; i < crossPoint.size(); ++i)
	{
		AssociatedLine AssociatedEnt;
		AssociatedEnt.pt = crossPoint[i]; 
		AssociatedEnt.vecLine = GetAssociated(DirectedLine, crossPoint[i]);
		vecAssociatedLine.push_back(AssociatedEnt);
	}
}


void GetClosedAreas(const std::vector<Segment>& DirectedLine, std::vector<AssociatedLine>& vecAssociated, std::vector<std::vector<Segment>>& vecAllClosedAreas)
{
	//提取任意一条有向线段作为提取封闭图形的第一条线段
	for (int i = 0; i < DirectedLine.size(); i++)
	{
		bool bIsClosed = true;
		std::vector<Segment> vecClosedArea;
		vecClosedArea.push_back(DirectedLine[i]);
		bool bValuable = true;
		Segment m_nextLine;
		m_nextLine = GetNextLine(DirectedLine[i], vecAssociated, bValuable);

		if (!bValuable)
			continue;

		vecClosedArea.push_back(m_nextLine);
		while (m_nextLine.start != DirectedLine[i].start && m_nextLine.end != DirectedLine[i].end)
		{
			m_nextLine = GetNextLine(m_nextLine, vecAssociated, bValuable);
			if (!bValuable)
			{
				bIsClosed = false;
				break;
			}
			else
				vecClosedArea.push_back(m_nextLine);
		}
		if (bIsClosed)
			vecAllClosedAreas.push_back(vecClosedArea);//获得以DirectedLine[i]为起始线段的封闭区域
	}
}

bool IsOverlap(const std::vector<Segment>& AreaLeft, const std::vector<Segment>& AreaRight)
{
	//若两个闭合区域重叠,其size()必然相同
	if (AreaLeft.size() != AreaRight.size())
		return false;
	for (unsigned int i = 0; i < AreaLeft.size(); i++)
	{
		Segment m_lineLeft = AreaLeft[i];
		bool bEqualLine = false;
		for (unsigned int j = 0; j < AreaRight.size(); j++)
		{
			if (m_lineLeft.start == AreaRight[j].start && m_lineLeft.end == AreaRight[j].end ||
				m_lineLeft.start == AreaRight[j].end && m_lineLeft.end == AreaRight[j].start)
			{
				bEqualLine = true;
			}
		}
		if (bEqualLine == false)
			return false; //只要有一条线段不相同,必然不重合
	}
	return true;
}

std::vector<std::vector<Segment>>& RemoveOverlapArea(std::vector<std::vector<Segment>>& poly)
{
	int n = poly.size();
	if (n < 2) //只有一个闭合区域
		return poly;

	//去除重叠的区域
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < i; j++)
		{
			if (IsOverlap(poly[i], poly[j]))
			{
				n--;
				for (int k = i; k < n; k++)
				{
					poly[k] = poly[k + 1];
				}
				i--;
			}
		}
	}

	std::vector<std::vector<Segment>>::iterator it1, it2;
	it1 = poly.begin() + n;
	it2 = poly.end();
	poly.erase(it1, it2);
	return poly;
}

//判断点集合pointsA是否包含在点集合pointsB中(B的点数量大于A的点数量)
bool IsContainPoints(std::vector<Vertex>& pointsA, std::vector<Vertex>& pointsB)
{
	int nSizeA = pointsA.size();
	int nSizeB = pointsB.size();
	//若B中包含有A,B的顶点数量要大于A的顶点数量
	if (nSizeA >= nSizeB)
	{
		return false;
	}
	for (int i = 0; i < nSizeA; i++) //遍历小区域A的顶点
	{
		bool isEqual = false;
		for (int j = 0; j < nSizeB; j++) //遍历大区域B的顶点
		{
			if (pointsA[i] == pointsB[j])
			{
				isEqual = true;
				break;
			}
		}
		if (!isEqual)
		{
			return false; //只要A中的任意顶点,在B中找不到,则说明A不包含在B中
		}
	}

	return true; //在B中可以找到A中所有的顶点,说明A包含在B中
}

//判断点是否在闭合区域内
bool IsPointInPolygon(const Vertex& p, std::vector<Vertex>& points, double des = 0.01)
{
	int nCount = points.size();
	// 交点个数  
	int nCross = 0;
	for (int i = 0; i < nCount; i++)
	{
		Vertex p1 = points[i];
		Vertex p2 = points[(i + 1) % nCount];// 点P1与P2形成连线  

		Segment seg(p1, p2);
		if (IsPointOnSegment(p, seg))
			return false;

		if (abs(p1.y - p2.y) <= abs(des)) continue;
		if (p.y < min(p1.y, p2.y) && abs(p.y - min(p1.y, p2.y)) > abs(des))
			continue;
		if (p.y >= max(p1.y, p2.y) || abs(p.y - max(p1.y, p2.y)) <= abs(des))
			continue;
		// 求交点的x坐标（由直线两点式方程转化而来）   
		double x = (double)(p.y - p1.y) * (double)(p2.x - p1.x) / (double)(p2.y - p1.y) + p1.x;

		// 只统计p1p2与p向右射线的交点  
		if (x > p.x)
			nCross++;
	}

	if ((nCross % 2) == 1)
		return true;
	else
		return false;
}

//判断多边形B的所有边的中心点存在在区域A内部的情况（不在区域A边上）
bool IsContainLine(const std::vector<Segment>& linesA, const std::vector<Segment>& linesB)
{
	if (linesA.size() < linesB.size())
		return false;

	bool bIsContainLine = false;
	for (int i = 0; i < linesB.size(); i++)
	{
		auto& lineB = linesB[i];
		Vertex centerPoint;
		centerPoint.x = (lineB.start.x + lineB.end.x) / 2.;
		centerPoint.y = (lineB.start.y + lineB.end.y) / 2.;
		std::vector<Vertex> PloyVertex;
		GetCrossPoints(linesA, PloyVertex);
		if (IsPointInPolygon(centerPoint, PloyVertex))
		{
			bIsContainLine = true;
			break;
		}
	}
	return bIsContainLine;
}

//移除大区域包含小区域的图形
//只有一种情况：大区域的交点包含小区域所有的交点
std::vector<std::vector<Segment>> GetRemoveContainPoly(const std::vector<std::vector<Segment>>& InPolys, std::vector<std::vector<Segment>>& outPolys)
{
	int nSize = InPolys.size();
	for (int i = 0; i < nSize; i++)
	{
		auto& m_AreaA = InPolys[i]; //获得区域A
		std::vector<Vertex> m_PointsA;
		GetCrossPoints(m_AreaA, m_PointsA);//获得区域A所有的顶点
		bool m_bContain = false;
		for (int j = 0; j < nSize; j++)
		{
			if (j == i)
				continue;
			auto& m_AreaB = InPolys[j]; //获得区域B
			std::vector<Vertex> m_PointsB;
			GetCrossPoints(m_AreaB, m_PointsB);//获得区域B的所有顶点
			//判断A中是否包含有其他区域B
			//判断方法：当A和B的顶点数不相同时,若区域A包含区域B的所有顶点,则可能包含，须进一步判断
			if (IsContainPoints(m_PointsB, m_PointsA)) //A中可能包含有B
			{
				//再判断区域B的所有边中点是否存在在区域A内部，如存在，删除A
				if (IsContainLine(m_AreaA, m_AreaB))
				{
					m_bContain = true;
					break;
				}
			}
		}
		if (!m_bContain) //区域A中没有包含其他区域
			outPolys.push_back(m_AreaA);
	}
	return outPolys;
}

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	/*
	vector<Segment> segments = {
		Segment(Vertex(0, 0), Vertex(30, 0)),
		Segment(Vertex(0, 10), Vertex(30, 10)),
		Segment(Vertex(0, 20), Vertex(30, 20)),
		Segment(Vertex(0, 30), Vertex(30, 30)),
		Segment(Vertex(0, 0), Vertex(0, 30)),
		Segment(Vertex(10, 0), Vertex(10, 30)),
		Segment(Vertex(20, 0), Vertex(20, 30)),
		Segment(Vertex(30, 0), Vertex(30, 30))
	};

	vector<Segment> vecSegments = AdjustSegments(segments);
	//过滤掉长短为0的线段
	for (auto it = vecSegments.begin(); it != vecSegments.end(); ) {
		if (it->start.distanceTo(it->end) == 0)
		{
			it = vecSegments.erase(it);  // 如果匹配，删除元素并更新迭代器
		}
		else {
			++it;  // 如果不匹配，继续遍历
		}
	}

	//移除孤立的线段（两端孤立跟一端孤立）
	std::vector<Segment> vecNonisolatedLines;
	RemoveisolatedLine(vecSegments, vecNonisolatedLines);

	//获取所有的交点
	std::vector<Vertex> vecCrossPoints;
	GetCrossPoints(vecNonisolatedLines, vecCrossPoints);

	//生成有向线段
	std::vector<Segment> vecDirectedLines;
	GetDirectedLine(vecNonisolatedLines, vecDirectedLines);

	//交点与线段关联
	std::vector<AssociatedLine> vecAssociatedLines;
	GetAssociatedLine(vecDirectedLines, vecCrossPoints, vecAssociatedLines);

	//生成闭合区域
	std::vector<std::vector<Segment>> vecPolys;
	GetClosedAreas(vecDirectedLines, vecAssociatedLines, vecPolys);

	//去除重叠的区域
	RemoveOverlapArea(vecPolys);

	//去除内部仍包含有图形的区域
	std::vector<std::vector<Segment>> RemoveContainPoly;
	GetRemoveContainPoly(vecPolys, RemoveContainPoly);
	//去除包含内部顶点的多边形
	std::vector<std::vector<Segment>> vecResultPolys;
	for (int i = 0; i < RemoveContainPoly.size(); i++)
	{
		bool bEnable = true;
		std::vector<Vertex> PloyVertex;
		GetCrossPoints(RemoveContainPoly[i], PloyVertex);
		for (int j = 0; j < vecCrossPoints.size(); j++)
		{
			Vertex currentPoint = vecCrossPoints[j];
			if (IsPointInPolygon(currentPoint, PloyVertex))
			{
				bEnable = false;
				break;
			}
		}
		if (bEnable)
			vecResultPolys.push_back(RemoveContainPoly[i]);
	}
	*/


	SComboBox* pCbxCodeType = FindChildByName2<SComboBox>(L"cbx_codetype");
	{
 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_CODE11");  //Code 11
 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_C25STANDARD");  //Standard Code 2 of 5
 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_C25INTER");  //Interleaved 2 of 5
 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_C25IATA");  //Code 2 of 5 IATA
 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_C25LOGIC");  //Code 2 of 5 Data Logic
 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_C25IND");  //Code 2 of 5 Industrial
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_CODE39");  //Code 3 of 9 (Code 39)
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_EXCODE39");  //Extended Code 3 of 9 (Code 39+)
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_EANX");  //EAN (EAN-2, EAN-5, EAN-8 and EAN-13)
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_EANX_CHK");  //EAN + Check Digit
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_GS1_128");  //GS1-128 (UCC.EAN-128)
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_CODABAR");  //Codabar
 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_CODE128");  //Code 128 (automatic Code Set switching)
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DPLEIT");  //Deutsche Post Leitcode
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DPIDENT");  //Deutsche Post Identcode
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_CODE16K");  //Code 16K
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_CODE49");  //Code 49
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_CODE93");  //Code 93
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_FLAT");  //Flattermarken
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DBAR_OMN");  //GS1 DataBar Omnidirectional (including GS1 DataBar Truncated)
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DBAR_LTD");  //GS1 DataBar Limited
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DBAR_EXP");  //GS1 DataBar Expanded
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_TELEPEN");  //Telepen Alpha
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_UPCA");  //UPC-A
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_UPCA_CHK");  //UPC-A + Check Digit
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_UPCE");  //UPC-E
		pCbxCodeType->GetListBox()->AddString(L"BARCODE_UPCE_CHK");  //UPC-E + Check Digit
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_POSTNET");  //POSTNET
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_MSI_PLESSEY");  //MSI Plessey
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_FIM");  //FIM
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_LOGMARS");  //LOGMARS
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_PHARMA");  //Pharmacode One-Track
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_PZN");  //PZN
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_PHARMA_TWO");  //Pharmacode Two-Track
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_CEPNET");  //Brazilian CEPNet
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_PDF417");  //PDF417
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_PDF417COMP");  //Compact PDF417 (Truncated PDF417)
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_MAXICODE");  //MaxiCode
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_QRCODE");  //QR Code
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_CODE128AB");  //Code 128 (Suppress Code Set C)
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_AUSPOST");  //Australia Post Standard Customer
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_AUSREPLY");  //Australia Post Reply Paid
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_AUSROUTE");  //Australia Post Routing
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_AUSDIRECT");  //Australia Post Redirection
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_ISBNX");  //ISBN (EAN-13 with verification stage)
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_RM4SCC");  //Royal Mail 4-State Customer Code (RM4SCC)
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DATAMATRIX");  //Data Matrix (ECC200)
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_EAN14");  //EAN-14
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_VIN");  //Vehicle Identification Number
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_CODABLOCKF");  //Codablock-F
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_NVE18");  //NVE-18 (SSCC-18)
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_JAPANPOST");  //Japanese Postal Code
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_KOREAPOST");  //Korea Post
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DBAR_STK");  //GS1 DataBar Stacked
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DBAR_OMNSTK");  //GS1 DataBar Stacked Omnidirectional
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DBAR_EXPSTK");  //GS1 DataBar Expanded Stacked
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_PLANET");  //PLANET
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_MICROPDF417");  //MicroPDF417
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_USPS_IMAIL");  //USPS Intelligent Mail (OneCode)
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_PLESSEY");  //UK Plessey
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_TELEPEN_NUM");  //Telepen Numeric
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_ITF14");  //ITF-14
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_KIX");  //Dutch Post KIX Code
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_AZTEC");  //Aztec Code
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DAFT");  //	DAFT Code
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DPD");  //	DPD Code
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_MICROQR");  //Micro QR Code
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_HIBC_128");  //HIBC Code 128
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_HIBC_39");  //HIBC Code 39
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_HIBC_DM");  //HIBC Data Matrix ECC200
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_HIBC_QR");  //HIBC QR Code
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_HIBC_PDF");  //HIBC PDF417
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_HIBC_MICPDF");  //HIBC MicroPDF417
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_HIBC_BLOCKF");  //HIBC Codablock-F
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_HIBC_AZTEC");  //HIBC Aztec Code
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DOTCODE");  //DotCode
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_HANXIN");  //Han Xin (Chinese Sensible) Code
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_MAILMARK_2D");  //Royal Mail 2D Mailmark (CMDM) (Data Matrix)
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_MAILMARK_4S");  //Royal Mail 4-State Mailmark
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_AZRUNE");  //Aztec Runes
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_CODE32");  //Code 32
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_EANX_CC");  //GS1 Composite Symbol with EAN linear component
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_GS1_128_CC");  //GS1 Composite Symbol with GS1-128 linear component
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DBAR_OMN_CC");  //GS1 Composite Symbol with GS1 DataBar Omnidirectional linear component
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DBAR_LTD_CC");  //GS1 Composite Symbol with GS1 DataBar Limited linear component
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DBAR_EXP_CC");  //GS1 Composite Symbol with GS1 DataBar Expanded linear component
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_UPCA_CC");  //GS1 Composite Symbol with UPC-A linear component
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_UPCE_CC");  //GS1 Composite Symbol with UPC-E linear component
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DBAR_STK_CC");  //GS1 Composite Symbol with GS1 DataBar Stacked component
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DBAR_OMNSTK_CC");  //GS1 Composite Symbol with GS1 DataBar Stacked Omnidirectional component
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_DBAR_EXPSTK_CC");  //GS1 Composite Symbol with GS1 DataBar Expanded Stacked component
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_CHANNEL");  //Channel Code
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_CODEONE");  //Code One
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_GRIDMATRIX");  //Grid Matrix
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_UPNQR");  //UPNQR (Univerzalnega Plačilnega Naloga QR)
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_ULTRA");  //Ultracode
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_RMQR");  //Rectangular Micro QR Code (rMQR)
// 		pCbxCodeType->GetListBox()->AddString(L"BARCODE_BC412");  //IBM BC412 (SEMI T1-95)
		pCbxCodeType->SetCurSel(0);
	}

	{
		SEdit* pControl = FindChildByName2<SEdit>(L"edit_mm2pix");
		pControl->SetWindowTextW(L"20");
	}
	{
		SEdit* pControl = FindChildByName2<SEdit>(L"edit_xdpi");
		pControl->SetWindowTextW(L"300");
	}
	{
		SEdit* pControl = FindChildByName2<SEdit>(L"edit_ydpi");
		pControl->SetWindowTextW(L"300");
	}
	{
		SEdit* pControl = FindChildByName2<SEdit>(L"edit_content");
		pControl->SetWindowTextW(L"012345");
	}
	{
		SEdit* pControl = FindChildByName2<SEdit>(L"edit_width");
		pControl->SetWindowTextW(L"200");
	}
	{
		SEdit* pControl = FindChildByName2<SEdit>(L"edit_height");
		pControl->SetWindowTextW(L"80");
	}

	return 0;
}

void CMainDlg::OnLanguageBtnCN()
{
	OnLanguage(1);
}
void CMainDlg::OnLanguageBtnJP()
{
	OnLanguage(0);
}
void CMainDlg::OnLanguage(int nID)
{
	ITranslatorMgr *pTransMgr = SApplication::getSingletonPtr()->GetTranslator();
	SASSERT(pTransMgr);
	bool bCnLang = nID == 1;

	SXmlDoc xmlLang;
	if (SApplication::getSingletonPtr()->LoadXmlDocment(xmlLang, bCnLang ? _T("translator:lang_cn") : _T("translator:lang_jp")))
	{
		CAutoRefPtr<ITranslator> lang;
		pTransMgr->CreateTranslator(&lang);
		lang->Load(&xmlLang.root().child(L"language"), 1);//1=LD_XML
		TCHAR lngName[TR_MAX_NAME_LEN] = {0};
		lang->GetName(lngName);
        pTransMgr->SetLanguage(lngName);
		pTransMgr->InstallTranslator(lang);
        GetRoot()->SDispatchMessage(UM_SETLANGUAGE, 0, 0);
	}
}

void CMainDlg::OnBnClickGenerate()
{
	ContainerBox* pContainerBox = FindChildByName2<ContainerBox>(L"containerbox");

	//获取参数值
	{
		SEdit* pControl = FindChildByName2<SEdit>(L"edit_mm2pix");
		SStringW sstrVal = pControl->GetWindowTextW();

		pContainerBox->SetMM2Pix(sstrVal);
	}
	{
		SEdit* pControl = FindChildByName2<SEdit>(L"edit_xdpi");
		SStringW sstrVal = pControl->GetWindowTextW();

		pContainerBox->SetXDPI(sstrVal);
	}
	{
		SEdit* pControl = FindChildByName2<SEdit>(L"edit_ydpi");
		SStringW sstrVal = pControl->GetWindowTextW();

		pContainerBox->SetYDPI(sstrVal);
	}
	{
		SComboBox* pControl = FindChildByName2<SComboBox>(L"cbx_codetype");
		SStringW sstrVal = pControl->GetWindowTextW();

		pContainerBox->SetCodeType(sstrVal);
	}
	{
		SEdit* pControl = FindChildByName2<SEdit>(L"edit_content");
		SStringW sstrVal = pControl->GetWindowTextW();

		pContainerBox->SetContent(sstrVal);
	}
	{
		SEdit* pControl = FindChildByName2<SEdit>(L"edit_width");
		SStringW sstrVal = pControl->GetWindowTextW();

		pContainerBox->SetWidth(sstrVal);
	}
	{
		SEdit* pControl = FindChildByName2<SEdit>(L"edit_height");
		SStringW sstrVal = pControl->GetWindowTextW();

		pContainerBox->SetHeight(sstrVal);
	}

	pContainerBox->Invalidate();
}

//TODO:消息映射
void CMainDlg::OnClose()
{
	SNativeWnd::DestroyWindow();
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CMainDlg::OnRestore()
{
// 	SWindow* pMainCaption = FindChildByName2<SWindow>(L"main_caption");
// 	pMainCaption->SetAttribute(L"pos", L"12,12,12,12", FALSE);

	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
// 	SWindow* pMainCaption = FindChildByName2<SWindow>(L"main_caption");
// 	pMainCaption->SetAttribute(L"pos", L"0,0,0,0", FALSE);

	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);	
	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}


#include "stdafx.h"
#include "ContainerBox.h"

ContainerBox::ContainerBox(void)
{
	m_sstrMM2Pix = L"20";
	m_sstrXDPI = L"300";
	m_sstrYDPI = L"300";
	m_sstrCodeType = L"BARCODE_CODE128";
	m_sstrContent = L"012345";
	m_sstrWidth = L"200";
	m_sstrHeight = L"80";
}

ContainerBox::~ContainerBox(void)
{

}

void ContainerBox::SetMM2Pix(SStringW& sstrMM2Pix)
{
	m_sstrMM2Pix = sstrMM2Pix;
}

void ContainerBox::SetXDPI(SStringW& sstrXDPI)
{
	m_sstrXDPI = sstrXDPI;
}

void ContainerBox::SetYDPI(SStringW& sstrYDPI)
{
	m_sstrYDPI = sstrYDPI;
}

void ContainerBox::SetCodeType(SStringW& sstrCodeType)
{
	m_sstrCodeType = sstrCodeType;
}

void ContainerBox::SetContent(SStringW& sstrContnet)
{
	m_sstrContent = sstrContnet;
}

void ContainerBox::SetWidth(SStringW& sstrWidth)
{
	m_sstrWidth = sstrWidth;
}

void ContainerBox::SetHeight(SStringW& sstrHeight)
{
	m_sstrHeight = sstrHeight;
}

void ContainerBox::OnPaint(IRenderTarget* pRT)
{
	SStringW sstrFontFormat = L"face:微软雅黑,bold:0,italic:0,underline:0,strike:0,size:16";
	SOUI::IFontPtr pFont = SFontPool::GetFont(sstrFontFormat, GetScale());
	pRT->SelectObject(pFont);

	std::string strMM2Pix = S_CW2A(m_sstrMM2Pix);
	std::string strXDPI = S_CW2A(m_sstrXDPI);
	std::string strYDPI = S_CW2A(m_sstrYDPI);
	std::string strWidth = S_CW2A(m_sstrWidth);
	std::string strHeight = S_CW2A(m_sstrHeight);

	double dMM2Pix = std::stod(strMM2Pix.c_str());
	int nXDPI = std::stoi(strXDPI.c_str());
	int nYDPI = std::stoi(strYDPI.c_str());
	double dWidth = std::stod(strWidth.c_str());
	double dHeight = std::stod(strHeight.c_str());

	int nWid = (int)(dWidth * dMM2Pix) / 10;
	int nHei = (int)(dHeight * dMM2Pix) / 10;

	CRect rcClient = GetClientRect();
	CPoint ptCenter = rcClient.CenterPoint();

	CRect rcCode;
	rcCode.left = ptCenter.x - nWid / 2;
	rcCode.right = ptCenter.x + nWid / 2;
	rcCode.top = ptCenter.y - nHei / 2;
	rcCode.bottom = ptCenter.y + nHei / 2;

	if (m_sstrCodeType == L"BARCODE_CODE128")
	{
		SStringW sstrSubset = m_sstrContent.GetAt(0);
		std::string strContent = S_CW2A(m_sstrContent);
		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_CODE128;
		symbol->input_mode |= EXTRA_ESCAPE_MODE; //数据编码格式

// 		if (sstrSubset == L"0")	symbol->option_1 = 0;
// 		if (sstrSubset == L"1")	symbol->option_1 = 1;
// 		if (sstrSubset == L"2")	symbol->option_1 = 2;
// 		if (sstrSubset == L"3")	symbol->option_1 = 3;

		//symbol->option_1 = 1;   //0 自动选择子集、1 强制使用子集A、2 强制使用子集B、3 强制使用子集C

		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}
			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			{//绘制文本
				SIZE szContent;
				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
				CRect rcText(rcCode);
				rcText.top = rcCode.bottom;
				rcText.bottom = rcText.top + szContent.cy;
				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			}
		}

		ZBarcode_Delete(symbol);
	}
	else if (m_sstrCodeType == L"BARCODE_CODE11")
	{//Code 11 是一种主要用于标识电信设备的条形码格式，支持数字（0-9）和短横线（-）字符。需要先判断文本是否符合规则
		/*计算校验位
		* 1 位校验码计算
		* 1.1 分配权重：从右到左（从最后一位字符开始），为每个字符分配一个权重，权重从 1 开始递增。
		* 1.2 计算加权和：将每个字符的值乘以其权重，然后求和。字符 - 的值为 10。
		* 1.3 计算校验码：将加权和除以 11，取余数作为校验码。如果余数为 10，则校验码为 -。
		* 1.4 附加校验码：将校验码附加到条码数据的末尾。
		* 2 位校验码计算
		* 如果启用 2 位校验码，则需要在 1 位校验码的基础上再计算一次校验码，计算出的结果即为第2位校验码，然后将第二位的校验码也附加到条码数据的末尾。
		*/

		int nCheckType = 1;  //0 不校验、1 一位校验位、2 二位校验位
		if (nCheckType == 1)
		{
			//从右到左分配权重
			SStringW sstrCheckOne;
			std::vector<int> vecCheckOne;
			int nLength = m_sstrContent.GetLength();
			for (int i = 0; i < nLength; i++)
			{
				SStringW sstrChar = m_sstrContent.GetAt(i);
				if (sstrChar == L"-")  vecCheckOne.push_back(10);
				else
				{
					int nChar = std::stoi(sstrChar.c_str());
					vecCheckOne.push_back(nChar);
				}
			}
			
			int nTmp = 0;
			for (int i = 0; i < vecCheckOne.size(); i++)
			{
				int nDataWeight = vecCheckOne.size() - i;
				nTmp += nDataWeight * vecCheckOne[i];
			}
			int nCheck = nTmp % 11;

			sstrCheckOne.Format(L"%d", nCheck);
			//m_sstrContent += sstrCheckOne;
		}

		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_CODE11;
		symbol->input_mode = DATA_MODE; //数据编码格式
		symbol->option_2 = 2;   //默认添加俩校验位、1 添加一个校验位、2 不添加校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			{//绘制文本
				SIZE szContent;
				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
				CRect rcText(rcCode);
				rcText.top = rcCode.bottom;
				rcText.bottom = rcText.top + szContent.cy;
				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_C25STANDARD")
	{
		/*
		* 标准 Code 25（也称为 Code 2 of 5 Standard）是一种简单的数字条形码，仅支持数字字符（0-9）。
		* option_1参数用来控制是否启用校验位	0：禁用校验位。1：启用校验位。默认为0
		* 
		* 校验位计算方法：
		* 从右向左，依次给每个数据位分配权重，奇数位权重为 3，偶数位权重为 1。
		* 将每个数据位的值乘以其对应的权重，然后将所有乘积相加。
		* 用 10 减去加权和的个位数，得到的结果即为校验码。如果加权和的个位数为 0，则校验码为 0。
		*/
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_C25STANDARD;
		symbol->input_mode = DATA_MODE; //数据编码格式
		symbol->option_2 = 0;   //0 禁用校验位、1 启用校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			{//绘制文本
				SIZE szContent;
				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
				CRect rcText(rcCode);
				rcText.top = rcCode.bottom;
				rcText.bottom = rcText.top + szContent.cy;
				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_C25INTER")
	{
		/*
		* 是一种高密度的数字条形码，仅支持偶数位数字字符（0-9）
		* option_1：是否启用校验位（Check Digit）。0：禁用校验位。1：启用校验位。默认值：0（禁用）。
		* 
		* 校验位计算方法：
		* 从右到左（从最后一位字符开始），为每个字符分配一个权重，权重交替为 3 和 1。
		* 将每个字符的值乘以其权重，然后求和。
		* 将加权和除以 10，取余数，然后用 10 减去余数。如果结果为 10，则校验码为 0。
		* 将校验码附加到条码数据的末尾。
		*/

		/*
		* 如原始数据为12345，不计算校验码则实际数据为012345（需要补全成偶数个数）
		* 如果需要计算校验位，计算出校验值为7，将校验位附加到原始数据则实际数据为123457  个数为偶数不需要在数据前补0
		*/
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_C25INTER;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			{//绘制文本
				SIZE szContent;
				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
				CRect rcText(rcCode);
				rcText.top = rcCode.bottom;
				rcText.bottom = rcText.top + szContent.cy;
				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_C25IATA")
	{
		/*
		* BARCODE_C25IATA 是 IATA 2 of 5 的条形码类型。IATA 2 of 5 是 Interleaved 2 of 5 的一种变体，主要用于国际航空运输协会（IATA）的行李标签。
		* option_1：是否启用校验位（Check Digit）。0：禁用校验位。1：启用校验位。
		* 
		* 校验位计算方法：
		* 从右到左（从最后一位字符开始），为每个字符分配一个权重，权重交替为 3 和 1。
		* 将每个字符的值乘以其权重，然后求和。
		* 将加权和除以 10，取余数，然后用 10 减去余数。如果结果为 10，则校验码为 0。
		* 将校验码附加到条码数据的末尾。
		*/
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_C25IATA;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			{//绘制文本
				SIZE szContent;
				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
				CRect rcText(rcCode);
				rcText.top = rcCode.bottom;
				rcText.bottom = rcText.top + szContent.cy;
				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_C25LOGIC")
	{
		/*
		* BARCODE_C25LOGIC 是 Code 25 Logic 的条形码类型。Code 25 Logic 是一种基于逻辑编码的条形码格式，主要用于特定的工业应用。
		* option_1：是否启用校验位（Check Digit）。0：禁用校验位。1：启用校验位。默认值：0（禁用）。
		* 
		* 校验位计算方法：
		* 从右到左（从最后一位字符开始），为每个字符分配一个权重，权重从 1 开始递增。
		* 将每个字符的值乘以其权重，然后求和。
		* 将加权和除以 10，取余数，然后用 10 减去余数。如果结果为 10，则校验码为 0。
		* 将校验码附加到条码数据的末尾。
		*/
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_C25LOGIC;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 1;   
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			{//绘制文本
				SIZE szContent;
				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
				CRect rcText(rcCode);
				rcText.top = rcCode.bottom;
				rcText.bottom = rcText.top + szContent.cy;
				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_C25IND")
	{
		/*
		* 
		*/
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_C25IND;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			{//绘制文本
				SIZE szContent;
				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
				CRect rcText(rcCode);
				rcText.top = rcCode.bottom;
				rcText.bottom = rcText.top + szContent.cy;
				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_CODE39")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_CODE39;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
// 			{//绘制文本
// 				SIZE szContent;
// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
// 				CRect rcText(rcCode);
// 				rcText.top = rcCode.bottom;
// 				rcText.bottom = rcText.top + szContent.cy;
// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_EXCODE39")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_EXCODE39;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_EANX")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		std::string strAppend;
		std::string strPre;
		size_t szPos = strContent.find_first_of("+");
		if (szPos != std::string::npos)
		{
			strPre = strContent.substr(0, szPos);
			strAppend = strContent.substr(szPos + 1);
		}

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_EANX;
		symbol->input_mode = DATA_MODE; //数据编码格式
		symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strPre.c_str(), strPre.size(), 0);
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		//int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strAppend.c_str(), strAppend.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_EANX_CHK")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_EANX_CHK;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_GS1_128")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_GS1_128;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_CODABAR")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_CODABAR;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_DPLEIT")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_DPLEIT;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_DPIDENT")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_DPIDENT;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_CODE16K")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_CODE16K;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_CODE49")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_CODE49;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_CODE93")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_CODE93;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_FLAT")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_FLAT;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_DBAR_OMN")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_DBAR_OMN;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_DBAR_LTD")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_DBAR_LTD;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_DBAR_EXP")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_DBAR_EXP;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_TELEPEN")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_TELEPEN;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_UPCA")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		//处理校验码
		/*
		* 如果输入的
		*/

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_UPCA;
		symbol->input_mode = DATA_MODE; //数据编码格式
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_UPCA_CHK")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_UPCA_CHK;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_UPCE")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_UPCE;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
	else if (m_sstrCodeType == L"BARCODE_UPCE_CHK")
	{
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_UPCE_CHK;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 禁用校验位、1 启用 1 位校验位、2 启用 2 位校验位
		int nRet = ZBarcode_Encode_and_Buffer_Vector(symbol, (unsigned char*)strContent.c_str(), strContent.size(), 0);
		if (nRet == 0)
		{
			ZBarcode_Print(symbol, 0);
			std::vector<CRect> vecBlackRect;
			if (symbol->vector)
			{
				struct zint_vector_rect* rect = symbol->vector->rectangles;
				while (rect)
				{
					CRect rcTmp;
					rcTmp.left = rect->x;
					rcTmp.top = rect->y;
					rcTmp.right = rcTmp.left + rect->width;
					rcTmp.bottom = rcTmp.top + rect->height;
					vecBlackRect.push_back(rcTmp);
					rect = rect->next;
				}
			}

			double nDrawUint = (double)rcCode.Width() / symbol->width;
			double nUint = (double)symbol->bitmap_width / symbol->width;
			std::vector<CRect> vecDrawBlack;  //绘制条码条的真实区域
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//绘制条码背景色
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//绘制条码
				CAutoRefPtr<IPath> path;
				GETRENDERFACTORY->CreatePath(&path);
				for (int i = 0; i < vecDrawBlack.size(); i++)
				{
					CRect rcBlack;
					rcBlack.left += vecDrawBlack[i].left + rcCode.left;
					rcBlack.top += vecDrawBlack[i].top + rcCode.top;
					rcBlack.right = rcBlack.left + vecDrawBlack[i].Width();
					rcBlack.bottom = rcBlack.top + vecDrawBlack[i].Height();
					path->addRect(rcBlack);
				}
				COLORREF clrFrgnd = GETCOLOR(L"RGB(0,0,0)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrFrgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillPath(path);
				pRT->SelectObject(oldbrush, NULL);
			}
			// 			{//绘制文本
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //文本整体的长度
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
}
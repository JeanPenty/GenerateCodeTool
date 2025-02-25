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
	SStringW sstrFontFormat = L"face:΢���ź�,bold:0,italic:0,underline:0,strike:0,size:16";
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
		symbol->input_mode |= EXTRA_ESCAPE_MODE; //���ݱ����ʽ

// 		if (sstrSubset == L"0")	symbol->option_1 = 0;
// 		if (sstrSubset == L"1")	symbol->option_1 = 1;
// 		if (sstrSubset == L"2")	symbol->option_1 = 2;
// 		if (sstrSubset == L"3")	symbol->option_1 = 3;

		//symbol->option_1 = 1;   //0 �Զ�ѡ���Ӽ���1 ǿ��ʹ���Ӽ�A��2 ǿ��ʹ���Ӽ�B��3 ǿ��ʹ���Ӽ�C

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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			{//�����ı�
				SIZE szContent;
				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
				CRect rcText(rcCode);
				rcText.top = rcCode.bottom;
				rcText.bottom = rcText.top + szContent.cy;
				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			}
		}

		ZBarcode_Delete(symbol);
	}
	else if (m_sstrCodeType == L"BARCODE_CODE11")
	{//Code 11 ��һ����Ҫ���ڱ�ʶ�����豸���������ʽ��֧�����֣�0-9���Ͷ̺��ߣ�-���ַ�����Ҫ���ж��ı��Ƿ���Ϲ���
		/*����У��λ
		* 1 λУ�������
		* 1.1 ����Ȩ�أ����ҵ��󣨴����һλ�ַ���ʼ����Ϊÿ���ַ�����һ��Ȩ�أ�Ȩ�ش� 1 ��ʼ������
		* 1.2 �����Ȩ�ͣ���ÿ���ַ���ֵ������Ȩ�أ�Ȼ����͡��ַ� - ��ֵΪ 10��
		* 1.3 ����У���룺����Ȩ�ͳ��� 11��ȡ������ΪУ���롣�������Ϊ 10����У����Ϊ -��
		* 1.4 ����У���룺��У���븽�ӵ��������ݵ�ĩβ��
		* 2 λУ�������
		* ������� 2 λУ���룬����Ҫ�� 1 λУ����Ļ������ټ���һ��У���룬������Ľ����Ϊ��2λУ���룬Ȼ�󽫵ڶ�λ��У����Ҳ���ӵ��������ݵ�ĩβ��
		*/

		int nCheckType = 1;  //0 ��У�顢1 һλУ��λ��2 ��λУ��λ
		if (nCheckType == 1)
		{
			//���ҵ������Ȩ��
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		symbol->option_2 = 2;   //Ĭ�������У��λ��1 ���һ��У��λ��2 �����У��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			{//�����ı�
				SIZE szContent;
				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		* ��׼ Code 25��Ҳ��Ϊ Code 2 of 5 Standard����һ�ּ򵥵����������룬��֧�������ַ���0-9����
		* option_1�������������Ƿ�����У��λ	0������У��λ��1������У��λ��Ĭ��Ϊ0
		* 
		* У��λ���㷽����
		* �����������θ�ÿ������λ����Ȩ�أ�����λȨ��Ϊ 3��ż��λȨ��Ϊ 1��
		* ��ÿ������λ��ֵ�������Ӧ��Ȩ�أ�Ȼ�����г˻���ӡ�
		* �� 10 ��ȥ��Ȩ�͵ĸ�λ�����õ��Ľ����ΪУ���롣�����Ȩ�͵ĸ�λ��Ϊ 0����У����Ϊ 0��
		*/
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_C25STANDARD;
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		symbol->option_2 = 0;   //0 ����У��λ��1 ����У��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			{//�����ı�
				SIZE szContent;
				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		* ��һ�ָ��ܶȵ����������룬��֧��ż��λ�����ַ���0-9��
		* option_1���Ƿ�����У��λ��Check Digit����0������У��λ��1������У��λ��Ĭ��ֵ��0�����ã���
		* 
		* У��λ���㷽����
		* ���ҵ��󣨴����һλ�ַ���ʼ����Ϊÿ���ַ�����һ��Ȩ�أ�Ȩ�ؽ���Ϊ 3 �� 1��
		* ��ÿ���ַ���ֵ������Ȩ�أ�Ȼ����͡�
		* ����Ȩ�ͳ��� 10��ȡ������Ȼ���� 10 ��ȥ������������Ϊ 10����У����Ϊ 0��
		* ��У���븽�ӵ��������ݵ�ĩβ��
		*/

		/*
		* ��ԭʼ����Ϊ12345��������У������ʵ������Ϊ012345����Ҫ��ȫ��ż��������
		* �����Ҫ����У��λ�������У��ֵΪ7����У��λ���ӵ�ԭʼ������ʵ������Ϊ123457  ����Ϊż������Ҫ������ǰ��0
		*/
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_C25INTER;
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			{//�����ı�
				SIZE szContent;
				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		* BARCODE_C25IATA �� IATA 2 of 5 �����������͡�IATA 2 of 5 �� Interleaved 2 of 5 ��һ�ֱ��壬��Ҫ���ڹ��ʺ�������Э�ᣨIATA���������ǩ��
		* option_1���Ƿ�����У��λ��Check Digit����0������У��λ��1������У��λ��
		* 
		* У��λ���㷽����
		* ���ҵ��󣨴����һλ�ַ���ʼ����Ϊÿ���ַ�����һ��Ȩ�أ�Ȩ�ؽ���Ϊ 3 �� 1��
		* ��ÿ���ַ���ֵ������Ȩ�أ�Ȼ����͡�
		* ����Ȩ�ͳ��� 10��ȡ������Ȼ���� 10 ��ȥ������������Ϊ 10����У����Ϊ 0��
		* ��У���븽�ӵ��������ݵ�ĩβ��
		*/
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_C25IATA;
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			{//�����ı�
				SIZE szContent;
				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		* BARCODE_C25LOGIC �� Code 25 Logic �����������͡�Code 25 Logic ��һ�ֻ����߼�������������ʽ����Ҫ�����ض��Ĺ�ҵӦ�á�
		* option_1���Ƿ�����У��λ��Check Digit����0������У��λ��1������У��λ��Ĭ��ֵ��0�����ã���
		* 
		* У��λ���㷽����
		* ���ҵ��󣨴����һλ�ַ���ʼ����Ϊÿ���ַ�����һ��Ȩ�أ�Ȩ�ش� 1 ��ʼ������
		* ��ÿ���ַ���ֵ������Ȩ�أ�Ȼ����͡�
		* ����Ȩ�ͳ��� 10��ȡ������Ȼ���� 10 ��ȥ������������Ϊ 10����У����Ϊ 0��
		* ��У���븽�ӵ��������ݵ�ĩβ��
		*/
		std::string strContent = S_CW2A(m_sstrContent);

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_C25LOGIC;
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			{//�����ı�
				SIZE szContent;
				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			{//�����ı�
				SIZE szContent;
				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
// 			{//�����ı�
// 				SIZE szContent;
// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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

		//����У����
		/*
		* ��������
		*/

		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_UPCA;
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
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
		symbol->input_mode = DATA_MODE; //���ݱ����ʽ
		//symbol->option_1 = 0;   //0 ����У��λ��1 ���� 1 λУ��λ��2 ���� 2 λУ��λ
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
			std::vector<CRect> vecDrawBlack;  //��������������ʵ����
			for (int i = 0; i < vecBlackRect.size(); i++)
			{
				CRect rcTmp(vecBlackRect[i]);
				rcTmp.left = rcTmp.left / nUint * nDrawUint;
				rcTmp.right = rcTmp.right / nUint * nDrawUint;
				rcTmp.bottom = rcTmp.top + rcCode.Height();

				vecDrawBlack.push_back(rcTmp);
			}
			{//�������뱳��ɫ
				COLORREF clrBkgnd = GETCOLOR(L"RGB(255,255,255)");
				CAutoRefPtr<IBrush> brush, oldbrush;
				pRT->CreateSolidColorBrush(clrBkgnd, &brush);
				pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
				pRT->FillRectangle(&rcCode);
				pRT->SelectObject(oldbrush, NULL);
			}

			{//��������
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
			// 			{//�����ı�
			// 				SIZE szContent;
			// 				pRT->MeasureText(m_sstrContent, m_sstrContent.GetLength(), &szContent); //�ı�����ĳ���
			// 				CRect rcText(rcCode);
			// 				rcText.top = rcCode.bottom;
			// 				rcText.bottom = rcText.top + szContent.cy;
			// 				pRT->DrawText(m_sstrContent, -1, (LPRECT)rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
			// 			}
		}
	}
}
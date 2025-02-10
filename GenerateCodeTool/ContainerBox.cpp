#include "stdafx.h"
#include "ContainerBox.h"

ContainerBox::ContainerBox(void)
{
	
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

void ContainerBox::OnPaint(IRenderTarget* pRT)
{
	CRect rcClient = GetClientRect();
	CPoint ptCenter = rcClient.CenterPoint();

	SStringW sstrFontFormat = L"face:宋体,bold:0,italic:0,underline:0,strike:0,size:20";
	SOUI::IFontPtr pFont = SFontPool::GetFont(sstrFontFormat, GetScale());
	pRT->SelectObject(pFont);

	CRect rcCode;
	rcCode.left = ptCenter.x - 100;
	rcCode.right = ptCenter.x + 100;
	rcCode.top = ptCenter.y - 100;
	rcCode.bottom = ptCenter.y + 100;

	if (m_sstrCodeType == L"BARCODE_CODE128")
	{
		struct zint_symbol* symbol;
		symbol = ZBarcode_Create();
		symbol->symbology = BARCODE_CODE128;
		symbol->input_mode = DATA_MODE; //数据编码格式
		//symbol->option_1 = 0;   //0 自动选择子集、1 强制使用子集A、2 强制使用子集B、3 强制使用子集C

		COLORREF clrBkgnd = GETCOLOR(L"RGB(0,255,0)");
		CAutoRefPtr<IBrush> brush, oldbrush;
		pRT->CreateSolidColorBrush(clrBkgnd, &brush);
		pRT->SelectObject(brush, (IRenderObj**)&oldbrush);
		pRT->FillRectangle(&rcCode);
		pRT->SelectObject(oldbrush, NULL);
	}
}
#pragma once
class ContainerBox : public SWindow
{
	DEF_SOBJECT(SWindow, L"containerbox")
public:
	ContainerBox(void);
	~ContainerBox(void);

protected:
	void OnPaint(IRenderTarget* pRT);

	SOUI_MSG_MAP_BEGIN()
		MSG_WM_PAINT_EX(OnPaint)
		SOUI_MSG_MAP_END()

		SOUI_ATTRS_BEGIN()
		//
		SOUI_ATTRS_END()

private:
	SStringW	m_sstrMM2Pix;
	SStringW	m_sstrXDPI;
	SStringW	m_sstrYDPI;
	SStringW	m_sstrCodeType;
	SStringW	m_sstrContent;
	SStringW	m_sstrWidth;
	SStringW	m_sstrHeight;

public:
	void SetMM2Pix(SStringW& sstrMM2Pix);
	void SetXDPI(SStringW& sstrXDPI);
	void SetYDPI(SStringW& sstrYDPI);
	void SetCodeType(SStringW& sstrCodeType);
	void SetContent(SStringW& sstrContnet);
	void SetWidth(SStringW& sstrWidth);
	void SetHeight(SStringW& sstrHeight);
};


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

BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
// 	f(1);
// 	f(1.0);
// 	f(1);

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


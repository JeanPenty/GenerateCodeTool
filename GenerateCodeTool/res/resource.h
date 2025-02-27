//stamp:076deb4b3583cce2
/*<------------------------------------------------------------------------------------------------->*/
/*该文件由uiresbuilder生成，请不要手动修改*/
/*<------------------------------------------------------------------------------------------------->*/
#ifndef _UIRES_H_
#define _UIRES_H_
	struct _UIRES{
		struct _UIDEF{
			const TCHAR * XML_INIT;
			}UIDEF;
		struct _LAYOUT{
			const TCHAR * XML_MAINWND;
			}LAYOUT;
		struct _values{
			const TCHAR * string;
			const TCHAR * color;
			const TCHAR * skin;
			}values;
		struct _PNG{
			const TCHAR * IDB_PNG_SHADOW;
			const TCHAR * IDB_PNG_BTNMIN;
			const TCHAR * IDB_PNG_BTNMAX;
			const TCHAR * IDB_PNG_BTNRESTORE;
			const TCHAR * IDB_PNG_BTNCLOSE;
			const TCHAR * IDB_PNG_BTNMENU;
			const TCHAR * IDB_PNG_BTNSKIN;
			const TCHAR * IDB_PNG_CBXBTN;
			const TCHAR * IDB_PNG_NCBKIMAGE;
			const TCHAR * IDB_PNG_SB;
			const TCHAR * IDB_PNG_BTNCOMMON;
			}PNG;
		struct _ICON{
			const TCHAR * ICON_LOGO;
			}ICON;
		struct _translator{
			const TCHAR * lang_cn;
			const TCHAR * lang_jp;
			}translator;
	};
#endif//_UIRES_H_
#ifdef INIT_R_DATA
struct _UIRES UIRES={
		{
			_T("UIDEF:XML_INIT"),
		},
		{
			_T("LAYOUT:XML_MAINWND"),
		},
		{
			_T("values:string"),
			_T("values:color"),
			_T("values:skin"),
		},
		{
			_T("PNG:IDB_PNG_SHADOW"),
			_T("PNG:IDB_PNG_BTNMIN"),
			_T("PNG:IDB_PNG_BTNMAX"),
			_T("PNG:IDB_PNG_BTNRESTORE"),
			_T("PNG:IDB_PNG_BTNCLOSE"),
			_T("PNG:IDB_PNG_BTNMENU"),
			_T("PNG:IDB_PNG_BTNSKIN"),
			_T("PNG:IDB_PNG_CBXBTN"),
			_T("PNG:IDB_PNG_NCBKIMAGE"),
			_T("PNG:IDB_PNG_SB"),
			_T("PNG:IDB_PNG_BTNCOMMON"),
		},
		{
			_T("ICON:ICON_LOGO"),
		},
		{
			_T("translator:lang_cn"),
			_T("translator:lang_jp"),
		},
	};
#else
extern struct _UIRES UIRES;
#endif//INIT_R_DATA

#ifndef _R_H_
#define _R_H_
struct _R{
	struct _name{
		 const wchar_t * btn_close;
		 const wchar_t * btn_generate;
		 const wchar_t * btn_max;
		 const wchar_t * btn_min;
		 const wchar_t * btn_restore;
		 const wchar_t * cbx_codetype;
		 const wchar_t * containerbox;
		 const wchar_t * edit_content;
		 const wchar_t * edit_height;
		 const wchar_t * edit_mm2pix;
		 const wchar_t * edit_width;
		 const wchar_t * edit_xdpi;
		 const wchar_t * edit_ydpi;
		 const wchar_t * main_caption;
		 const wchar_t * scrollview;
	}name;
	struct _id{
		int btn_close;
		int btn_generate;
		int btn_max;
		int btn_min;
		int btn_restore;
		int cbx_codetype;
		int containerbox;
		int edit_content;
		int edit_height;
		int edit_mm2pix;
		int edit_width;
		int edit_xdpi;
		int edit_ydpi;
		int main_caption;
		int scrollview;
	}id;
	struct _color{
		int blue;
		int gray;
		int green;
		int red;
		int white;
	}color;
	struct _string{
		int title;
		int ver;
	}string;

};
#endif//_R_H_
#ifdef INIT_R_DATA
struct _R R={
	{
		L"btn_close",
		L"btn_generate",
		L"btn_max",
		L"btn_min",
		L"btn_restore",
		L"cbx_codetype",
		L"containerbox",
		L"edit_content",
		L"edit_height",
		L"edit_mm2pix",
		L"edit_width",
		L"edit_xdpi",
		L"edit_ydpi",
		L"main_caption",
		L"scrollview"
	}
	,
	{
		65537,
		65549,
		65538,
		65540,
		65539,
		65545,
		65550,
		65546,
		65548,
		65542,
		65547,
		65543,
		65544,
		65536,
		65541
	}
	,
	{
		0,
		1,
		2,
		3,
		4
	}
	,
	{
		0,
		1
	}
	
};
#else
extern struct _R R;
#endif//INIT_R_DATA

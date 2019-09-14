# climenu
Tiny hierarchical menu suitable for serial or command line interface. Can be used in CLI projects but exactly this implementation is built for AVR, effictively using RAM and PROGMEM strings.

Integrating menu in your project is easy, just fill in the structure like below. The structure defines the tree hierarchy and addresses of variables to be filled in.

const char szpText0[] PROGMEM = "Main menu";
const char szpText1[] PROGMEM = "Set values 11,12";
const char szpText2[] PROGMEM = "Set bool value 2";
const char szpText3[] PROGMEM = "Set int value 3";
const char szpText4[] PROGMEM = "Check settings";
const char szpText5[] PROGMEM = "Set string 1";
const char szpText6[] PROGMEM = "Set bool value 11";
const char szpText7[] PROGMEM = "Set int value 12";
const char szpText8[] PROGMEM = "Exit program";

const char szpVarName0[] PROGMEM = "";
const char szpVarName1[] PROGMEM = "";
const char szpVarName2[] PROGMEM = "Val2";
const char szpVarName3[] PROGMEM = "Val3";
const char szpVarName4[] PROGMEM = "";
const char szpVarName5[] PROGMEM = "Str1";
const char szpVarName6[] PROGMEM = "Val11";
const char szpVarName7[] PROGMEM = "Val12";
const char szpVarName8[] PROGMEM = "";


MENUITEM MenuItem[] = 
{
//idx  Item DataType		Key	     pText		Varname		pData		nBytes					pParent;		pNextSibling;	pFirstChild;
/* 0*/	{1,	TYPE_MENU,		0,   szpText0,	szpVarName0,	NULL,		0,						NULL,			NULL,			&MenuItem[1]	},
/* 1*/	{2,	TYPE_SUBMENU,	'1', szpText1,	szpVarName1,	NULL,		0,						&MenuItem[0],	&MenuItem[2],	&MenuItem[6]	},
/* 2*/	{3,	TYPE_BOOL,		'2', szpText2,	szpVarName2,	&Value2,	0,						&MenuItem[0],	&MenuItem[3],   NULL            },
/* 3*/  {4, TYPE_INT,       '3', szpText3,	szpVarName3,	&Value3,    0,                      &MenuItem[0],   &MenuItem[4],   NULL            },
/* 4*/	{5,	TYPE_EXEC,		'C', szpText4,	szpVarName4,	DisplaySettings,0,					&MenuItem[0],   &MenuItem[5],   NULL            },
/* 5*/	{6,	TYPE_STRING,	'S', szpText5,	szpVarName5,	String1,	sizeof(String1) - 1,    &MenuItem[0],   NULL,           NULL            },
/* 6*/	{7,	TYPE_BOOL,		'1', szpText6,	szpVarName6,	&Value11,	0,                      &MenuItem[1],   &MenuItem[7],   NULL            },
/* 7*/	{8,	TYPE_INT,		'2', szpText7,	szpVarName7,	&Value12,	0,                      &MenuItem[1],   NULL,			NULL            },
/* 8*/	{9,	TYPE_INT,		'X', szpText8,	szpVarName8,	NULL,		0,                      &MenuItem[1],   NULL,			NULL            },
/* 9*/	{0,	TYPE_NONE,		0,		NULL,		NULL,		NULL,		0,						NULL,			NULL,			NULL} // Последний в списке
};

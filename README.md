# climenu
Tiny hierarchical menu suitable for serial or command line interface. Can be used in CLI projects but 
exactly this implementation is built for AVR, effictively using RAM and PROGMEM strings.

Integrating menu in your project is easy, just fill in the structure like below. The structure defines 
the tree hierarchy and addresses of variables to be filled in.

const char szpText0[] PROGMEM = "Main menu";
...
const char szpText8[] PROGMEM = "Exit program";

const char szpVarName0[] PROGMEM = "";
...
const char szpVarName5[] PROGMEM = "Str1";
...
const char szpVarName8[] PROGMEM = "";


MENUITEM MenuItem[] = 
{
//idx  Item DataType		Key	     pText		Varname		pData		nBytes					pParent;		pNextSibling;	pFirstChild;
/* 0*/	{1,	TYPE_MENU,		0,   szpText0,	szpVarName0,	NULL,		0,						NULL,			NULL,			&MenuItem[1]	},
/* 1*/	{2,	TYPE_SUBMENU,	'1', szpText1,	szpVarName1,	NULL,		0,						&MenuItem[0],	&MenuItem[2],	&MenuItem[6]	},
...
/* 9*/	{0,	TYPE_NONE,		0,		NULL,		NULL,		NULL,		0,						NULL,			NULL,			NULL} // Последний в списке
};

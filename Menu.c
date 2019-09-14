#include "f_cpu.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "Serial.h"

#include "Menu.h"
/*
Возвращаемые значения:
 0: ничего не нажато
 1: что-то нажато
 2: работа с меню окончена
 */

unsigned char CurrMenuItem = 0;			// текущий выбранный пункт меню
unsigned char InputType = TYPE_NONE;	// что мы сейчас вводим
void* pInputValue = NULL;				// куда вводим
unsigned char bEditingItem = 0;			// сейчас редактируем?
unsigned char szInputString[MAX_INPUT+1];
unsigned char InputPos;					// Индекс ввода в массив

// test variables
int Value1 = 0;
int Value11 = 0;
int Value12 = 0;
int Value2 = 0;
int Value3 = 0;
char String1[20];

// Декларация функций для вызовов типа TYPE_EXEC. Должны быть void fn(void).
void DisplaySettings(void);

typedef struct MENUITEM
{
	unsigned char		Item;
	unsigned char		DataType;	// INPUT_TYPES или номер пункта меню для перехода при выборе
	unsigned char		Key;		// клавиша для выбора этого пункта меню
	PGM_P				pText;		// текст, который показать
	PGM_P				VarName;	// имя этой переменной или настройки
	void*				pData;		// указатель на место, куда класть
	unsigned char		nBytes;		// Длина строки
	struct MENUITEM*	pParent;	
	struct MENUITEM*	pNextSibling;
	struct MENUITEM*	pFirstChild;
} MENUITEM; 

/*
ROOT--Main menu--Set values 11,12--Set value 11
                |                 +Set value 12
                +Set value 2                               
                +Set value 3                               
                +Set string 1                               
*/

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

MENUITEM* pCurrMenuItem = NULL;

//Строим дерево меню
void InitMenu(void)
{
	//TODO
}

const char PROGMEM szpCrlf[] = "\r\n";

unsigned char IsVariable(INPUT_TYPES Type)
{
	if(Type == TYPE_BOOL ||
			Type == TYPE_INT ||
			Type == TYPE_STRING)
		return TRUE;
	return FALSE;
}

void ShowMenuItems(void)
{
	if(pCurrMenuItem == NULL)
		return;

	// Покажем заголовок
	unsigned char HeadingLen = 0;

	if(pCurrMenuItem->pText != NULL)
	{
		UART0_SendString_P(szpCrlf);
		UART0_SendString_P(pCurrMenuItem->pText);
		HeadingLen = strlen_P(pCurrMenuItem->pText); // подечркнём cтроку
		// подчёркиваем заголовок
		unsigned char i;
		UART0_SendString_P(szpCrlf);
		for(i=0; i<HeadingLen; i++)
			UART0_SendString_P(PSTR("-"));
		UART0_SendString_P(szpCrlf);
	}

	// ПОкажем пункты меню, если они есть
	if(pCurrMenuItem->pFirstChild != NULL)
	{
		MENUITEM* pChild = pCurrMenuItem->pFirstChild;
		// печатаем все тксты меню с одинаковым родителем
		while(pChild != NULL)
		{
			UART0_SendByte(pChild->Key);
			UART0_SendString_P(PSTR("\t"));
			// printf("%c\t", pChild->Key);
			UART0_SendString_P(pChild->pText);
			UART0_SendString_P(szpCrlf);
			pChild = pChild->pNextSibling;
		}
		if(pCurrMenuItem->pParent == NULL)
			UART0_SendString_P(PSTR("ESC\tExit menu\r\n"));
		else
			UART0_SendString_P(PSTR("ESC\tBack\r\n"));
	}
	
}

void ShowCurrentValue(void)
{
	switch(pCurrMenuItem->DataType)
	{
	case TYPE_BOOL:
		if((*(unsigned char*)pCurrMenuItem->pData) == TRUE)
			UART0_SendString_P(PSTR("\"yes\""));
		else UART0_SendString_P(PSTR("\"no\""));
		break;
	case TYPE_INT:
		{
			// printf("%d", *(int*)pCurrMenuItem->pData);
			char ar[12];
			itoa(*(int*)pCurrMenuItem->pData, ar, 10);
			ar[11] = 0;
			UART0_SendString(ar);
		}
		
		break;
	case TYPE_STRING:
		//printf("\"%s\"",(char*)pCurrMenuItem->pData);
		UART0_SendString((char*)pCurrMenuItem->pData);
		break;
	default: 
		UART0_SendString_P(PSTR("ERROR! You have bad DataType value in this menu item"));
	}
}

// обрабатываем пункт меню в зависимости от того, что там надо сделать.
char ProcessItem(unsigned char c)
{
/*enum INPUT_TYPES
{
	TYPE_NONE = 0,
	TYPE_BOOL = 1,		// вводим yes/no
	TYPE_INT = 2,		// вводим int
	TYPE_STRING = 3,
	TYPE_SUBMENU = 4,	// переходим в подменю и показываем все пункты, у которых текущий родитель
	TYPE_EXEC = 5,		// вызываем обработчик, который должен что-то сделать (например, показать список параметров)
	TYPE_EXITPROGRAM = 6, // по нажатию надо выйти из программы
	TYPE_MENU = 7,		// Это меню, чисто показать пункты
};
*/
	// Показываем текущее значение и предлагаем редактировать
	if(bEditingItem == FALSE)
	{
		const char* pHelp = "";
		switch(pCurrMenuItem->DataType)
		{
		case TYPE_BOOL: pHelp = "y/n, 1/0"; break;
		case TYPE_INT:	pHelp = "number"; break;
		case TYPE_STRING:	pHelp = "string"; break;
		case TYPE_EXEC: // Вызываем функцию
			if(pCurrMenuItem->pData != NULL)
			{
				void (*pFn)() = (void (*)())pCurrMenuItem->pData; 
				(*pFn)();
			}
			else UART0_SendString_P(PSTR("ERROR: No ptr to function!\r\n"));
			// Выполнили и сразу вощвращаемся на уровень выше
			pCurrMenuItem = pCurrMenuItem->pParent;
			ShowMenuItems();
			return MR_WORKING; // Отработали и выходим.
		default: UART0_SendString_P(PSTR("ERROR: Data type not processed!\r\n"));
		}

		bEditingItem = TRUE;
		//printf("Current %s is ", pCurrMenuItem->VarName);
		UART0_SendString_P(PSTR("Current "));
		UART0_SendString_P(pCurrMenuItem->VarName);
		UART0_SendString_P(PSTR(" is "));
		ShowCurrentValue();
		//printf(".\r\nPlease enter new value (%s, ESC to cancel): ", pHelp);
		UART0_SendString_P(PSTR(".\r\nPlease enter new value ("));
		UART0_SendString(pHelp);
		UART0_SendString_P(PSTR(", ESC to cancel): "));
		
		// Готовим массив к редактированию
		memset(szInputString, 0, sizeof(szInputString));
		InputPos = 0;
	}
	else // Собственно, редактируем
	{
		if(c == 27) // ESC
		{
			// Закончили редактирование или хотим выйти с уровня
			if(bEditingItem == TRUE)
			{
				//printf("\r\n%s = ", pCurrMenuItem->VarName);
				UART0_SendString_P(szpCrlf);
				UART0_SendString_P(pCurrMenuItem->VarName);
				UART0_SendString_P(PSTR(" = "));
				ShowCurrentValue(); // Покажем, что значение не изменилось
				UART0_SendString_P(szpCrlf);
			}
			bEditingItem = FALSE;
			return MR_EXITSUBMENU;
		}
		else if(c == 13)
		{
			// ENTER, закончили редактирование
			switch(pCurrMenuItem->DataType)
			{
			case TYPE_BOOL:
				for(c=0; c<MAX_INPUT; c++)
					szInputString[c] = tolower(szInputString[c]);
				if(strchr((char*)szInputString, 'y') != NULL)
					*((unsigned char*)pCurrMenuItem->pData) = TRUE;
				if(strchr((char*)szInputString, '1') != NULL)
					*((unsigned char*)pCurrMenuItem->pData) = TRUE;
				if(strchr((char*)szInputString, 'n') != NULL)
					*((unsigned char*)pCurrMenuItem->pData) = FALSE;
				if(strchr((char*)szInputString, '0') != NULL)
					*((unsigned char*)pCurrMenuItem->pData) = FALSE;
				break;
			case TYPE_INT:
				if(szInputString[0] >= '0' && szInputString[0] <= '9')
					*((int*)pCurrMenuItem->pData) = atoi((char*)szInputString);
				break;
			case TYPE_STRING:
				strncpy((char*)pCurrMenuItem->pData, (char*)szInputString, pCurrMenuItem->nBytes);
				break;
			}
			//printf("\r\n%s = ", pCurrMenuItem->VarName);
			UART0_SendString_P(szpCrlf);
			UART0_SendString_P(pCurrMenuItem->VarName);
			UART0_SendString_P(PSTR(" = "));
			ShowCurrentValue();
			UART0_SendString_P(szpCrlf);
			bEditingItem = FALSE;
			return MR_EXITSUBMENU;
		}
		else if(c == 8) // BACKSPACE
		{
			if(InputPos > 0)
			{
				szInputString[InputPos] = 0;
				InputPos--;
			}
		}
		else if(c >= ' ' && c <= 'z')
		{
			if(InputPos < MAX_INPUT)
			{
				szInputString[InputPos++] = c;
				putch(c);
			}
		}
	}

	return MR_WORKING; // FIXME
}

char ProcessMenu(void)
{
	if(!kbhit())
	{
		if(pCurrMenuItem == NULL)
			return MR_NOTHINGPRESSED;
		else return MR_WORKING;
	}

	char c = getch();

	if(!c)
	{
		getch(); //Fn key was pressed
		return 0;
	}

	if(pCurrMenuItem == NULL)
	{
		if(c != 0x0d) 
			return MR_NOTHINGPRESSED;
		else
		{
			pCurrMenuItem = MenuItem;
			ShowMenuItems();
			return MR_WORKING;
		}
	}
	else
	{
//		unsigned char done = 0;
//		while(!done)
		{
//			done = 1;
			if(bEditingItem == FALSE)
			{
				if(c == 27)// переходим на уровень выше
				{
					if(pCurrMenuItem->pParent != NULL)
					{
						pCurrMenuItem = pCurrMenuItem->pParent;
						ShowMenuItems();
						c = 0; // Чтобы этот ESC не попал в редактор 
					}
					else 
						return MR_EXITMENU;
				}
				if(c == 13)// ENTER - покажем пункты меню, если не редактируем, иначе отдаём в редактор
				{
					ShowMenuItems();
					return MR_WORKING;
				}
			}
			
			// мы в меню, обрабатываем его
			MENUITEM* pItem = pCurrMenuItem->pFirstChild;
			// Ксли у пункта меню есть дети, то проверяем, какому из них соответствует
			// нажатая клавиша. 
			// Если у пункта меню нет детей, то делаем то, что там написано
			if(pItem == NULL) // нет детей
			{
				switch(ProcessItem(c))
				{
				case MR_EXITSUBMENU:
					pCurrMenuItem = pCurrMenuItem->pParent;
					ShowMenuItems();
					break;
				}
				return MR_WORKING;
			}
			else
			{
				while(pItem != NULL)
				{
					// перед сравнением приводим к одному и тому же строчному виду
					if( (pItem->Key | 0x20) == (c | 0x20) )
					{
						// выбран этот пункт - нажата кнопка, соответствующая пункту
						// делаем его текущим
						pCurrMenuItem = pItem;
						ShowMenuItems();
						// Ксли ниже этого пункта ничего нет, начинаем обрабатывать пункт
						if(pCurrMenuItem->pFirstChild == NULL)
							if(ProcessItem(c) == MR_EXITSUBMENU)
							{
								pCurrMenuItem = pCurrMenuItem->pParent;
								ShowMenuItems();
								return MR_WORKING;
							}
						return ProcessMenu();
					}
					pItem = pItem->pNextSibling;
				}
				if(pItem == NULL)
				{
					ShowMenuItems();
					return MR_WORKING;
				}
			}
		} // while (!done)
	}

	return MR_WORKING;
}

// Вызываемые функции
void DisplaySettings(void)
{
	// Сохраняем текущий пункт меню
	MENUITEM* pCurr = pCurrMenuItem;
	pCurrMenuItem = MenuItem;
	while(pCurrMenuItem->DataType != TYPE_NONE)
	{
		if(pCurrMenuItem->pData != NULL && pCurrMenuItem->VarName != NULL && 
			IsVariable((INPUT_TYPES)pCurrMenuItem->DataType))
		{
			//printf("%s = ", pCurrMenuItem->VarName);
			UART0_SendString_P(pCurrMenuItem->VarName);
			UART0_SendString_P(PSTR(" = "));
			ShowCurrentValue();
			UART0_SendString_P(szpCrlf);
		}
		pCurrMenuItem++;
	}

	// Возвращаем взад
	pCurrMenuItem = pCurr;
}


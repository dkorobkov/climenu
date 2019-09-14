#ifndef MENU_H_
#define MENU_H_


enum MENU_RETURNS
{
	MR_NOTHINGPRESSED = 0,
	MR_WORKING,
	MR_EXITMENU,
	MR_EXITSUBMENU,
	MR_EXITPROGRAM
};

typedef enum 
{
	TYPE_NONE = 0,
	TYPE_BOOL = 1,		// ������ yes/no
	TYPE_INT = 2,		// ������ int
	TYPE_STRING = 3,
	TYPE_SUBMENU = 4,	// ��������� � ������� � ���������� ��� ������, � ������� ������� ��������
	TYPE_EXEC = 5,		// �������� ����������, ������� ������ ���-�� ������� (��������, �������� ������ ����������)
	TYPE_EXITPROGRAM = 6, // �� ������� ���� ����� �� ���������
	TYPE_MENU = 7,		// ��� ����, ����� �������� ������
}INPUT_TYPES;

#define FALSE 0
#define TRUE 1
#define MAX_INPUT 40


char ProcessMenu(void);



#endif /* MENU_H_ */
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
	TYPE_BOOL = 1,		// вводим yes/no
	TYPE_INT = 2,		// вводим int
	TYPE_STRING = 3,
	TYPE_SUBMENU = 4,	// переходим в подменю и показываем все пункты, у которых текущий родитель
	TYPE_EXEC = 5,		// вызываем обработчик, который должен что-то сделать (например, показать список параметров)
	TYPE_EXITPROGRAM = 6, // по нажатию надо выйти из программы
	TYPE_MENU = 7,		// Это меню, чисто показать пункты
}INPUT_TYPES;

#define FALSE 0
#define TRUE 1
#define MAX_INPUT 40


char ProcessMenu(void);



#endif /* MENU_H_ */
#include "AutoPebble.h"


#define PEBBLE_SCREEN_TYPE_INDIVIDUAL  1	
#define PEBBLE_SCREEN_TYPE_LIST  2	
#define PEBBLE_SCREEN_TYPE_QUICK_SCREEN  3
#define PEBBLE_SCREEN_TYPE_TEXT_SCREEN  4
#define PEBBLE_SCREEN_TYPE_SETTINGS  5



typedef struct AutoPebbleWindow{
	Window * window;
	void * windowData;
	int type;
	int id;
	bool doNotDisturb;
	char* titleFont;
	char* textFont;
	bool noPrefixIfCommand;
	char * tapCommand;
	char * xCommand;
	char * yCommand;
	char * zCommand;
	
	char * actionBack;
	char * multiActionBack;
	int doBackOnBackCommand;
	int doBackOnLongBackCommand;
	
	bool isFullScreen;
	
	char* batteryPrefix;
	
	int16_t light;
} AutoPebbleWindow;
AutoPebbleWindow * newPebbleWindow();
void freePebbleWindow(AutoPebbleWindow * window);


typedef struct AutoPebbleWindows{
	struct AutoPebbleWindows * previous;
	struct AutoPebbleWindow * window;
	struct AutoPebbleWindows * next;
}AutoPebbleWindows;
AutoPebbleWindows * newPebbleWindows();
void initPebbleWindows();
void addAutoPebbleWindow(AutoPebbleWindow* item);
AutoPebbleWindow * addAutoPebbleWindowFromType(Window * window, int type, void*  item);
AutoPebbleWindow * addAutoPebbleListWindow();
AutoPebbleWindow * addAutoPebbleQuickScreenWindow();
AutoPebbleWindow* popAutoPebbleWindow();
void freePebbleWindows();

AutoPebbleWindow* getCurrentAutoPebbleWindow();
void replacePebbleWindowInWindows(AutoPebbleWindows * windows, AutoPebbleWindow* newWindow);
void removeAutoPebbleWindow(Window * window);
void removePebbleWindow(int id);
AutoPebbleWindow * replaceWithAutoPebbleWindowFromType(Window * windowPebble, int type, void*  item, int id,bool bringToTop);
void * getAutoPebbleItem(AutoPebbleWindow* window);
void* getCurrentAutoPebbleItem();
void handle_tap(AccelAxisType axis, int32_t direction);
void handle_battery(BatteryChargeState charge);
int getWindowsCount();
bool sendActionWithPrefixAndFallbackToPhone(AutoPebbleWindow * window, char* prefix, char* text, char* fallback);
/****************************************************************************************************
**************************************************LIST***********************************************
*****************************************************************************************************/
typedef struct AutoPebbleList{
	char  ** labels;
	char **  actions;
	char ** longClickActions;
	uint16_t cellSizes[20]; 
	uint16_t size;
	uint16_t firstCellSize;
	uint16_t defaultCellSize;
	char * headerText;
	char * commandPrefix;
	char * longCommandPrefix;
	bool keepPosition;
	MenuLayer * menuLayer;
}AutoPebbleList;
AutoPebbleList * newPebbleList();
void freePebbleList(AutoPebbleList * list);

void resetLabels();
void resetActions();
void resetLongClickActions();

void setTapCommand(AutoPebbleWindow * window, char * str);

void sendActionToPhone(const char* text);
void sendLongActionToPhone(const char* text);

char * getLabel(int index);
void mainMenu_select_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
void mainMenu_select_long_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
void mainMenu_draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context);
void mainMenu_draw_header(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context);
int16_t mainMenu_get_header_height(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
int16_t mainMenu_get_cell_height(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
uint16_t mainMenu_get_num_rows_in_section(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
uint16_t mainMenu_get_num_sections(struct MenuLayer *menu_layer, void *callback_context);

MenuLayerCallbacks cbacks; // The Pebble API documentation says this should be long-lived
void listUnload(Window* win);
AutoPebbleWindow* initList();
void finishList(AutoPebbleWindow* window);
void handleList(DictionaryIterator *received, void *context, AutoPebbleWindow* window);
AutoPebbleList* getCurrentAutoPebbleList();

AutoPebbleWindow * replaceWithAutoPebbleListWindow(Window * windowPebble,int id,bool bringToTop);
/****************************************************************************************************
*************************************************QUICK SCREEN****************************************
*****************************************************************************************************/
	
typedef struct AutoPebbleQuickScreen{
	char * labelTop;
	char * labelMiddle;
	char * labelBottom;
	char * labelTitle;
	
	char * actionTop;
	char * actionMiddle;
	char * actionBottom;
	
	char * longActionTop;
	char * longActionMiddle;
	char * longActionBottom;
	
	char * multiActionTop;
	char * multiActionMiddle;
	char * multiActionBottom;
	
	
	char * commandPrefix;
	char * longCommandPrefix;
	char * multiCommandPrefix;
	TextLayer* textLayerTop;
	TextLayer* textLayerMiddle;
	TextLayer* textLayerBottom;
	TextLayer* textLayerTitle;
}AutoPebbleQuickScreen;

AutoPebbleQuickScreen * newPebbleQuickScreen();
void freePebbleQuickScreen(AutoPebbleQuickScreen * item);
AutoPebbleQuickScreen* getCurrentAutoPebbleQuickScreen();
AutoPebbleWindow * replaceWithAutoPebbleQuickScreenWindow(Window * windowPebble,int id,bool bringToTop);
void quickScreenUnload(Window* win);
AutoPebbleWindow * initQuickScreen();
void handleQuickScreen(DictionaryIterator *received, void *context, AutoPebbleWindow * window);
void finishQuickScreen(AutoPebbleWindow * window);

/****************************************************************************************************
*************************************************TEXT SCREEN****************************************
*****************************************************************************************************/
typedef struct AutoPebbleTextScreen{
	char * labelTitle;
	char * labelText;
	char * actionMiddle;
	char * longActionMiddle;
	char * multiActionMiddle;
	TextLayer* textLayerTitle;
	TextLayer* textLayerText;
	ScrollLayer* scroll_layer;
}AutoPebbleTextScreen;
AutoPebbleTextScreen * newPebbleTextScreen();
void freePebbleTextScreen(AutoPebbleTextScreen * item);
AutoPebbleWindow * initTextScreen();
void handleTextScreen(DictionaryIterator *received, void *context, AutoPebbleWindow * window);
void finishTextScreen(AutoPebbleWindow * window);
AutoPebbleWindow * replaceWithAutoPebbleTextScreenWindow(Window * windowPebble,int id,bool bringToTop);
extern TextLayer * textLayerTime;
extern char * textFontTime;
char * getTimeTextIfTimeVariable(TextLayer * textLayer, char * text, char * textFont, TickHandler handler);
void setLayerText(TextLayer * textLayer, char * text, char * textFont);
void refreshWindows();
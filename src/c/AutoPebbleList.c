
#include "AutoPebbleWindows.h"
#include "Utils.h"
	
#define PEBBLE_MAX_LIST_SIZE  30
	
AutoPebbleList * newPebbleList(){
	struct AutoPebbleList * item = (AutoPebbleList*) malloc(sizeof(AutoPebbleList));
	item->labels = NULL;
	item->actions = NULL;
	item->longClickActions = NULL;
	for (int i = 0; i < PEBBLE_MAX_LIST_SIZE; i++)
	{
		item->cellSizes[i] = -1;
	}
	item->size = 0;
	item->firstCellSize = 0;
	item->defaultCellSize = -1;
	item->headerText = NULL;
	item->commandPrefix = NULL;
	item->longCommandPrefix = NULL;
	item->keepPosition = false;
	return item;
}
void freePebbleList(AutoPebbleList * list){
	resetArray(list->labels,list->size);
	list->labels = NULL;
	resetArray(list->actions,list->size);
	list->actions = NULL;
	resetArray(list->longClickActions,list->size);
	list->longClickActions = NULL;
	free(list->headerText);
	list->headerText = NULL;
	free(list->commandPrefix);
	list->commandPrefix = NULL;
	free(list->longCommandPrefix);
	list->longCommandPrefix = NULL;
	list->keepPosition = false;
	menu_layer_destroy(list->menuLayer);
	list->menuLayer = NULL;
	free(list);
}
AutoPebbleList* getAutoPebbleList(AutoPebbleWindow* window){	
	return (AutoPebbleList*)getAutoPebbleItem(window);
}
AutoPebbleList* getCurrentAutoPebbleList(){	
	return (AutoPebbleList*)getCurrentAutoPebbleItem();
}
AutoPebbleWindow * replaceWithAutoPebbleListWindow(Window * windowPebble,int id,bool bringToTop){
	return replaceWithAutoPebbleWindowFromType(windowPebble, PEBBLE_SCREEN_TYPE_LIST,newPebbleList(), id,bringToTop);
}

AutoPebbleWindow * addAutoPebbleListWindow(Window * window){
	return addAutoPebbleWindowFromType(window, PEBBLE_SCREEN_TYPE_LIST,newPebbleList());
}
void resetLabels(AutoPebbleList * autoPebbleList){
	resetArray(getCurrentAutoPebbleList()->labels,PEBBLE_MAX_LIST_SIZE);
}
void resetActions(AutoPebbleList * autoPebbleList){
	resetArray(getCurrentAutoPebbleList()->actions,PEBBLE_MAX_LIST_SIZE);
}
void resetLongClickActions(AutoPebbleList * autoPebbleList){
	resetArray(getCurrentAutoPebbleList()->longClickActions,PEBBLE_MAX_LIST_SIZE);
}

void setLabel(AutoPebbleList * autoPebbleList, int index, char * str){
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting label %d: %s", index, str);
	autoPebbleList->labels = setItemInArray(autoPebbleList->labels, PEBBLE_MAX_LIST_SIZE, index, str);
}
void setAction(AutoPebbleList * autoPebbleList, int index, char * str){
	autoPebbleList->actions = setItemInArray(autoPebbleList->actions, PEBBLE_MAX_LIST_SIZE, index, str);
}
void setLongClickAction(AutoPebbleList * autoPebbleList, int index, char * str){
	autoPebbleList->longClickActions = setItemInArray(autoPebbleList->longClickActions, PEBBLE_MAX_LIST_SIZE, index, str);
}


void sendActionToPhone(const char* text){
	sendActionWithPrefixToPhone(getCurrentAutoPebbleList()->commandPrefix, text);	
}
void sendLongActionToPhone(const char* text){
	sendActionWithPrefixToPhone(getCurrentAutoPebbleList()->longCommandPrefix, text);
}

char * getLabel(int index){
	char* text = getCurrentAutoPebbleList()->labels[index];
	if(!text){
		text = "not found";
	}
	
	return text;
}
void mainMenu_select_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	AutoPebbleList * listScreen = getAutoPebbleList(window);
	char* text = NULL;
	if(listScreen->actions){	
		text = listScreen->actions[cell_index->row];		
	}
	sendActionWithPrefixAndFallbackToPhone(window,listScreen->commandPrefix, text,getLabel(cell_index->row));
}
void mainMenu_select_long_click(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{ // Show the detail view when select is pressed.
	
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	AutoPebbleList * listScreen = getAutoPebbleList(window);
	char* text = NULL;
	if(listScreen->longClickActions){
		text = listScreen->longClickActions[cell_index->row];		
	}else{
		if(listScreen->actions){	
			text = listScreen->actions[cell_index->row];		
		}
	}
	sendActionWithPrefixAndFallbackToPhone(window,listScreen->longCommandPrefix, text,getLabel(cell_index->row));
}
static void handle_tick_list(struct tm *tick_time, TimeUnits units_changed) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Minute passed in List");
	if(textFontTime){
		APP_LOG(APP_LOG_LEVEL_INFO, "Setting time now in List");
		menu_layer_reload_data(getCurrentAutoPebbleList()->menuLayer);
	}
}
void mainMenu_draw_row(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context)
{ // Adding the row number as text on the row cell.
	graphics_context_set_text_color(ctx, GColorBlack); // This is important.
	char * font = getCurrentAutoPebbleWindow()->textFont;
	if(!font){
		font = FONT_KEY_GOTHIC_14;
	}
	char * strToUse =  getTimeTextIfTimeVariable(NULL, getLabel(cell_index->row),font,handle_tick_list);
	graphics_draw_text(ctx, strToUse, fonts_get_system_font(font), GRect(0,0,layer_get_frame(cell_layer).size.w,layer_get_frame(cell_layer).size.h), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
	// Just saying layer_get_frame(cell_layer) for the 4th argument doesn't work.  Probably because the GContext is relative to the cell already, but the cell_layer.frame is relative to the menulayer or the screen or something.
}

void mainMenu_draw_header(GContext *ctx, const Layer *cell_layer, uint16_t section_index, void *callback_context)
{ // Adding the header number as text on the header cell.
	char * font = getCurrentAutoPebbleWindow()->titleFont;
	if(!font){
		font = FONT_KEY_GOTHIC_14_BOLD;
	}
	APP_LOG(APP_LOG_LEVEL_INFO, "Drawing header");
	char * strToUse =  getTimeTextIfTimeVariable(NULL, getCurrentAutoPebbleList()->headerText,font,handle_tick_list);
	graphics_context_set_text_color(ctx, GColorBlack); // This is important.
	graphics_draw_text(ctx, strToUse, fonts_get_system_font(font), GRect(0,0,layer_get_frame(cell_layer).size.w,layer_get_frame(cell_layer).size.h), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}
int16_t mainMenu_get_header_height(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{ // Always 30px tall for a header cell
	return 34;
}
int16_t mainMenu_get_cell_height(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{ 
	AutoPebbleList * list = getCurrentAutoPebbleList();
	int16_t size = list->cellSizes[cell_index->row];
	if(size == -1){
		size = list->defaultCellSize;
	}
	if(size == -1){
		/*char * text = list->labels[cell_index->row];
		if(text){
			GRect frame = layer_get_frame(cell_layer);
			GRect text_cell_rect = GRect(0,0,frame.size.w,frame.size.h);
			char * font = getCurrentAutoPebbleWindow()->textFont;
			GSize max_used_size = graphics_text_layout_get_content_size(text, fonts_get_system_font(font), text_cell_rect,GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft);
			size = max_used_size.h;
		}else{*/
		size = 25;
		//}
	}
	return size;
}
uint16_t mainMenu_get_num_rows_in_section(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context)
{
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Nr of rows: %d", getCurrentAutoPebbleList()->size);
	return getCurrentAutoPebbleList()->size;
}
uint16_t mainMenu_get_num_sections(struct MenuLayer *menu_layer, void *callback_context)
{ 
	return 1;
}

MenuIndex lastIndex;
void mainMenu_selection_changed(struct MenuLayer *menu_layer, MenuIndex new_index, MenuIndex old_index, void *callback_context)
{ 
	lastIndex = new_index;
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Index: %d; Section: %d", new_index.row, new_index.section);
	
}

MenuLayerCallbacks cbacks; // The Pebble API documentation says this should be long-lived
void listUnload(Window* win)
{
        APP_LOG(APP_LOG_LEVEL_INFO, "Destroying List Window.");
		removeAutoPebbleWindow(win);
        window_destroy(win);
}

void back_click_handler(ClickRecognizerRef recognizer, void *context) {
	logString("Back");
}
void list_config_provider(AutoPebbleWindow* autoPebbleWindow) {
	logString("configProvider");
	AutoPebbleList * autoPebbleList = getAutoPebbleList(autoPebbleWindow);
	logString("list");
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Type %d",  autoPebbleWindow->type);
	logString("config");
    window_single_click_subscribe(BUTTON_ID_BACK , back_click_handler);	
	menu_layer_set_click_config_onto_window(autoPebbleList->menuLayer, autoPebbleWindow->window);
	
}
AutoPebbleWindow* initList(){
	
	//if(!window){
	GRect frame;
	WindowHandlers wh = { .unload = &listUnload };
	
	Window* window = window_create();
	AutoPebbleWindow* autoPebbleWindow = addAutoPebbleListWindow(window);
	AutoPebbleList * autoPebbleList = getAutoPebbleList(autoPebbleWindow);
	window_set_window_handlers(window, wh);
	autoPebbleList->defaultCellSize = -1;
	
	frame = layer_get_bounds(window_get_root_layer(window));
	
	MenuLayer* mainMenu = menu_layer_create(GRect(0,0,frame.size.w,frame.size.h));
	autoPebbleList->menuLayer = mainMenu;
	
	menu_layer_set_click_config_onto_window(autoPebbleList->menuLayer, autoPebbleWindow->window);
	//window_set_click_config_provider_with_context(window, (ClickConfigProvider) list_config_provider, autoPebbleWindow);
	// Sets the Window's button callbacks to the MenuLayer's default button callbacks.
	
	// Set all of our callbacks.
	cbacks.get_num_sections = &mainMenu_get_num_sections; // Gets called at the beginning of drawing the table.
	cbacks.get_num_rows = &mainMenu_get_num_rows_in_section; // Gets called at the beginning of drawing each section.
	cbacks.get_cell_height = &mainMenu_get_cell_height; // Gets called at the beginning of drawing each normal cell.
	cbacks.get_header_height = &mainMenu_get_header_height; // Gets called at the beginning of drawing each header cell.
	cbacks.select_click = &mainMenu_select_click; // Gets called when select is pressed.
	cbacks.draw_row = &mainMenu_draw_row; // Gets called to set the content of a normal cell.
	cbacks.draw_header = &mainMenu_draw_header; // Gets called to set the content of a header cell.
	cbacks.selection_changed = &mainMenu_selection_changed;
	cbacks.select_long_click = &mainMenu_select_long_click; // I didn't use this.
	menu_layer_set_callbacks(mainMenu, NULL, cbacks); // I have no user data to supply to the callback functions, so 
	layer_add_child(window_get_root_layer(window), menu_layer_get_layer(mainMenu));
	window_stack_push(window, true);
	return autoPebbleWindow;
	//}
}
void finishList(AutoPebbleWindow* mainMenu){
	AutoPebbleList* autoPebbleList = getAutoPebbleList(mainMenu);
	//logString("Finishing list");
	MenuIndex index;
	if(autoPebbleList->keepPosition){
		index = lastIndex;
	}else{
		index.row = 0;
	}
	menu_layer_set_selected_index(autoPebbleList->menuLayer, index, MenuRowAlignTop, true);
	menu_layer_reload_data(autoPebbleList->menuLayer);
}
void handleList(DictionaryIterator *received, void *context, AutoPebbleWindow* window){
	AutoPebbleList * autoPebbleList = getAutoPebbleList(window);
	//logString("setting list...");
	
	//uint16_t cellSizesTemp[20] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};  
	//logToPhone(app_message_inbox_size_maximum());
	 
	Tuple *tuple = tuple = dict_read_first(received);
	while (tuple) {

		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Tuple %d", (int) tuple->key);
		if(tuple->key < PEBBLE_COMMAND_FIRST_ACTION){
			setLabel(autoPebbleList,tuple->key,tuple->value->cstring);
		}else{
			if(tuple->key < PEBBLE_COMMAND_FIRST_LONG_CLICK_ACTION){
				setAction(autoPebbleList,tuple->key - PEBBLE_COMMAND_FIRST_ACTION,tuple->value->cstring);
			}else{
			
				if(tuple->key < PEBBLE_COMMAND_FIRST_CELL_SIZE_ACTION){
					setLongClickAction(autoPebbleList,tuple->key - PEBBLE_COMMAND_FIRST_LONG_CLICK_ACTION,tuple->value->cstring);
				}else{
					if(tuple->key < PEBBLE_COMMAND_SIZE){
						//cellSizesTemp[tuple->key - PEBBLE_COMMAND_FIRST_CELL_SIZE_ACTION] = tuple->value->int16;
						autoPebbleList->cellSizes[tuple->key - PEBBLE_COMMAND_FIRST_CELL_SIZE_ACTION] = tuple->value->int16;
					}else{
						if(tuple->key == PEBBLE_COMMAND_SIZE){
							autoPebbleList->size =  tuple->value->int16;
						}
						if(tuple->key == PEBBLE_COMMAND_HEADER){
							//APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting header %s", tuple->value->cstring);
							autoPebbleList->headerText = setString(autoPebbleList->headerText,tuple->value->cstring);
						}	
						if(tuple->key == PEBBLE_START_LONG_COMMAND){		
							//memcpy(cellSizes, defaultcellSizes, sizeof(defaultcellSizes));
						}
						if(tuple->key == PEBBLE_COMMAND_PREFIX){
							autoPebbleList->commandPrefix = setString(autoPebbleList->commandPrefix,tuple->value->cstring);
						}
						if(tuple->key == PEBBLE_LONG_COMMAND_PREFIX){
							autoPebbleList->longCommandPrefix = setString(autoPebbleList->longCommandPrefix,tuple->value->cstring);
						}
						if(tuple->key == PEBBLE_LAST_LINE_DEFAULT){
							autoPebbleList->defaultCellSize = tuple->value->int16;
						}
						if(tuple->key == PEBBLE_KEEP_LIST_POSITION){
							autoPebbleList->keepPosition = true;
						}	
					}
				}
			}
		}
	  	tuple = dict_read_next(received);
	}
}
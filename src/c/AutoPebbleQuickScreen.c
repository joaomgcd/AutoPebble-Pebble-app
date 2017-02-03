
#include "AutoPebbleWindows.h"
	#include "Utils.h"
	
	#define QUICKSCREEN_TOP_LABEL  0
	#define QUICKSCREEN_TOP_ACTION  1
	#define QUICKSCREEN_TOP_LONG_ACTION  2
	#define QUICKSCREEN_MIDDLE_LABEL  3
	#define QUICKSCREEN_MIDDLE_ACTION  4
	#define QUICKSCREEN_MIDDLE_LONG_ACTION  5
	#define QUICKSCREEN_BOTTOM_LABEL  6
	#define QUICKSCREEN_BOTTOM_ACTION  7
	#define QUICKSCREEN_BOTTOM_LONG_ACTION  8
	#define QUICKSCREEN_TOP_MULTI_ACTION  9
	#define QUICKSCREEN_MIDDLE_MULTI_ACTION  10
	#define QUICKSCREEN_BOTTOM_MULTI_ACTION  11
	#define QUICKSCREEN_ACTION_PREFIX  12
	#define QUICKSCREEN_LONG_ACTION_PREFIX  13
	#define QUICKSCREEN_MULTI_ACTION_PREFIX  14
	
AutoPebbleQuickScreen * newPebbleQuickScreen(){
	struct AutoPebbleQuickScreen * item = (AutoPebbleQuickScreen*) malloc(sizeof(AutoPebbleQuickScreen));
	item->labelTop = NULL;
	item->labelMiddle = NULL;
	item->labelBottom = NULL;
	item->labelTitle = NULL;
	
	item->actionTop = NULL;
	item->actionMiddle = NULL;
	item->actionBottom = NULL;
	
	item->longActionTop = NULL;
	item->longActionMiddle = NULL;
	item->longActionBottom = NULL;
	
	item->multiActionTop = NULL;
	item->multiActionMiddle = NULL;
	item->multiActionBottom = NULL;
	
	item->commandPrefix = NULL;
	item->longCommandPrefix = NULL;
	item->multiCommandPrefix = NULL;
	item->textLayerTop = NULL;
	item->textLayerMiddle = NULL;
	item->textLayerBottom = NULL;
	item->textLayerTitle = NULL;
	return item;
}
void freePebbleQuickScreen(AutoPebbleQuickScreen * item){
	free(item->labelTop);
	item->labelTop = NULL;
	free(item->labelMiddle);
	item->labelMiddle = NULL;
	free(item->labelBottom);
	item->labelBottom = NULL;
	free(item->labelTitle);
	item->labelTitle = NULL;
	
	free(item->actionTop);
	item->actionTop = NULL;
	free(item->actionMiddle);
	item->actionMiddle = NULL;
	free(item->actionBottom);
	item->actionBottom = NULL;
	
	free(item->longActionTop);
	item->longActionTop = NULL;
	free(item->longActionMiddle);
	item->longActionMiddle = NULL;
	free(item->longActionBottom);
	item->longActionBottom = NULL;
	
	free(item->multiActionTop);
	item->multiActionTop = NULL;
	free(item->multiActionMiddle);
	item->multiActionMiddle = NULL;
	free(item->multiActionBottom);
	item->multiActionBottom = NULL;
	
	free(item->commandPrefix);
	item->commandPrefix = NULL;
	
	free(item->longCommandPrefix);
	item->longCommandPrefix = NULL;
	
	free(item->multiCommandPrefix);
	item->multiCommandPrefix = NULL;
	
	
    text_layer_destroy(item->textLayerTop);
	item->textLayerTop = NULL;
    text_layer_destroy(item->textLayerMiddle);
	item->textLayerMiddle = NULL;
    text_layer_destroy(item->textLayerBottom);
	item->textLayerBottom = NULL;
	if(item->textLayerTitle){
		text_layer_destroy(item->textLayerTitle);
	}
	item->textLayerTitle = NULL;
}
	
AutoPebbleQuickScreen* getAutoPebbleQuickScreen(AutoPebbleWindow* window){	
	return (AutoPebbleQuickScreen*)getAutoPebbleItem(window);
}
AutoPebbleQuickScreen* getCurrentAutoPebbleQuickScreen(){
	return (AutoPebbleQuickScreen*)getCurrentAutoPebbleItem();
}
AutoPebbleWindow * replaceWithAutoPebbleQuickScreenWindow(Window * windowPebble, int id,bool bringToTop){
	return replaceWithAutoPebbleWindowFromType(windowPebble, PEBBLE_SCREEN_TYPE_QUICK_SCREEN,newPebbleQuickScreen(),id,bringToTop);
}
AutoPebbleWindow * addAutoPebbleQuickScreenWindow(Window* window){
	return addAutoPebbleWindowFromType(window, PEBBLE_SCREEN_TYPE_QUICK_SCREEN,newPebbleQuickScreen());
}
void quickScreenUnload(Window* win)
{
	APP_LOG(APP_LOG_LEVEL_INFO, "Destroying Quick Screen Window.");
	removeAutoPebbleWindow(win);
	window_destroy(win);
	
}
;
void deselectOption(void *data){
	TextLayer * textLayer = (TextLayer * )data;
	text_layer_set_background_color(textLayer,GColorWhite );
	text_layer_set_text_color(textLayer, GColorBlack);
}

void doSelectOptionAnimation(TextLayer * textLayer){
	if(text_layer_get_text(textLayer)){
		text_layer_set_background_color(textLayer, GColorBlack);
		text_layer_set_text_color(textLayer, GColorWhite);
		app_timer_register(100,deselectOption,textLayer);
	}
}
char * getCommandPrefix(AutoPebbleQuickScreen * quickScreen, char * defaultPrefix){
	if(defaultPrefix){
		return defaultPrefix;
	}else{
		return quickScreen->commandPrefix;
	}
}
void quickscreen_up_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	AutoPebbleQuickScreen * quickScreen = getAutoPebbleQuickScreen(window);
	sendActionWithPrefixAndFallbackToPhone(window, getCommandPrefix(quickScreen, quickScreen->multiCommandPrefix), quickScreen->multiActionTop,quickScreen->labelTop);
	doSelectOptionAnimation(quickScreen->textLayerTop);
}
void quickscreen_middle_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	AutoPebbleQuickScreen * quickScreen = getAutoPebbleQuickScreen(window);
	sendActionWithPrefixAndFallbackToPhone(window,  getCommandPrefix(quickScreen,quickScreen->multiCommandPrefix), quickScreen->multiActionMiddle,quickScreen->labelMiddle);
	doSelectOptionAnimation(quickScreen->textLayerMiddle);
}
void quickscreen_down_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	AutoPebbleQuickScreen * quickScreen = getAutoPebbleQuickScreen(window);
	sendActionWithPrefixAndFallbackToPhone(window,  getCommandPrefix(quickScreen,quickScreen->multiCommandPrefix), quickScreen->multiActionBottom,quickScreen->labelBottom);
	doSelectOptionAnimation(quickScreen->textLayerBottom);
}
void quickscreen_up_long_click_handler(ClickRecognizerRef recognizer, void *context) {
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	AutoPebbleQuickScreen * quickScreen = getAutoPebbleQuickScreen(window);
	sendActionWithPrefixAndFallbackToPhone(window, quickScreen->longCommandPrefix, quickScreen->longActionTop,quickScreen->labelTop);
	doSelectOptionAnimation(quickScreen->textLayerTop);
}
void quickscreen_middle_long_click_handler(ClickRecognizerRef recognizer, void *context) {
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	AutoPebbleQuickScreen * quickScreen = getAutoPebbleQuickScreen(window);
	sendActionWithPrefixAndFallbackToPhone(window,  getCommandPrefix(quickScreen,quickScreen->longCommandPrefix),quickScreen->longActionMiddle,quickScreen->labelMiddle);
	doSelectOptionAnimation(quickScreen->textLayerMiddle);
}
void quickscreen_down_long_click_handler(ClickRecognizerRef recognizer, void *context) {
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	AutoPebbleQuickScreen * quickScreen = getAutoPebbleQuickScreen(window);
	sendActionWithPrefixAndFallbackToPhone(window,  getCommandPrefix(quickScreen,quickScreen->longCommandPrefix),quickScreen->longActionBottom,quickScreen->labelBottom);
	doSelectOptionAnimation(quickScreen->textLayerBottom);
}
void quickscreen_up_click_handler(ClickRecognizerRef recognizer, void *context) {
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	AutoPebbleQuickScreen * quickScreen = getAutoPebbleQuickScreen(window);

	sendActionWithPrefixAndFallbackToPhone(window, quickScreen->commandPrefix, quickScreen->actionTop,quickScreen->labelTop);
	doSelectOptionAnimation(quickScreen->textLayerTop);
}
void quickscreen_middle_click_handler(ClickRecognizerRef recognizer, void *context) {
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	AutoPebbleQuickScreen * quickScreen = getAutoPebbleQuickScreen(window);
	sendActionWithPrefixAndFallbackToPhone(window, quickScreen->commandPrefix, quickScreen->actionMiddle,quickScreen->labelMiddle);
	doSelectOptionAnimation(quickScreen->textLayerMiddle);
}
void quickscreen_down_click_handler(ClickRecognizerRef recognizer, void *context) {
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	AutoPebbleQuickScreen * quickScreen = getAutoPebbleQuickScreen(window);
	sendActionWithPrefixAndFallbackToPhone(window, quickScreen->commandPrefix, quickScreen->actionBottom,quickScreen->labelBottom);
	
	doSelectOptionAnimation(quickScreen->textLayerBottom);
}
void quickscreen_back_click_handler(ClickRecognizerRef recognizer, void *context) {
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	AutoPebbleQuickScreen * quickScreen = getAutoPebbleQuickScreen(window);
	char * prefix = quickScreen->commandPrefix;
	char * action =  window->actionBack;
	if(window->noPrefixIfCommand){
		logString("no prefix");
		if(contains(action, "=:=")){
			prefix = NULL;
		}
	}
	bool sent = sendActionWithPrefixToPhone(prefix,action);
	if(!sent){	
		window_stack_pop(true);
	}else{
		setShouldPopWindowAfterCommand(window->doBackOnBackCommand);
	}
}
void quickscreen_back_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	AutoPebbleQuickScreen * quickScreen = getAutoPebbleQuickScreen(window);
	char * prefix = quickScreen->multiCommandPrefix;
	char * action =  window->multiActionBack;
	if(window->noPrefixIfCommand){
		logString("no prefix");
		if(contains(action, "=:=")){
			prefix = NULL;
		}
	}
	bool sent = sendActionWithPrefixToPhone(prefix,action);
	if(!sent){			
		window_stack_pop(true);
	}else{
		setShouldPopWindowAfterCommand(window->doBackOnLongBackCommand);
	}
}
void config_quickscreen_click_provider(Window *window) {
    window_single_click_subscribe(BUTTON_ID_UP , quickscreen_up_click_handler);	
    window_single_click_subscribe(BUTTON_ID_SELECT , quickscreen_middle_click_handler);	
    window_single_click_subscribe(BUTTON_ID_DOWN, quickscreen_down_click_handler);	
    window_single_click_subscribe(BUTTON_ID_BACK , quickscreen_back_click_handler);	
	
    window_long_click_subscribe(BUTTON_ID_UP ,0, quickscreen_up_long_click_handler, NULL);	
    window_long_click_subscribe(BUTTON_ID_SELECT ,0, quickscreen_middle_long_click_handler, NULL);	
    window_long_click_subscribe(BUTTON_ID_DOWN,0, quickscreen_down_long_click_handler, NULL);	
	
    window_multi_click_subscribe(BUTTON_ID_UP,2, 0, 0, true, quickscreen_up_multi_click_handler);	
    window_multi_click_subscribe(BUTTON_ID_SELECT,2, 0, 0, true, quickscreen_middle_multi_click_handler);	
    window_multi_click_subscribe(BUTTON_ID_DOWN,2, 0, 0, true, quickscreen_down_multi_click_handler);	
    window_multi_click_subscribe(BUTTON_ID_BACK,2, 0, 0, true, quickscreen_back_multi_click_handler);	
}
AutoPebbleWindow * initQuickScreen(){
	
	WindowHandlers wh = { .unload = &quickScreenUnload };
	Window * window = window_create();
	window_set_window_handlers(window, wh);
	
	AutoPebbleWindow* autoPebbleWindow = addAutoPebbleQuickScreenWindow(window);	
	AutoPebbleQuickScreen* autoPebbleQuickScreen = getAutoPebbleQuickScreen(autoPebbleWindow);

	TextLayer* textLayerTitle = text_layer_create(GRect(0,0,144,30));
	text_layer_set_text_alignment(textLayerTitle, GTextAlignmentCenter); // Center the text.
	text_layer_set_font(textLayerTitle, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	text_layer_set_text(textLayerTitle, "AutoPebble");
	text_layer_set_text_color(textLayerTitle, GColorWhite );
	text_layer_set_background_color(textLayerTitle, GColorBlack );	
	autoPebbleQuickScreen->textLayerTitle = textLayerTitle;
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(textLayerTitle));
	

	TextLayer* textLayerTop = text_layer_create(GRect(0,35,144,30));
	text_layer_set_text_alignment(textLayerTop, GTextAlignmentCenter); // Center the text.
	text_layer_set_font(textLayerTop, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	autoPebbleQuickScreen->textLayerTop = textLayerTop;
	layer_set_clips(text_layer_get_layer(autoPebbleQuickScreen->textLayerTop), false);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(textLayerTop));


	TextLayer* textLayerMiddle = text_layer_create(GRect(0,76,144,30));
	text_layer_set_text_alignment(textLayerMiddle, GTextAlignmentCenter); // Center the text.
	text_layer_set_font(textLayerMiddle, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text(textLayerMiddle, "Loading Quick Screen...");
	autoPebbleQuickScreen->textLayerMiddle = textLayerMiddle;
	layer_set_clips(text_layer_get_layer(autoPebbleQuickScreen->textLayerMiddle), false);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(textLayerMiddle));


	TextLayer* textLayerBottom = text_layer_create(GRect(0,133,144,30));
	text_layer_set_text_alignment(textLayerBottom, GTextAlignmentCenter); // Center the text.
	text_layer_set_font(textLayerBottom, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	autoPebbleQuickScreen->textLayerBottom = textLayerBottom;
	layer_set_clips(text_layer_get_layer(autoPebbleQuickScreen->textLayerBottom), false);
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(textLayerBottom));
	
	window_set_click_config_provider(window, (ClickConfigProvider) config_quickscreen_click_provider);
	
	window_stack_push(window, true);
	return autoPebbleWindow;
	//}
}
void handleQuickScreen(DictionaryIterator *received, void *context, AutoPebbleWindow* window){
	AutoPebbleQuickScreen* autoPebbleQuickScreen = getAutoPebbleQuickScreen(window);
	Tuple *tuple = tuple = dict_read_first(received);
	while (tuple) {
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Tuple %d", (int) tuple->key);
		if(tuple->key == QUICKSCREEN_TOP_LABEL){
			autoPebbleQuickScreen->labelTop = resetString(autoPebbleQuickScreen->labelTop, tuple->value->cstring);			
		}else if(tuple->key == QUICKSCREEN_TOP_ACTION){
			autoPebbleQuickScreen->actionTop = resetString(autoPebbleQuickScreen->actionTop, tuple->value->cstring);
		}else if(tuple->key == QUICKSCREEN_TOP_LONG_ACTION){
			autoPebbleQuickScreen->longActionTop = resetString(autoPebbleQuickScreen->longActionTop, tuple->value->cstring);
		}else if(tuple->key == QUICKSCREEN_TOP_MULTI_ACTION){
			autoPebbleQuickScreen->multiActionTop = resetString(autoPebbleQuickScreen->multiActionTop, tuple->value->cstring);
		}else if(tuple->key == QUICKSCREEN_MIDDLE_LABEL){
			autoPebbleQuickScreen->labelMiddle = resetString(autoPebbleQuickScreen->labelMiddle, tuple->value->cstring);
		}else if(tuple->key == QUICKSCREEN_MIDDLE_ACTION){
			autoPebbleQuickScreen->actionMiddle = resetString(autoPebbleQuickScreen->actionMiddle, tuple->value->cstring);
		}else if(tuple->key == QUICKSCREEN_MIDDLE_LONG_ACTION){
			autoPebbleQuickScreen->longActionMiddle = resetString(autoPebbleQuickScreen->longActionMiddle, tuple->value->cstring);
		}else if(tuple->key == QUICKSCREEN_MIDDLE_MULTI_ACTION){
			autoPebbleQuickScreen->multiActionMiddle = resetString(autoPebbleQuickScreen->multiActionMiddle, tuple->value->cstring);
		}else if(tuple->key == QUICKSCREEN_BOTTOM_LABEL){
			autoPebbleQuickScreen->labelBottom = resetString(autoPebbleQuickScreen->labelBottom, tuple->value->cstring);
		}else if(tuple->key == QUICKSCREEN_BOTTOM_ACTION){
			autoPebbleQuickScreen->actionBottom = resetString(autoPebbleQuickScreen->actionBottom, tuple->value->cstring);
		}else if(tuple->key == QUICKSCREEN_BOTTOM_LONG_ACTION){
			autoPebbleQuickScreen->longActionBottom = resetString(autoPebbleQuickScreen->longActionBottom, tuple->value->cstring);
		}else if(tuple->key == QUICKSCREEN_BOTTOM_MULTI_ACTION){
			autoPebbleQuickScreen->multiActionBottom = resetString(autoPebbleQuickScreen->multiActionBottom, tuple->value->cstring);
		}else if(tuple->key == QUICKSCREEN_ACTION_PREFIX){
			autoPebbleQuickScreen->commandPrefix = resetString(autoPebbleQuickScreen->commandPrefix, tuple->value->cstring);
		}else if(tuple->key == QUICKSCREEN_LONG_ACTION_PREFIX){
			autoPebbleQuickScreen->longCommandPrefix = resetString(autoPebbleQuickScreen->longCommandPrefix, tuple->value->cstring);
		}else if(tuple->key == QUICKSCREEN_MULTI_ACTION_PREFIX){
			autoPebbleQuickScreen->multiCommandPrefix = resetString(autoPebbleQuickScreen->multiCommandPrefix, tuple->value->cstring);
		}else if(tuple->key == PEBBLE_COMMAND_HEADER){
			autoPebbleQuickScreen->labelTitle = resetString(autoPebbleQuickScreen->labelTitle, tuple->value->cstring);
		}
		tuple = dict_read_next(received);
	}
}

void finishQuickScreen(AutoPebbleWindow * window){
	AutoPebbleQuickScreen* autoPebbleQuickScreen = getAutoPebbleQuickScreen(window);
	//logString("Refreshing quick screen");
	char * textFont = window->textFont;
	if(!textFont){
		textFont = FONT_KEY_GOTHIC_18_BOLD;
	}
	char * titleFont = window->titleFont;
	if(!titleFont){
		titleFont = FONT_KEY_GOTHIC_24_BOLD;
	}
	setLayerText(autoPebbleQuickScreen->textLayerTitle, autoPebbleQuickScreen->labelTitle, titleFont);	
	setLayerText(autoPebbleQuickScreen->textLayerTop, autoPebbleQuickScreen->labelTop, textFont);
	setLayerText(autoPebbleQuickScreen->textLayerMiddle, autoPebbleQuickScreen->labelMiddle, textFont);
	setLayerText(autoPebbleQuickScreen->textLayerBottom, autoPebbleQuickScreen->labelBottom, textFont);
	
	/*text_layer_set_text(autoPebbleQuickScreen->textLayerTitle, autoPebbleQuickScreen->labelTitle);
	text_layer_set_font(autoPebbleQuickScreen->textLayerTitle, fonts_get_system_font(titleFont));
	
	text_layer_set_text(autoPebbleQuickScreen->textLayerTop, autoPebbleQuickScreen->labelTop);
	text_layer_set_font(autoPebbleQuickScreen->textLayerTop, fonts_get_system_font(textFont));
	
	text_layer_set_text(autoPebbleQuickScreen->textLayerMiddle, autoPebbleQuickScreen->labelMiddle);
	text_layer_set_font(autoPebbleQuickScreen->textLayerMiddle, fonts_get_system_font(textFont));
	
	text_layer_set_text(autoPebbleQuickScreen->textLayerBottom, autoPebbleQuickScreen->labelBottom);
	text_layer_set_font(autoPebbleQuickScreen->textLayerBottom, fonts_get_system_font(textFont));*/
	int screenSize = 168;
	int totalSize = screenSize;
	int statusBarHeight = 0;
	if(!window->isFullScreen){
		statusBarHeight = 16;
	}
	int titleBarHeight = 0;
	GSize sizeTop = text_layer_get_content_size(autoPebbleQuickScreen->textLayerTop);
	GSize sizeMiddle = text_layer_get_content_size(autoPebbleQuickScreen->textLayerMiddle);
	GSize sizeBottom = text_layer_get_content_size(autoPebbleQuickScreen->textLayerBottom);
	if(autoPebbleQuickScreen->labelTitle){	
		GSize sizeTitle = text_layer_get_content_size(autoPebbleQuickScreen->textLayerTitle);
		titleBarHeight =sizeTitle.h + 2;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Title Height: %d", titleBarHeight);
		layer_set_frame(text_layer_get_layer(autoPebbleQuickScreen->textLayerTitle), GRect(0,0,144,titleBarHeight));
		totalSize = totalSize - titleBarHeight;
	}else{
		text_layer_destroy(autoPebbleQuickScreen->textLayerTitle);
		autoPebbleQuickScreen->textLayerTitle = NULL;		
	}
	totalSize = totalSize - statusBarHeight;
	int halfFullScreen = totalSize / 2;
	
	int positionTop = 5;	
	layer_set_frame(text_layer_get_layer(autoPebbleQuickScreen->textLayerTop), GRect(0,positionTop + titleBarHeight,144,sizeTop.h));
	
	int positionMiddle = halfFullScreen - (sizeMiddle.h / 2);		
	layer_set_frame(text_layer_get_layer(autoPebbleQuickScreen->textLayerMiddle), GRect(0,positionMiddle + titleBarHeight,144,sizeMiddle.h));
	
	int positionBottom = totalSize - sizeBottom.h - 5;		
	layer_set_frame(text_layer_get_layer(autoPebbleQuickScreen->textLayerBottom), GRect(0,positionBottom + titleBarHeight,144,sizeBottom.h));
	
	Layer *root = window_get_root_layer(window->window);
    layer_mark_dirty(root);
}
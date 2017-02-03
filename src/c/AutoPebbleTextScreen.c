#include "AutoPebbleWindows.h"
#include "Utils.h"

#define TEXTSCREEN_TEXT  0
#define TEXTSCREEN_MIDDLE_ACTION  1
#define TEXTSCREEN_MIDDLE_LONG_ACTION  2
#define TEXTSCREEN_MIDDLE_MULTI_ACTION  3
	
AutoPebbleTextScreen * newPebbleTextScreen(){
	struct AutoPebbleTextScreen * item = (AutoPebbleTextScreen*) malloc(sizeof(AutoPebbleTextScreen));
	item->labelTitle = NULL;
	item->labelText = NULL;
	item->textLayerTitle = NULL;
	item->textLayerText = NULL;
	item->scroll_layer = NULL;	
	item->actionMiddle = NULL;
	item->longActionMiddle = NULL;
	item->multiActionMiddle = NULL;
	return item;
}
void freePebbleTextScreen(AutoPebbleTextScreen * item){
	free(item->labelTitle);
	item->labelTitle = NULL;
	free(item->labelText);
	item->labelText = NULL;
	
    text_layer_destroy(item->textLayerTitle);
	item->textLayerTitle = NULL;
    text_layer_destroy(item->textLayerText);
	item->textLayerText = NULL;
	
	scroll_layer_destroy(item->scroll_layer);
	item->scroll_layer = NULL;
	
	
	free(item->actionMiddle);
	item->actionMiddle = NULL;
	free(item->longActionMiddle);
	item->longActionMiddle = NULL;
	free(item->multiActionMiddle);
	item->multiActionMiddle = NULL;
}
AutoPebbleWindow * replaceWithAutoPebbleTextScreenWindow(Window * windowPebble,int id,bool bringToTop){
	return replaceWithAutoPebbleWindowFromType(windowPebble, PEBBLE_SCREEN_TYPE_TEXT_SCREEN,newPebbleQuickScreen(),id,bringToTop);
}
AutoPebbleTextScreen* getAutoPebbleTextScreen(AutoPebbleWindow* window){	
	return (AutoPebbleTextScreen*)getAutoPebbleItem(window);
}
AutoPebbleTextScreen* getCurrentAutoPebbleTextScreen(){
	return (AutoPebbleTextScreen*)getCurrentAutoPebbleItem();
}
AutoPebbleWindow * addAutoPebbleTextScreenWindow(Window* window){
	return addAutoPebbleWindowFromType(window, PEBBLE_SCREEN_TYPE_TEXT_SCREEN,newPebbleTextScreen());
}
static const int vert_scroll_text_padding = 4;
int title_height = 18;
void textScreenUnload(Window* win)
{
	APP_LOG(APP_LOG_LEVEL_INFO, "Destroying Text Screen Window.");
	removeAutoPebbleWindow(win);
	window_destroy(win);
	
	
}
void textscreen_middle_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
	sendCommandActionToPhone(getCurrentAutoPebbleTextScreen()->multiActionMiddle);
}
void textscreen_middle_long_click_handler(ClickRecognizerRef recognizer, void *context) {
	sendCommandActionToPhone(getCurrentAutoPebbleTextScreen()->longActionMiddle);
}
void textscreen_middle_click_handler(ClickRecognizerRef recognizer, void *context) {
	sendCommandActionToPhone( getCurrentAutoPebbleTextScreen()->actionMiddle);
}
void textscreen_back_multi_click_handler(ClickRecognizerRef recognizer, void *context) {
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	bool sent = sendCommandActionToPhone(window->multiActionBack);
	if(!sent){			
		window_stack_pop(true);
	}else{
		setShouldPopWindowAfterCommand(window->doBackOnLongBackCommand);
	}
}
void textscreen_back_click_handler(ClickRecognizerRef recognizer, void *context) {
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	bool sent = sendCommandActionToPhone(window->actionBack);
	if(!sent){			
		window_stack_pop(true);
	}else{
		setShouldPopWindowAfterCommand(window->doBackOnBackCommand);
	}
}
void config_textscreen_click_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT , textscreen_middle_click_handler);	
    window_long_click_subscribe(BUTTON_ID_SELECT ,0, textscreen_middle_long_click_handler, NULL);	
    window_multi_click_subscribe(BUTTON_ID_SELECT,2, 0, 0, true, textscreen_middle_multi_click_handler);
    window_single_click_subscribe(BUTTON_ID_BACK , textscreen_back_click_handler);	
    window_multi_click_subscribe(BUTTON_ID_BACK,2, 0, 0, true, textscreen_back_multi_click_handler);	
}
AutoPebbleWindow * initTextScreen(){
	
	WindowHandlers wh = { .unload = &textScreenUnload };
	Window * window = window_create();
	window_set_window_handlers(window, wh);
	AutoPebbleWindow * autoPebbleWindow = addAutoPebbleTextScreenWindow(window);
	AutoPebbleTextScreen* autoPebbleTextScreen = getAutoPebbleTextScreen(autoPebbleWindow);
	

	
	
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	GRect max_text_bounds = GRect(0, title_height + vert_scroll_text_padding, bounds.size.w, 2000);
	// Initialize the scroll layer
	autoPebbleTextScreen->scroll_layer = scroll_layer_create(bounds);
	
	// This binds the scroll layer to the window so that up and down map to scrolling
	// You may use scroll_layer_set_callbacks to add or override interactivity
	ScrollLayerCallbacks scrollCallbacks = {.click_config_provider = &config_textscreen_click_provider};
	scroll_layer_set_callbacks(autoPebbleTextScreen->scroll_layer, scrollCallbacks);
	
	scroll_layer_set_click_config_onto_window(autoPebbleTextScreen->scroll_layer, window);
	
	// Initialize the text layer
	autoPebbleTextScreen->textLayerText = text_layer_create(max_text_bounds);
	text_layer_set_text(autoPebbleTextScreen->textLayerText, "Loading Text Screen...");
	
	// Change the font to a nice readable one
	// This is system font; you can inspect pebble_fonts.h for all system fonts
	// or you can take a look at feature_custom_font to add your own font
	text_layer_set_font(autoPebbleTextScreen->textLayerText, fonts_get_system_font(FONT_KEY_GOTHIC_18));
	
	// Trim text layer and scroll content to fit text box
	GSize max_size = text_layer_get_content_size(autoPebbleTextScreen->textLayerText);
	text_layer_set_size(autoPebbleTextScreen->textLayerText, max_size);
	scroll_layer_set_content_size(autoPebbleTextScreen->scroll_layer, GSize(bounds.size.w, max_size.h + vert_scroll_text_padding));
	
	// Add the layers for display
	scroll_layer_add_child(autoPebbleTextScreen->scroll_layer, text_layer_get_layer(autoPebbleTextScreen->textLayerText));
	
	TextLayer* textLayerTitle = text_layer_create(GRect(0,0,144,title_height));
	text_layer_set_text_alignment(textLayerTitle, GTextAlignmentCenter); // Center the text.
	text_layer_set_font(textLayerTitle, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
	text_layer_set_text(textLayerTitle, "AutoPebble");
	autoPebbleTextScreen->textLayerTitle = textLayerTitle;
	scroll_layer_add_child(autoPebbleTextScreen->scroll_layer, text_layer_get_layer(textLayerTitle));
	
	layer_add_child(window_layer, scroll_layer_get_layer(autoPebbleTextScreen->scroll_layer));
	
	window_stack_push(window, true);
	return autoPebbleWindow;
}

void handleTextScreen(DictionaryIterator *received, void *context, AutoPebbleWindow* window){
	AutoPebbleTextScreen* autoPebbleTextScreen = getAutoPebbleTextScreen(window);
	Tuple *tuple = tuple = dict_read_first(received);
	while (tuple) {
		//APP_LOG(APP_LOG_LEVEL_DEBUG, "Tuple %d", (int)tuple->key);
		if(tuple->key == PEBBLE_COMMAND_HEADER){
			autoPebbleTextScreen->labelTitle = setString(autoPebbleTextScreen->labelTitle, tuple->value->cstring);
		}else if(tuple->key == TEXTSCREEN_TEXT){
			autoPebbleTextScreen->labelText = setString(autoPebbleTextScreen->labelText, tuple->value->cstring);
		}else if(tuple->key == TEXTSCREEN_MIDDLE_ACTION){
			autoPebbleTextScreen->actionMiddle = resetString(autoPebbleTextScreen->actionMiddle, tuple->value->cstring);
		}else if(tuple->key == TEXTSCREEN_MIDDLE_LONG_ACTION){
			autoPebbleTextScreen->longActionMiddle = resetString(autoPebbleTextScreen->longActionMiddle, tuple->value->cstring);
		}else if(tuple->key == TEXTSCREEN_MIDDLE_MULTI_ACTION){
			autoPebbleTextScreen->multiActionMiddle = resetString(autoPebbleTextScreen->multiActionMiddle, tuple->value->cstring);
		}
		tuple = dict_read_next(received);
	}
}

void finishTextScreen(AutoPebbleWindow * window){
	//logString("Refreshing text screen");
	AutoPebbleTextScreen* autoPebbleTextScreen = getAutoPebbleTextScreen(window);
	char * textFont = window->textFont;
	if(!textFont){
		textFont = FONT_KEY_GOTHIC_18;
	}
	char * titleFont = window->titleFont;
	if(!titleFont){
		titleFont = FONT_KEY_GOTHIC_18_BOLD;
	}
	Layer *window_layer = window_get_root_layer(window->window);
	GRect bounds = layer_get_frame(window_layer);
	
	setLayerText(autoPebbleTextScreen->textLayerTitle, autoPebbleTextScreen->labelTitle, titleFont);
	setLayerText(autoPebbleTextScreen->textLayerText, autoPebbleTextScreen->labelText, textFont);	
	/*text_layer_set_text(autoPebbleTextScreen->textLayerTitle, autoPebbleTextScreen->labelTitle);
	text_layer_set_font(autoPebbleTextScreen->textLayerTitle, fonts_get_system_font(titleFont));
	text_layer_set_text(autoPebbleTextScreen->textLayerText, autoPebbleTextScreen->labelText);
	text_layer_set_font(autoPebbleTextScreen->textLayerText, fonts_get_system_font(textFont));*/
	
	GSize max_size_title = text_layer_get_content_size(autoPebbleTextScreen->textLayerTitle);
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "Text: '%s'; Font: %s; Width %d; Height %d",text_layer_get_text(autoPebbleTextScreen->textLayerTitle),titleFont, (int)max_size_title.w, max_size_title.h);
	title_height = max_size_title.h + 10;
	text_layer_set_size(autoPebbleTextScreen->textLayerTitle, GSize(144,  title_height));
	layer_set_frame(text_layer_get_layer(autoPebbleTextScreen->textLayerTitle), GRect(0,0,144,title_height));
	
	GSize max_size = text_layer_get_content_size(autoPebbleTextScreen->textLayerText);
	text_layer_set_size(autoPebbleTextScreen->textLayerText, max_size);
	scroll_layer_set_content_size(autoPebbleTextScreen->scroll_layer, GSize(bounds.size.w, max_size.h + vert_scroll_text_padding + title_height));
	
	/*Layer *root = window_get_root_layer(window);
    layer_mark_dirty(root);*/
}
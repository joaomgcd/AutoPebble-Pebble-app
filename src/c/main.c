
#include "AutoPebbleWindows.h"
#include "Utils.h"



void getLastCommandsFromPhone(){
	sendCommandToPhone(PEBBLE_GET_LAST_LIST_COMMAND, "get");
}
void signalAppStarted(){
	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
	/*Tuplet value1 = TupletInteger(PEBBLE_WATCH_APP_VERSION, (uint8_t)1);
 	dict_write_tuplet(iter, &value1);*/
	Tuplet value = TupletInteger(PEBBLE_SIGNAL_APP_START_COMMAND, (uint8_t)app_message_inbox_size_maximum());
 	dict_write_tuplet(iter, &value);
	app_message_outbox_send();
}
// hex+2*0 is the string "0",
// hex+2*1 is the string "1",
// hex+2*3 is the string "3" and so on.  There is an implied \0 at the end of a constant C string.
// I used \000 here to escape a null-termination.  \0 followed by a 1 looks like \01 and confuses my syntax hilighter.  It may also confuse the compiler.




 void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
   // outgoing message failed
 }
enum {
	AKEY_NUMBER,
	AKEY_TEXT,
};


void subscribe_tap_delayed(){
	accel_tap_service_subscribe(handle_tap);
}
void vibrate(char * vibrationPattern){
    accel_tap_service_unsubscribe();
	struct IntsFromCsv * result = getIntsFromCsv(vibrationPattern);
	//APP_LOG(APP_LOG_LEVEL_DEBUG,"Size %d.\n",(int)result->size);
		
	VibePattern pat = {
		.durations = result->nrs,
		.num_segments = result->size,
	};
	vibes_enqueue_custom_pattern(pat);
	free(result->nrs);
	free(result);
	
	app_timer_register(2000, subscribe_tap_delayed, NULL);
}
void out_sent_handler(DictionaryIterator *sent, void *context) {
	
	//vibes_short_pulse();
	
	char buffer[PERSIST_STRING_MAX_LENGTH];
	if(persist_exists(PEBBLE_SET_VIBRATION_ON_CLICK)){
		persist_read_string(PEBBLE_SET_VIBRATION_ON_CLICK, buffer, PERSIST_STRING_MAX_LENGTH);	
		
		//APP_LOG(APP_LOG_LEVEL_DEBUG,"Got persisted vibration on click: %s",buffer);
		vibrate(buffer);
	}
	if(getShouldPopWindowAfterCommand()){		
		window_stack_pop(true);
		setShouldPopWindowAfterCommand(false);
	}
	
 }
int16_t light = 0;
bool newCommand = true;
AutoPebbleWindow  * autoPebbleWindow = NULL;
uint16_t commandType = 0;
bool firstWindow = true;
bool doNotDisturb = false;
void in_received_handler(DictionaryIterator *received, void *context) {
	
	Tuple *individualCommandTuple = dict_find(received, PEBBLE_SCREEN_TYPE);
	if(individualCommandTuple){
		newCommand = true;
		commandType = individualCommandTuple->value->int16;
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Changing command type to %d", individualCommandTuple->value->int16);		 
	}else{		 
		newCommand = false;
	}
	Tuple *commandScreenId = dict_find(received, PEBBLE_SCREEN_ID);
	AutoPebbleWindow * current = getCurrentAutoPebbleWindow();
	
	if(newCommand &&  current){
		if(commandScreenId && current->id == commandScreenId->value->int32){
			//logString("Ids match, setting do not disturb to false");
			doNotDisturb = false;
		}else{
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting do not disturb to current do not disturb: %d", current->doNotDisturb);	
			doNotDisturb = current->doNotDisturb;	
		}
	}
	if(doNotDisturb){
		//logString("DO NOT DISTURB!!!");		
	}else{
		Tuple *commandEndedTuple = dict_find(received, PEBBLE_COMMAND_END);
		Tuple *commandClearHistory = dict_find(received, PEBBLE_COMMAND_CLEAR_HISTORY);
		Tuple *commandLightMode = dict_find(received, PEBBLE_LIGHT_MODE);
		Tuple *commandVibrate = dict_find(received, PEBBLE_VIBRATION_PATTERN);
		
		Tuple *commandTap = dict_find(received, PEBBLE_COMMAND_TAP);
		Tuple *commandX = dict_find(received, PEBBLE_COMMAND_MOTION_X);
		Tuple *commandY = dict_find(received, PEBBLE_COMMAND_MOTION_Y);
		Tuple *commandZ = dict_find(received, PEBBLE_COMMAND_MOTION_Z);
		
		Tuple *commandDoNotDisturb = dict_find(received, PEBBLE_SCREEN_DO_NOT_DISTURB);
		Tuple *commandTitleFont = dict_find(received, PEBBLE_SCREEN_TITLE_FONT);
		Tuple *commandTextFont = dict_find(received, PEBBLE_SCREEN_TEXT_FONT);
		Tuple *commandNoPrefixIfCommand = dict_find(received, PEBBLE_NO_PREFIX_IF_COMMAND);
		
		Tuple *commandBackAction = dict_find(received, BACK_ACTION);
		Tuple *commandMultiBackAction = dict_find(received, MULTI_BACK_ACTION);
		Tuple *commandGoBackOnBackAction = dict_find(received, GO_BACK_ON_BACK_ACTION);
		Tuple *commandGoBackOnMultiBackAction = dict_find(received, GO_BACK_ON_LONG_BACK_ACTION);
		Tuple *commandFullScreen = dict_find(received, PEBBLE_WINDOW_FULLSCREEN);
		Tuple *commandBatteryPrefix = dict_find(received, PEBBLE_COMMAND_BATTERY_PREFIX);
		
		if(firstWindow){
			window_stack_pop_all(false);
			firstWindow = false;
		}
		if(commandScreenId){
			int screenId = commandScreenId->value->int32;
			removePebbleWindow(screenId);
		}
		if(commandClearHistory){		 
			if(commandClearHistory->value->int16 == HISTORY_REPLACE){
				logString("Popping window");
				window_stack_pop(false);
				// popAutoPebbleWindow();
			}else if(commandClearHistory->value->int16 == HISTORY_CLEAR){	
				logString("Clearing history");		 
				window_stack_pop_all(false);
				freePebbleWindows();
			}
		}
		
		bool commandEnded = false;
		if(commandEndedTuple){
			commandEnded = true;
		}
		if(commandVibrate){		 
			vibrate(commandVibrate->value->cstring);
		}
		
		if(commandType == PEBBLE_SCREEN_TYPE_LIST)
		{ 
			if(newCommand){
				autoPebbleWindow = initList();
			}
			
		}else if(commandType == PEBBLE_SCREEN_TYPE_QUICK_SCREEN)
		{ 
			if(newCommand){
				autoPebbleWindow = initQuickScreen();
			}
			
		}else if(commandType == PEBBLE_SCREEN_TYPE_TEXT_SCREEN)
		{ 
			if(newCommand){
				autoPebbleWindow = initTextScreen();
			}
			
		}else if(commandType == PEBBLE_SCREEN_TYPE_SETTINGS)
		{ 
			
			Tuple *commandVibrateOnClick = dict_find(received, PEBBLE_SET_VIBRATION_ON_CLICK);
			if(commandVibrateOnClick){
				if(commandVibrateOnClick->value->cstring[0] != '-'){
					persist_write_string(PEBBLE_SET_VIBRATION_ON_CLICK, commandVibrateOnClick->value->cstring);
					APP_LOG(APP_LOG_LEVEL_DEBUG,"New vibration on click: %s",commandVibrateOnClick->value->cstring);
				}else{
					logString("deleting vibration on click");
					persist_delete(PEBBLE_SET_VIBRATION_ON_CLICK);
				}
			}
		}
		
		if(commandX){			
			autoPebbleWindow->xCommand = setString(autoPebbleWindow->xCommand, commandX->value->cstring);
		}
		if(commandY){
			autoPebbleWindow->yCommand = setString(autoPebbleWindow->yCommand, commandY->value->cstring);
		}
		if(commandZ){
			autoPebbleWindow->zCommand = setString(autoPebbleWindow->zCommand, commandZ->value->cstring);
		}
		if(commandTitleFont){
			autoPebbleWindow->titleFont = resetString(autoPebbleWindow->titleFont,commandTitleFont->value->cstring);
		}
		if(commandTextFont){
			autoPebbleWindow->textFont = resetString(autoPebbleWindow->textFont,commandTextFont->value->cstring);
		}
		if(commandLightMode){
			autoPebbleWindow->light = commandLightMode->value->int16;
		}
		if(commandDoNotDisturb){
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting Do Not Disturb to true");
			autoPebbleWindow->doNotDisturb = true;
		}
		if(commandNoPrefixIfCommand){
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Setting No prefix to true");
			autoPebbleWindow->noPrefixIfCommand = true;
		}
		if(newCommand && commandScreenId){
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Screen with id %d", (int)commandScreenId->value->int32);
			autoPebbleWindow->id = commandScreenId->value->int32;
		}
		if(commandBackAction){
			autoPebbleWindow->actionBack = resetString(autoPebbleWindow->actionBack, commandBackAction->value->cstring);
		}
		if(commandMultiBackAction){
			autoPebbleWindow->multiActionBack = resetString(autoPebbleWindow->multiActionBack, commandMultiBackAction->value->cstring);
		}
		if(commandGoBackOnBackAction){
			autoPebbleWindow->doBackOnBackCommand = 1;
		}
		if(commandGoBackOnMultiBackAction){
			autoPebbleWindow->doBackOnLongBackCommand = 1;
		}
		if(commandFullScreen){
			/*window_set_fullscreen(autoPebbleWindow->window, true);
			autoPebbleWindow->isFullScreen = true;*/
		}
		if(commandBatteryPrefix){
			autoPebbleWindow->batteryPrefix = resetString(autoPebbleWindow->batteryPrefix, commandBatteryPrefix->value->cstring);
		}
		if(commandEnded){
			textLayerTime = NULL;
			textFontTime = NULL;			
			//tick_timer_service_unsubscribe();
		}
		if(commandType == PEBBLE_SCREEN_TYPE_LIST)
		{ 
			
			handleList(received, context, autoPebbleWindow);
			if(commandEnded){
				finishList(autoPebbleWindow);
			}
		}else if(commandType == PEBBLE_SCREEN_TYPE_QUICK_SCREEN)
		{ 

			handleQuickScreen(received, context, autoPebbleWindow);
			if(commandEnded){
				finishQuickScreen(autoPebbleWindow);
			}
		}else if(commandType == PEBBLE_SCREEN_TYPE_TEXT_SCREEN)
		{ 

			handleTextScreen(received, context, autoPebbleWindow);
			if(commandEnded){
				finishTextScreen(autoPebbleWindow);
			}
		}
		if(commandTap){
			setTapCommand(autoPebbleWindow, commandTap->value->cstring);
		}
		if(commandEnded){
			AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
			if(window){
				int16_t light = window->light;
				if(light == PEBBLE_TURN_ON_LIGHT){
					light_enable(true);
				}
				if(light == PEBBLE_TURN_OFF_LIGHT){
					light_enable(false);
				}
				if(light == PEBBLE_TURN_ON_LIGHT_SHORT){			
					light_enable_interaction();
				}
				light = 0;
			}
			doNotDisturb = false;
		}
	}
	
	//app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
	//app_comm_set_sniff_interval(SNIFF_INTERVAL_NORMAL); 
}
 void in_dropped_handler(AppMessageResult reason, void *context) {
   // incoming message dropped
 }
// Currently, however, MenuLayerCallbacks is a struct and the menu_layer_set_callbacks does not take a pointer to this struct as an argument.
// Instead, it takes the struct itself.  In C structs are passed by value and returned by value, so this would work fine if it were not long-lived.  The struct's data would be copied anyway.
void handle_back_click(ClickRecognizerRef recognizer, void *context){
	
	 	vibes_short_pulse();
}
static void handle_accel(AccelData *accel_data, uint32_t num_samples) {
  
	 	vibes_short_pulse();
}
TextLayer* detailW_text;
TextLayer* tutorialText;
void detailUnload(Window* win)
{
        APP_LOG(APP_LOG_LEVEL_INFO, "Destroying initial Window.");
        text_layer_destroy(detailW_text);
        text_layer_destroy(tutorialText);
        window_destroy(win);
}
void init_delayed(){	
	app_message_register_outbox_sent(out_sent_handler);
}
void showTutorial(void * windowVoid){
	Window * window = (Window*)windowVoid;
	if(window && window_stack_contains_window(window) && firstWindow){
		tutorialText = text_layer_create(GRect(0,0,144,168));
		text_layer_set_text_alignment(tutorialText, GTextAlignmentLeft); // Center the text.
		text_layer_set_font(tutorialText, fonts_get_system_font(FONT_KEY_GOTHIC_18));	
		text_layer_set_text(tutorialText, "In Tasker:\n- add an 'AutoPebble App' condition\n- check 'Watch App Opened'\n- in the Task set an AutoPebble screen");
		layer_add_child(window_get_root_layer(window), text_layer_get_layer(tutorialText));
	}
} 
void bluetooth_connection_callback(bool connected) {
	APP_LOG(APP_LOG_LEVEL_INFO, "bluetooth connected=%d", (int) connected);
	if(!connected){
		window_stack_pop_all(false);
	}
}
static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Minute passed");
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	refreshWindows();	
}
void init(void)
{
	
	WindowHandlers wh = { .unload = &detailUnload };
	Window * detailW = window_create();
	window_set_window_handlers(detailW, wh);
	
	detailW_text = text_layer_create(GRect(0,64,144,168));
	text_layer_set_text_alignment(detailW_text, GTextAlignmentCenter); // Center the text.
	text_layer_set_font(detailW_text, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));	
	text_layer_set_text(detailW_text, "Loading...");
	layer_add_child(window_get_root_layer(detailW), text_layer_get_layer(detailW_text));
	
	app_timer_register(3000, showTutorial, detailW);
	
	window_stack_push(detailW, true);
	
	//initList(); 
	app_message_register_inbox_received(in_received_handler);
	app_message_register_inbox_dropped(in_dropped_handler);
	app_message_register_outbox_failed(out_failed_handler);
	const uint32_t inbound_size = 64;
	const uint32_t outbound_size = 64;
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
	
	app_timer_register(2000, init_delayed, NULL);
	accel_tap_service_subscribe(handle_tap);
	battery_state_service_subscribe(&handle_battery);
	bluetooth_connection_service_subscribe(bluetooth_connection_callback);
	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
	//app_comm_set_sniff_interval(SNIFF_INTERVAL_REDUCED);
	//app_comm_set_sniff_interval(SNIFF_INTERVAL_NORMAL);
	signalAppStarted();

}

void deinit(void)
{
    accel_tap_service_unsubscribe();
	//menu_layer_destroy(mainMenu);
	//window_destroy(window);
}

int main(void)
{
	init();
	app_event_loop();
	deinit();
}
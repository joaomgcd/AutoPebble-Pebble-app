
#include "AutoPebbleWindows.h"
#include "Utils.h"


AutoPebbleWindow * newPebbleWindow(){
	struct AutoPebbleWindow * window = (AutoPebbleWindow*) malloc(sizeof(AutoPebbleWindow));
	window->windowData = NULL;
	window->type = 0;
	window->doNotDisturb = false;
	window->id = -1;
	window->titleFont = NULL;
	window->textFont = NULL;
	window->noPrefixIfCommand = false;
	window->tapCommand = NULL;
	window->xCommand = NULL;
	window->yCommand = NULL;
	window->zCommand = NULL;
	
	window->actionBack = NULL;
	window->multiActionBack = NULL;	
	window->doBackOnBackCommand = 0;
	window->doBackOnLongBackCommand = 0;
	window->isFullScreen = false;
	
	window->batteryPrefix = NULL;
	
	window->light = 0;
	return window;
}
void freePebbleWindow(AutoPebbleWindow * window){
	if(window){
		if(window->type==PEBBLE_SCREEN_TYPE_LIST){
			freePebbleList((AutoPebbleList*)window->windowData);
		}else if(window->type==PEBBLE_SCREEN_TYPE_QUICK_SCREEN){
			freePebbleQuickScreen((AutoPebbleQuickScreen*)window->windowData);
		}else if(window->type==PEBBLE_SCREEN_TYPE_TEXT_SCREEN){
			freePebbleTextScreen((AutoPebbleTextScreen*)window->windowData);
		}
		window->windowData = NULL;
		
		window->type = 0;
		window->doNotDisturb = false;
		window->id = 0;
		window->noPrefixIfCommand = false;
		if(window->titleFont){
			free(window->titleFont);
			window->titleFont = NULL;
		}
		if(window->textFont){
			free(window->textFont);
			window->textFont = NULL;
		}
		if(window->tapCommand){
			free(window->tapCommand);
			window->tapCommand = NULL;
		}
		if(window->xCommand){
			free(window->xCommand);
			window->xCommand = NULL;
		}
		if(window->yCommand){
			free(window->yCommand);
			window->yCommand = NULL;
		}
		if(window->zCommand){
			free(window->zCommand);
			window->zCommand = NULL;
		}
		if(window->actionBack){
			free(window->actionBack);
			window->actionBack = NULL;
		}
		if(window->multiActionBack){
			free(window->multiActionBack);
			window->multiActionBack = NULL;
		}
		window->doBackOnBackCommand = 0;
		window->doBackOnLongBackCommand = 0;
		window->isFullScreen = false;
		
		free(window->batteryPrefix);
		window->batteryPrefix = NULL;
		
		window->light = 0;
		free(window);
	}
}
AutoPebbleWindows * newPebbleWindows(){
	struct AutoPebbleWindows * windows = (AutoPebbleWindows*) malloc(sizeof(AutoPebbleWindows));	
	windows->window = NULL;
	windows->next = NULL;
	windows->previous = NULL;
	return windows;
}

AutoPebbleWindows * autoPebbleWindows = NULL;
void initPebbleWindows(){
	if(!autoPebbleWindows){
		autoPebbleWindows = newPebbleWindows();
	}
}
void addAutoPebbleWindow(AutoPebbleWindow* item){
	initPebbleWindows();
	if(!autoPebbleWindows->window){
		autoPebbleWindows->window = item;
	}else{
		struct AutoPebbleWindows * newWindows = newPebbleWindows() ;
		newWindows->window = item;
		autoPebbleWindows->next = newWindows;
		newWindows->previous = autoPebbleWindows;
		autoPebbleWindows = newWindows;
	}
}
AutoPebbleWindow * addAutoPebbleWindowFromType(Window * windowPebble, int type, void*  item){
	struct AutoPebbleWindow * window = newPebbleWindow();
	window->window = windowPebble;
	window->windowData = item;
	window->type = type;	
	addAutoPebbleWindow(window);
	return window;
}


AutoPebbleWindow* popAutoPebbleWindow(){
	initPebbleWindows();
	freePebbleWindow(autoPebbleWindows->window);
	autoPebbleWindows->window = NULL;
	if(autoPebbleWindows->previous){
		AutoPebbleWindows * previous = autoPebbleWindows->previous;
		free(autoPebbleWindows);
		autoPebbleWindows = NULL;
		previous->next = NULL;
		autoPebbleWindows = previous;
		return autoPebbleWindows->window;
	}else{
		return NULL;
	}
}
void freePebbleWindows(){
	while(autoPebbleWindows && autoPebbleWindows->window){
		popAutoPebbleWindow();
	}
}
AutoPebbleWindow* getCurrentAutoPebbleWindow(){
	initPebbleWindows();
	return autoPebbleWindows->window;
}
void* getCurrentAutoPebbleItem(){
	AutoPebbleWindow* currentWindow = getCurrentAutoPebbleWindow();
	if(currentWindow){
		return currentWindow->windowData;			
	}
	return NULL;
}
char * getAutoPebbleWindowCommandPrefix(AutoPebbleWindow * window){
	if(window->type==PEBBLE_SCREEN_TYPE_LIST){
		return ((AutoPebbleList*)window->windowData)->commandPrefix;
	}else if(window->type==PEBBLE_SCREEN_TYPE_QUICK_SCREEN){
		AutoPebbleQuickScreen * quickScreen = ((AutoPebbleQuickScreen*)window->windowData);
		return quickScreen->commandPrefix;
	}	
	return NULL;
}
void refreshWindows(){
	AutoPebbleWindows * windows = autoPebbleWindows;
	while(windows && windows->window){
		AutoPebbleWindow * window = windows->window;
		if(window->type==PEBBLE_SCREEN_TYPE_LIST){
			AutoPebbleList* autoPebbleList = (AutoPebbleList*)getAutoPebbleItem(window);
			menu_layer_reload_data(autoPebbleList->menuLayer);
		}else if(window->type==PEBBLE_SCREEN_TYPE_QUICK_SCREEN){
			finishQuickScreen(window);
		}else if(window->type==PEBBLE_SCREEN_TYPE_TEXT_SCREEN){
			finishTextScreen(window);
		}
		windows = windows->previous;
	}
}
void setTapCommand(AutoPebbleWindow * window, char * str){
	char * prefix = getAutoPebbleWindowCommandPrefix(window);	
	if(prefix){
		window->tapCommand = resetString(window->tapCommand, prefix);
		window->tapCommand = setString(window->tapCommand, "=:=");
	}
	window->tapCommand = setString(window->tapCommand, str);
}
void handle_tap(AccelAxisType axis, int32_t direction){
	
	//APP_LOG(APP_LOG_LEVEL_DEBUG, "{axis:%d, direction:%d}", axis, (int) direction);
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	if(window){
		char * command = window->tapCommand;
		if(axis == ACCEL_AXIS_X && window->xCommand){
			command = window->xCommand;
		}
		if(axis == ACCEL_AXIS_Y && window->yCommand){
			command = window->yCommand;
		}
		if(axis == ACCEL_AXIS_Z && window->zCommand){
			command = window->zCommand;
		}
		sendCommandToPhone(PEBBLE_PHONE_COMMAND, command);
	}
}
void handle_battery(BatteryChargeState charge){
	logString("battery changed");
	AutoPebbleWindow * window = getCurrentAutoPebbleWindow();
	if(window && window->batteryPrefix){
		char batteryCommandWithLevel[100];
		snprintf (batteryCommandWithLevel,sizeof(batteryCommandWithLevel), "%s=:=%d=:=%d=:=%d",window->batteryPrefix, charge.charge_percent, charge.is_charging, charge.is_plugged);
		sendCommandToPhone(PEBBLE_PHONE_COMMAND, batteryCommandWithLevel);
	}
}
int getWindowsCount(){
	int count = 0;
	AutoPebbleWindows * windows = autoPebbleWindows;
	while(windows && windows->window){
		count++;
		windows = windows->previous;
	}
	return count;
}
void bringWindowsToTop(AutoPebbleWindows * windows){
	AutoPebbleWindows * nextTemp = windows->next;
	if(nextTemp){
		AutoPebbleWindows * previousTemp = windows->previous;
		if(previousTemp){
			windows->previous->next = windows->next;
		}		
		windows->next->previous = previousTemp;
		if(windows != autoPebbleWindows){
			windows->previous = autoPebbleWindows;
			windows->next = NULL;
			autoPebbleWindows->next = windows;
			autoPebbleWindows = autoPebbleWindows->next;
		}
	}

}
void replacePebbleWindowInWindows(AutoPebbleWindows * windows, AutoPebbleWindow* newWindow){
	freePebbleWindow( windows->window);
	windows->window=newWindow;
}
void removeAutoPebbleWindow(Window * window){
	if(autoPebbleWindows->window->window == window){
		popAutoPebbleWindow();
	}else{
		initPebbleWindows();
		AutoPebbleWindows * windows = autoPebbleWindows;
		while(windows && windows->window){
			if(window == windows->window->window){
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Found AutoPebble window. Removing of type %d", windows->window->type);
				AutoPebbleWindows * nextTemp = windows->next;
				AutoPebbleWindows * previousTemp = windows->previous;
				if(previousTemp){
					windows->previous->next = windows->next;
				}		
				windows->next->previous = previousTemp;
				freePebbleWindow(windows->window);
				free(windows);
				return;
			}else{
				windows = windows->previous;
			}
		}
		APP_LOG(APP_LOG_LEVEL_ERROR, "Didn't find Pebble window to remove AutoPebble Window!");
	}
}
void removePebbleWindow(int id){
	if(id){
		initPebbleWindows();
		AutoPebbleWindows * windows = autoPebbleWindows;
		while(windows && windows->window){	
			int currentId = windows->window->id;
			if(currentId == id){				
				APP_LOG(APP_LOG_LEVEL_DEBUG, "Found window with Id %d. Removing of type %d",currentId, windows->window->type);
				window_stack_remove(windows->window->window, false);
				return;
			}
			else{
				windows = windows->previous;
			}
		}
	}
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Didn't find window id %d",id);
}
/*AutoPebbleWindow * replaceWithAutoPebbleWindowFromType(Window * windowPebble, int type, void*  item, int id,bool bringToTop){
	if(id){
		struct AutoPebbleWindow * window = newPebbleWindow();
		window->windowData = item;
		window->type = type;	
		window->id = id; 
		window->window = windowPebble;
		replacePebbleWindow(window, bringToTop);
		return window;
	}else{		
		return addAutoPebbleWindowFromType(windowPebble, type,item);
	}
}*/
void * getAutoPebbleItem(AutoPebbleWindow* window){	
	return window->windowData;
}

bool sendActionWithPrefixAndFallbackToPhone(AutoPebbleWindow * window, char* prefix, char* text, char* fallback){
	
	char * action = text;
	if(!action){
		action = fallback;
	}
	if(window->noPrefixIfCommand){
		//logString("no prefix");
		if(contains(action, "=:=")){
			prefix = NULL;
		}
	}
	return sendActionWithPrefixToPhone(prefix, action);
}
TextLayer * textLayerTime = NULL;
char * textFontTime = NULL;
static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	APP_LOG(APP_LOG_LEVEL_INFO, "Minute passed");
	if(textLayerTime && textFontTime){
		APP_LOG(APP_LOG_LEVEL_INFO, "Setting time now");
		setLayerText(textLayerTime, "<time>", textFontTime);
	}
}
char * getTimeTextIfTimeVariable(TextLayer * textLayer, char * text, char * textFont, TickHandler handler){
	bool isTime = isTimeVariable(text);
	char * strToUse = NULL;
	if(isTime){
		uint8_t length = 9;
		strToUse = (char *)malloc(sizeof(char)*length);
		clock_copy_time_string(strToUse, length);
		if(textLayer){
			textLayerTime = textLayer;	
		}
		if(textFont){			
			textFontTime = textFont;	
		}
		if(handler){
			//tick_timer_service_subscribe(MINUTE_UNIT, handler);
		}
	}else{
		strToUse = text;
	}
	return strToUse;
}
void setLayerText(TextLayer * textLayer, char * text, char * textFont){
	bool isTime = isTimeVariable(text);
	char * strToUse = getTimeTextIfTimeVariable(textLayer, text,textFont, handle_tick);
	text_layer_set_text(textLayer, strToUse);
	text_layer_set_font(textLayer, fonts_get_system_font(textFont));
}
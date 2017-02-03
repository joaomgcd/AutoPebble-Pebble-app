
#include "Utils.h"


void logString(char * text){
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", text);
}
void logChar(char text){
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%c", text);
}
void logNr(int nr){
	APP_LOG(APP_LOG_LEVEL_DEBUG, "%d", nr);
}
bool contains(char * str, char * containsThis){
	return str && containsThis && strstr(str, containsThis) != NULL;
}
char * strndup (const char *s, size_t n)
{
  char *result;
  size_t len = strlen (s);

  if (n < len)
    len = n;

  result = (char *) malloc (len + 1);
  if (!result)
    return 0;

  result[len] = '\0';
  return (char *) memcpy (result, s, len);
}

int indexOf(char* source, char toFind){
	bool found = false;
	char current;
	int i = 0;
	while(source[i] != '\0'){
		if(source[i] == toFind){
			return i;
		}
		i++;
	}
	return -1;
}
char * subStringBefore(char * source, char separator){
	int index = indexOf(source, separator);
	char * subStringBefore;
	subStringBefore = strndup(source, index );
	return subStringBefore;
}
char * subStringAfter(char * source, char separator){
	int strLength = strlen(source);
	int index = indexOf(source, separator);
	int afterSize = strLength - (index+1);
	char * subStringAfter;
	subStringAfter = strndup(source + index+1, afterSize  );
	return subStringAfter;
}
uint32_t countString(char * source, char charToFind){
	int strLength = strlen(source);
	uint32_t count = 0;
	for (int i = 0; i < strLength; i++)
	{
		if(source[i] == charToFind){
			count++;
		}
	}
	return count;
}
void resetArray(char ** arr, int sizeOfArray){
	if(arr){
		for (int i = 0; i < sizeOfArray; i++)
		{
			free(arr[i]);
			arr[i] = NULL;
		}
		free(arr);
		arr = NULL;
	}
}

struct IntsFromCsv * getIntsFromCsv(char * str){
	
	uint32_t count = countString(str,',') + 1;

	uint32_t * result = (uint32_t*)malloc(sizeof(uint32_t) * count);	
	
	int i=0;
	char separator = ',';
	while(indexOf(str,separator) != -1){
		char * before = subStringBefore(str, separator);
  		char * pEnd;
		uint32_t nr = (uint32_t) atoi(before);
		str = subStringAfter(str, separator);
		result[i] = nr;
		i++;
	}
	uint32_t nr = (uint32_t) atoi(str);
	result[i] = nr;
	 IntsFromCsv * intsFromCsv = (IntsFromCsv*)malloc(sizeof(IntsFromCsv));
	intsFromCsv->nrs = result;	
	intsFromCsv->size = count;
	return intsFromCsv;
}
bool isTimeVariable(char * str){
	return contains(str,"<time>");
}
char * resetString(char * destination, char * str){
	if(destination){
		free(destination);
	}
	
	int size = strlen(str)+1;
	destination = (char *)malloc(size);	
	strcpy(destination, str);
	return destination;
}
char * setString(char * destination, char * str){
	if(!destination){
		destination = resetString(destination,str);
	}else{
		int sizeOfFirst = strlen(destination);
		int totalSize =  sizeOfFirst + strlen(str) +1 ;
		char * concat = (char *)malloc(totalSize);
		int i = 0;
		while(i < sizeOfFirst){
			concat[i] = destination[i];
			i++;
		}while(i < totalSize){
			concat[i] = str[i-sizeOfFirst];
			i++;
		}
		free(destination);
		destination = concat;
	}
	return destination;
}
char** setItemInArray(char** arr, int sizeOfArray, int index, char * str){
	if(!arr){
		arr = (char**) malloc(sizeOfArray* sizeof(char*));
		for (int i = 0; i < sizeOfArray; i++)
		{
			arr[i] = NULL;
		}
	}
	arr[index] = setString(arr[index],str);
	
	return arr;
}


bool sendCommandToPhone(uint32_t key, const char* text){
	
	if(text && strlen(text) > 0){
		DictionaryIterator *iter;
		app_message_outbox_begin(&iter);
		Tuplet value = TupletCString(key, text);
		dict_write_tuplet(iter, &value);
		app_message_outbox_send();
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Sent Command to phone: %s",text);
		return true;
	}else{
		return false;
	}
}
void sendIntCommandToPhone(uint32_t key, uint32_t command){
	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
	Tuplet value = TupletInteger(key, (uint8_t)command);
 	dict_write_tuplet(iter, &value);
	app_message_outbox_send();
}
bool sendCommandActionToPhone(const char* text){
	return sendCommandToPhone(PEBBLE_PHONE_COMMAND, text);
}
bool sendActionWithPrefixToPhone(const char* prefix, const char* text){
	bool result = false;
	if(text){
		if(prefix){
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending with prefix %s and text %s",prefix, text);
			int totalSize = strlen(prefix)  + strlen(text) + 1 +3;//+3 because of the =:=
			char buf[totalSize];
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "action: %s%s", commandPrefix, text);
			snprintf(buf, totalSize, "%s=:=%s", prefix, text);
			result = sendCommandToPhone(PEBBLE_PHONE_COMMAND, buf);
			
		}else{
			//APP_LOG(APP_LOG_LEVEL_DEBUG, "Sending with no prefix text %s", text);
			result = sendCommandToPhone(PEBBLE_PHONE_COMMAND, text);
			
		}
	}else{
		//logString("No action to send");
	}
	return result;
}
bool shouldPopWindowAfterCommand = false;
bool getShouldPopWindowAfterCommand(){
	return shouldPopWindowAfterCommand;
}
void setShouldPopWindowAfterCommand(bool should){
	shouldPopWindowAfterCommand = should;
}
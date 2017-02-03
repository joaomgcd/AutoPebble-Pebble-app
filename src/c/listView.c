#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <pebble.h>

void bla(uint32_t number){
	DataLoggingSessionRef logging_session = data_logging_create(0x1234, DATA_LOGGING_UINT, 4, false);
	// Fake creating some data and logging it to the session.
	uint32_t data[] = { number };
	data_logging_log(logging_session, &data, 1);
	
	// When we don't need to log anything else, we can close off the session.
	data_logging_finish(logging_session);
}
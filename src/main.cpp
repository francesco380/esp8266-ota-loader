extern "C" {
#include "user_interface.h"
#include "osapi.h"
#include "espconn.h"
#include "upgrade.h"
}

void setup() { system_init_done_cb(user_init); }
void loop() {}

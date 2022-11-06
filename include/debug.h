#pragma once

#ifdef DEBUG_ENABLED
#define DEBUG(...) ( { Serial.print("[DEBUG] "); Serial.printf(__VA_ARGS__); } )
#else
#define DEBUG(...) /* DEBUG IS DISABLED */
#endif
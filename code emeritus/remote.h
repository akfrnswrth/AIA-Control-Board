/*
 * remote.h
 * Ali Kocaturk 1/4/14
 * heavily based on Sony remote protocol
 */

#define REM_VOLUP 18
#define REM_VOLDN 19
#define REM_LEFT 52
#define REM_RIGHT 51
#define REM_ENTER 101
#define REM_BACK 99
#define REM_INFO 58

int8_t rem_getpress();
void reminit();

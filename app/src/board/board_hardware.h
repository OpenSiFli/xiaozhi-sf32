#ifndef BOARD_HARDWARE_H
#define BOARD_HARDWARE_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
//MCP --led
void open_led();
void close_led();
void set_mode_led();
//pinmux
void set_pinmux();
//关机
void PowerDownCustom(void);
//开机检查
void check_poweron_reason(void);
#ifdef __cplusplus
}
#endif

#endif // BOARD_HARDWARE_H
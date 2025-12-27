#ifndef __ENV_H
#define __ENV_H


#include <shell/shell.h>
void Sys_cmd_Init();
void _ls(int argc, void **argv);
void _reset(int argc, void **argv);
void _poweroff(int argc, void **argv);
void _help(int argc, void **argv);
void _clear(int argc, void **argv);
void _test(int argc, void **argv);

void stop(int argc, void **argv);
void pwm_set (int argc, void **argv);
void env_printf(int argc, void **argv);
#endif
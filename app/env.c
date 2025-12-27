#include <shell/shell.h>
#include <env.h>
#include <driver.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
extern Cmd_PointerTypeDef Cmd;

void Sys_cmd_Init(){
    Cmd.ls = _ls;
    Cmd.reset = _reset;
    Cmd.poweroff = _poweroff;
    Cmd.help = _help;
    Cmd.clear = _clear;
    Cmd.test = _test;
}

void _ls(int argc, void **argv){
    // 列出所有环境变量
    extern EnvVar env_vars[];
    for (int i = 0; env_vars[i].name != NULL; i++) {
        printf("%s\n", env_vars[i].name);
    }
}

void _reset(int argc, void **argv){
    // 重置系统
    NVIC_SystemReset();
}

void _poweroff(int argc, void **argv){
    // 关闭系统
    printf("System is powering off...\n");
    while(1);
}

void _help(int argc, void **argv){
    // 列出所有可用命令
    printf("Available commands:\n");
    printf("- hello\n");
    printf("- reset\n");
    printf("- poweroff\n");
    printf("- help\n");
    printf("- exit\n");
    printf("- clear\n");
    printf("- test\n");
    printf("- ls\n");
}

void _clear(int argc, void **argv){
    // 清屏
    printf(CLEAR_SCREEN);
}

void _test(int argc, void **argv){
    // 测试命令
    printf("Test command executed with %d arguments.\n", argc);
}


void stop(int argc, void **argv){
    // 停止系统运行
    printf("System is stopping...\n");
    pwm_set_all(0,0,0,0);

}

void pwm_set (int argc, void **argv){
    if(argc <= 1){
        printf("Usage: pwm_set <pe13> <pd14> <pb7> <pe6>\n");
        return;
    }
    if(!strcmp(argv[0],"all")){
        int32_t arg_value = atoi((char *)argv[1]);
        pwm_set_all(arg_value, arg_value, arg_value, arg_value);
        printf("PWM set all to %d\n", arg_value);
        return;
    }
    
}

void env_printf(int argc, void **argv){
    if(argc < 1){
        printf("Usage: printf <format> [args...]\n");
        return;
    }
    char *format = (char *)argv[0];
    // 构建可变参数列表
    switch(argc - 1){
        case 0:
            printf(format);
            break;
        case 1:
            printf(format, argv[1]);
            break;
        case 2:
            printf(format, argv[1], argv[2]);
            break;
        case 3:
            printf(format, argv[1], argv[2], argv[3]);
            break;
        case 4:
            printf(format, argv[1], argv[2], argv[3], argv[4]);
            break;
        default:
            printf("Too many arguments for printf command.\n");
            break;
    }
}
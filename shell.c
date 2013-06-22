#include <stdio.h>

int getCmdNumber(char * cmd);
int strEqual(char * str1, char * str2);
void getArg(char * fullStr, char * str, int num);

int main(){
    char cmd[32];
    char arg0[16];
    char arg1[16];
    char buf[512];
    int cmdNum = 0;
    int i;

    while(1){
        for(i = 0; i < 32; i++){
            cmd[i] = 0x0;
            if(i < 16){
                arg0[i] = 0x0;
                arg1[i] = 0x0;
            }
        }

        interrupt(0x21, 0, "\n\0", 0, 0);
        interrupt(0x21, 0, "SHELL>  ", 0, 0);
        interrupt(0x21, 1, cmd, 0, 0);
        interrupt(0x21, 0, "", 0, 0);
            interrupt(0x10, 0xE*256+0xD, 0, 0, 0);
        getArg(cmd, arg0, 0);
        cmdNum = getCmdNumber(arg0);
        if(cmdNum != 0){
            getArg(cmd, arg1, 1);
        }
        switch (cmdNum){
            case 0:
                interrupt(0x21, 0, "Command not found!\n\0", 0, 0);
                break;
            case 3:
                interrupt(0x21, 3, arg1, buf, 0);
                interrupt(0x21, 0, buf, 0, 0);
                break;
            case 4:
                interrupt(0x21, 4, arg1, 0x2000, 0);
                break;
        }
    }
}

int getCmdNumber(char * cmd){
    char * cmd1 = "type\0";
    char * cmd2 = "execute\0";
    int res = 0;
    if(strEqual(cmd, cmd1) == 1){
        return 3;
    } else if(strEqual(cmd, cmd2) == 1){
        return 4;
    } else {
        return 0;
    }
    return 0;
}

int strEqual(char * str1, char * str2){
    int i = 0;
    int tmp = 9;
    int result = 1;
    char car = ' ';

    while(str2[i] != '\0' || str1[i] != '\0'){
        if(str1[i] != str2[i]){
            return 0;
        }
        i++;

    }

    return result;
}

void getArg(char * fullStr, char * str, int num){
    int i = 0;
    int j = 0;
    int actualArg = 0;

    while(actualArg != num){
        while(fullStr[i] != ' '){
            if(fullStr[i] == 0x0){
                interrupt(0x21, 0, "Invalid command!\n\0", 0, 0);
                return;
            }
            i++;
        }
        actualArg++;
    }
    
    if(num > 0){
        i++;
    }
    while(fullStr[i] != ' ' && fullStr[i] != 0x0){
        str[j] = fullStr[i];
        i++;
        j++;
    }
    str[j] = 0x0;
}

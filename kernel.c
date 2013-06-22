void printString(char* str);
void printAll(char * str, int limit);
void readString(char * str);
void readSector(char * buffer, int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void readFile(char* filename, char * buffer);
int isFile(char * dir, int index, char * name);
void copyFile(char * dir, int index, char * buffer);
void intToStr(int num, char * str);
void executeProgram(char * name, int segment);
void terminate();

int main(){
    char buffer[13312];
    char * file = "messag\0";
    char res[10];
    makeInterrupt21();
    interrupt(0x21, 4, "shell\0", 0x2000, 0);
    interrupt(0x21, 5, 0, 0, 0);
 //   terminate();
}

void executeProgram(char * name, int segment){ 
    char buffer[13312];
    int counter = 0;
    int i;
    readFile(name, buffer);
    for(i = 0x0000; counter < 5; i++){
        putInMemory(segment, i, buffer[i]);
        if(buffer[i] == 0x00){
            counter++;
        } else {
            counter = 0;
        }

    }
    i++;
    launchProgram(segment);
    printString("tettsetse\n\0");
}

void terminate(){
    printString("From terminate\n\0");
    interrupt(0x21, 4, "shell\0", 0x2000, 0);
}

void printString(char* str){
    int i = 0;
    while(str[i] != 0x0){
        interrupt(0x10, 0xE*256+str[i], 0, 0, 0);
        if(str[i] == '\n'){
            interrupt(0x10, 0xE*256+0xD, 0, 0, 0);
        }
        i++;
    }
}

void printAll(char * str, int limit){
    int i = 0;
    char c = 0x0;
    for(i; i < limit; i++){
        c = str[i];
        interrupt(0x10, 0xE*256+c, 0, 0, 0);
    }
}

void readString(char * str){
    int i = 0;
    char c = 0x0;
    interrupt(0x10, 0xE*256+0x8, 0, 0, 0);
    c = interrupt(0x16, 0, 0, 0, 0);
    while(c != 0xD){
        if(c == 0x8){
            i--;
            interrupt(0x10, 0xE*256+c, 0, 0, 0);
            interrupt(0x10, 0xE*256+' ', 0, 0, 0);
            interrupt(0x10, 0xE*256+c, 0, 0, 0);
        } else {
            str[i] = c;
            interrupt(0x10, 0xE*256+c, 0, 0, 0);
            i++;
        }
        c = interrupt(0x16, 0, 0, 0, 0);
    }

//    str[i] = 0xA;
    i++;
    str[i] = 0x0;
    interrupt(0x10, 0xE*256+0xA, 0, 0, 0);
}

void readSector(char * buffer, int sector){
    int track, rel_sector, head, cx, dx;
    track = div(sector, 36);
    rel_sector = mod(sector, 18) + 1;
    head = mod( div(sector, 18), 2);

    cx = track*256 + rel_sector;
    dx = head*256 + 0;
    interrupt(0x13, 0x2*256+1, buffer, cx, dx);
}

void handleInterrupt21(int ax, int bx, int cx, int dx){
    switch(ax){
        case 0:
            printString((char*) bx);
            break;
        case 1:
            readString((char*) bx);
            break;
        case 2:
            readSector((char*) bx, cx);
            break;
        case 3:
            readFile((char*) bx,(char *) cx);
            break;
        case 4:
            executeProgram((char*) bx, cx);
            break;
        case 5:
            terminate();
            break;
    }
}

void readFile(char* filename, char * buffer){
    char dir[512];
    int i = 0;
    int maxNbFile = 16;
    int result = 0;
    readSector(dir, 2);
    result = isFile(dir, i*32, filename);
    while(result != 1 && i < maxNbFile){
        i++;
        result = isFile(dir, i*32, filename);
    }
    if(result != 1){
        printString("The file doesn't exist!\n\0");
    } else {
        copyFile(dir, i*32, buffer);
    }
}

void copyFile(char * dir, int index, char * buffer){
    int i = 6;
    int j = 0;
    int bufIndex = 0;
    int sectorIndex = 0;
    char sectorBuffer[512];
    while(dir[index+i] != 0x00 && i < 26){
        sectorIndex = dir[index+i];
        readSector(sectorBuffer, sectorIndex);
        for(j = 0; j < 512; j++){
            buffer[bufIndex] = sectorBuffer[j];
            bufIndex++;
        }
        i++;
    }
}

int isFile(char * dir, int index, char * name){
    int i = 0;
    int result = 1;
    for(i = 0; i < 6; i++){
        if(name[i] == '\0'){
            return result;
        }
        if(dir[i+index] != name[i]){
            result = 0;
        }
    }
    return result;
}

void intToStr(int num, char * str){
    int i = 8;
    int car = 0;
    str[9] = '\0';
    while(i != 0){
        if(num != 0){
            car = mod(num, 10);
            num = div(num, 10);
        } else {
            car = 0;
        }
        str[i] = (char) (((int)'0')+car);
        i--;
    }
}


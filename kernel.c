void printString(char* str);
void printAll(char * str, int limit);
void readString(char * str);
void readSector(char * buffer, int sector);
void deleteFile(char * name);
void writeSector(char * buffer, int sector);
void handleInterrupt21(int ax, int bx, int cx, int dx);
void writeFile(char * name, char * buffer, int numberOfSectors);
void readFile(char* filename, char * buffer);
int findFreeDir(char * dir);
int findFile(char * dir, char * name);
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

void writeSector(char * buffer, int sector){
    int track, rel_sector, head, cx, dx;
    track = div(sector, 36);
    rel_sector = mod(sector, 18) + 1;
    head = mod( div(sector, 18), 2);

    cx = track*256 + rel_sector;
    dx = head*256 + 0;
    
    interrupt(0x13, 0x3*256+1, buffer, cx, dx);
}

void deleteFile(char * name){
    char dir[512];
    char map[512];
    char index = 0;
    int i = 6;
    int sector;

    char tmp;

    readSector(dir, 2);
    readSector(map, 1);

    index = findFile(dir, name);
    if(index == 17){
        printString("File not found!\n\0");
        return;
    }
    dir[index*32] = 0x0;

    while(dir[(index*32) + i] != 0x0){
        sector = dir[(index*32)+i];
        map[sector] = 0x0;
        i++;
    }
    writeSector(dir, 2);
    writeSector(map, 1);
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
        case 6:
            writeSector((char*) bx, cx);
            break;
        case 7:
            deleteFile((char*) bx);
            break;
        case 8:
            writeFile((char *) bx, (char*) cx, dx);
            break;
    }
}

void writeFile(char * name, char * buffer, int numberOfSectors){
    char dir[512];
    char map[512];
    char tmpSect[512];
    int dirNum = 0;
    int i = 0;
    int sectNum = 0;
    int k = 0;
    int iBuf = 0;
    readSector(dir, 2);
    readSector(map, 1);

    dirNum = findFreeDir(dir);

    if(dirNum >= 16){
        return;
    }
    // Writing the name in directory
    while(name[i] != 0x0){
        dir[dirNum*32+i] = name[i];
        i++;
    }

    // Writing data and map
    for(i = 0; i <= numberOfSectors; i++){
        // Locating free sector
        while(map[sectNum] != 0x0){
            sectNum++;
        }
        if(i >= 16){
            deleteFile(name);
            return;
        }
        printString(name); 
        // Writing to map and directory
        map[sectNum] = 0xFF;
        writeSector(map, 0);
        dir[dirNum*32+6+i] = sectNum;

        // Writing the sector
        for(k = 0; k < 512; k++){
            tmpSect[k] = buffer[iBuf];
            iBuf++;
        }
        writeSector(tmpSect, sectNum);
    }

    // Filling the rest of the dir entry with 0x00
    while(i<16){
        dir[dirNum*32+6+i] = 0x0;
        i++;
    }

    writeSector(map, 1);
    writeSector(dir, 2);
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

int findFreeDir(char * dir){
    int i = 0;
    while(dir[i*32] != 0x0 && i < 16){
        i++;
    }
    if(dir[i*32] != 0x0){
        return 17;
    }
    return i;
}

int findFile(char * dir, char * name){
    int index = 0;
    int result = isFile(dir, index, name);
    char tmp;


    while(result != 1 && index < 16){
        index++;
        result = isFile(dir, index*32, name);
    }

    if(result == 0){
        return 17;
    } else {
        return index;
    }
}

int isFile(char * dir, int index, char * name){
    int i = 0;
    int result = 1;
    for(i = 0; i < 6; i++){
        if(name[i] == '\0' && dir[i+index] == '\0' && i != 0){
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


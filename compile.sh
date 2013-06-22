#Compiling
echo "----Compiling:"
bcc -ansi -c -o kernel.o kernel.c
bcc -ansi -c -o shell.o shell.c
bcc -ansi -c -o math.o math.c
as86 lib.asm -o lib.o
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel.o kernel_asm.o math.o
ld86 -o shell -d shell.o lib.o math.o

echo "----Creating the img:"
#making the floppy
dd if=/dev/zero of=floppya.img bs=512 count=2880
dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc
dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
# For the message.txt file in the floppy
dd if=message.txt of=floppya.img bs=512 count=1 seek=30 conv=notrunc

#loading files in the floppy
./loadFile message.txt
./loadFile tstpr2
./loadFile shell

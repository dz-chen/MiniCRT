# 1.编译库并不复杂,直接执行以下两行命令即可(这里只生成静态库)
# MiniCRT是32位代码,所以在64位平台上编译时加-m32选项; 下面的ld -m elf_i386同理 ...
# gcc -m32 -c -fno-builtin -nostdlib -fno-stack-protector entry.c malloc.c stdio.c string.c printf.c
# ar -rs minicrt.a malloc.o printf.o stdio.o string.o


# 2.编译测试代码
# gcc -m32 -c -ggdb -fno-builtin -nostdlib -fno-stack-protector test.c
# ld -m elf_i386 -static -e mini_crt_enrty entry.o test.o minicrt.a -a test 




# 下面写成常见的makefile格式 --- by cdz
CC :=gcc
LD :=ld
CFLAGS	:= -fno-builtin -nostdlib -fno-stack-protector
AR :=ar

cfiles := entry.c malloc.c stdio.c string.c printf.c
cobjs := 		  malloc.o printf.o stdio.o string.o
staticLib=minicrt.a


# 注意entrry.o应该不属于minicrt.a,编译时没有打包进去!!!
test:test.c
	$(CC) -m32 -c -ggdb $(CFLAGS) $(cfiles) 
	$(AR) -rs $(staticLib) $(cobjs)
	$(CC) -m32 -c -ggdb $(CFLAGS)  $<
	$(LD) -m elf_i386 -static -e mini_crt_entry entry.o $@.o $(staticLib) -o $@


clean:
	rm *.o
	rm *.a
	rm test 
	rm test.txt
	


#include "minicrt.h"

/********************************************************************************************
 *                                      字符串格式化相关函数
 * 1.核心函数是vfprintf,特别关注!
 * 2.理解获取变长参数的原理:从右向左开始压栈...
 * ******************************************************************************************/




// 将字符c写到文件指针stream所指向的文件的当前写指针的位置
int fputc(int c, FILE* stream){
    if(fwrite(&c,1,1,stream)!=1)
        return EOF;
    else return c;
}

int fputs(const char* str, FILE* stream){
    int len=strlen(str);
    if(fwrite(str,1,len,stream)!=len)
        return EOF;
    else return len;
}


#ifndef WIN32               // linux ...
// 这几个宏写得太妙了...
#define va_list char*                                              // 用于定义指针类型变量
#define va_start(ap,arg)    (ap=(va_list)&arg+sizeof(arg))         // 让ap指针指向第一个不定参数(arg也是一个指针,它位于参数列表,指向参数列表外的一个字符串,所以&arg其实是得到参数列表的地址,而不是format字符串的地址!!!)
#define va_arg(ap,t)        (*(t*)((ap+=sizeof(t))-sizeof(t)))     // 获取当前不定参数的值,且将指针移向下一个不定参数...(ap+sizeof(t)表示ap指针先往后移,之后再-sizeof(t)表示要获取得移动前的数据!!!)
#define va_end(ap)          (ap=(va_list)0)                        // 指针清0


#else
#include<Windows.h>
#endif

// 字符串格式化的核心函数
int vfprintf(FILE* stream, const char* format, va_list arglist){
    int translating=0;
    int ret=0;
    const char* p=0;
    for(p=format; *p!='\0';++p){
        switch (*p)
        {
        case '%':
            if(!translating)            // 第一次碰到 '%' 认为其是格式化标志
                translating=1;
            else{                       // 否则认为是普通 '%'字符
                if(fputc('%',stream)<0)
                    return EOF;
                ++ret;
                translating=0;
            }
            break;
        case 'd':
            if(translating){            // 如果需要格式化 %d
                char buf[16];
                translating=0;
                itoa(va_arg(arglist,int),buf,10);
                if(fputs(buf,stream)<0)
                    return EOF;
                ret+=strlen(buf);
            }
            else if(fputc('d',stream)<0){       // 不需要格式化,就是普通 'd'字符,直接写入...
                return EOF;
            }
            else
                ++ret;
            break;
        
        case 's':
            if(translating){          // 如果需要格式化%s          
                const char* str=va_arg(arglist,const char*);
                translating=0;
                if(fputs(str,stream)<0)
                    return EOF;
                ret+=strlen(str);
            }
            else if(fputc('s',stream)<0){   // 不需要格式化,就是普通 's'字符....
                return EOF;
            }
            else
                ++ret;
            break;
        default:
            if(translating)
                translating=0;
            if(fputc(*p,stream)<0)
                return EOF;
            else
                ++ret;
            break;
        }
    }

    return ret;
}


// 最终还是调用vfprintf
int printf(const char* format,...){
    va_list(arglist);                   // 定义变量: char* arglist;
    va_start(arglist,format);           // 让arglist指向第一个不定参数
    return vfprintf(stdout,format,arglist);
}


// 最终还是调用vfprintf
int fprintf(FILE* stream, const char* format,...){
    va_list(arglist);                       
    va_start(arglist,format);
    return vfprintf(stream,format,arglist);
}


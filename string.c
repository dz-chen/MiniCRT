/***********************************************************************************************
 *                                  字符串相关操作
 * 1.这部分不涉及与内核的交互...
 * 2.itoa(将整数转换为字符串并返回字符串地址),但是在C语言中它并不属于标准库!!!
 * *******************************************************************************************/

// integer to string
char* itoa(int n, char* str, int radix){
    // radix是进制
    char digit[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char* p=str;
    char* head=str;

    // 进制大小应该在[2,36]之间,且存储转换结果的str不能为空!
    if(!p || radix<2 || radix>36)
        return p;
    
    if(radix!=10 && n<0)
        return p;

    if(n==0){
        *p++='0';       // => *p='0'; p++;
        *p=0;           // 即'\0'字符
        return p;
    }

    if(radix==10 && n<0){
        *p++='-';
        n=-n;
    }

    while(n){
        *p++=digit[n%radix];
        n/=radix;
    }
    *p=0;
    // 注意需要头尾交换位置...
    for(--p; head<p;++head,--p){
        char temp=*head;
        *head=*p;
        *p=temp;
    }
    return str;
}


int strcmp(const char* src, const char* dst){
    int ret=0;
    unsigned char* p1=(unsigned char*)src;      // 上面const修饰的是dst数据,指针可以变...
    unsigned char* p2=(unsigned char*)dst;

    // 这里写得真好!!!
    // 如果p1先为空则ret为-1 ; 如果p2先为空则ret为1
    while(!(ret=*p1-*p2) && *p2)
        ++p1,++p2;
    
    if(ret<0)
        ret=-1;
    else if(ret>0)
        ret=1;
    return (ret);
}



char* strcpy(char* dest, const char* src){
    char* ret=dest;
    while(*src){
        *dest++=*src++;
    }
    *dest='\0';
    return ret;
}

unsigned strlen(const char* str){
    int cnt=0;
    if(!str)
        return 0;
    for(;*str!='\0';++str)
        ++cnt;
    return cnt;
}
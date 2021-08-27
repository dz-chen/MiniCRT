#include "minicrt.h"

int main(int argc,char* argv[]){
    int i;
    FILE* fp;
    char** v=malloc(argc*sizeof(char*));
    for(int i=0;i<argc;i++){
        v[i]=malloc(strlen(argv[i])+1);
        strcpy(v[i],argv[i]);
    }

    // 很奇怪的是,后面的循环中使用argc作为循环退出添加会出错(段错误); 使用num临时存储argc的值却不会出问题
    // 原因??
    int num=argc;
    fp=fopen("test.txt","w");
    for(i=0;i<num;++i){
        int len=strlen(v[i]);
        fwrite(&len,1,sizeof(int),fp);
        fwrite(v[i],1,len,fp);
    }

    fp=fopen("test.txt","r");
    for(i=0;i<num;i++){
        int len;
        char* buf;
        fread(&len,1,sizeof(int),fp);
        buf=malloc(len+1);
        fread(buf,1,len,fp);
        buf[len]='\0';
        printf("%d %s\n",len,buf);
        free(buf);
        free(v[i]);
    }
    fclose(fp);
}
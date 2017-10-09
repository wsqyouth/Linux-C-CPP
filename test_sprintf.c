#include <stdio.h>  

int main(int argc, char *argv[])  
{  
    unsigned char ip_string[4]={192,168,0,33};
    unsigned char Aip_str[10]={0};
    
    sprintf(Aip_str, "%d.%d.%d.%d",*ip_string,*(ip_string+1),*(ip_string+2),*(ip_string+3));
    printf("%s\n",Aip_str);
    return 0;  
}  

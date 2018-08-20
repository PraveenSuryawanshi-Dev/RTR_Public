#include<dlfcn.h>
#include<stdio.h>


typedef int (*_add_)(int,int);
_add_ add;


void *handle;

int main()
{

 handle = dlopen("addition.so",RTLD_LAZY);

 add =(_add_)dlsym(handle,"add");

 int result = add(2,4);

 printf("\nResult is %d",result);

return 1;
}

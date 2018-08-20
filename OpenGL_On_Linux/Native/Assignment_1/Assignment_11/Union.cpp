#include<iostream>
#include<stdio.h>
#include<string.h>

typedef union _buffer_ 
{
	char *string;
	char  character;
}BUFFER;

int main(void)
{

BUFFER buffer;
buffer.character ='c';
printf("\ncharacter : %c",buffer.character);

strcpy(buffer.string,"OpenGl");
printf("\nstring : %s",buffer.string);


printf("\n");
return 1;
}

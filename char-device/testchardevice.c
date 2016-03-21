/*		Programa para testar o LKM chardevice.c
*		Baseado nos programas de Derek Molloy
*		http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/
*		
*		A Linux user space program that communicates with the ebbchar.c LKM. It passes a
* 		string to the LKM and reads the response from the LKM. For this example to work the device
* 		must be called /dev/ebbchar.
*
*		Autor: Giovanni Bauermeister
*/


#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>

#define BUFFER_LENGTH 256
static char receive[BUFFER_LENGTH];

int main(){
	int ret, fd;
	char stringToSend[BUFFER_LENGTH];
	printf("Starting device test code example...\n");
	fd = open("/dev/chardevice", O_RDWR);
	if (fd < 0){
		perror("Failed to open device...");
		return errno;
	}
	printf("Type in a short string to send to Char Device kernel module:\n");
	scanf("%[^\n]%*c", stringToSend);
	printf("Writing message to the device [%s].\n", stringToSend);
	ret = write(fd, stringToSend, strlen(stringToSend));
	if (ret < 0){
		perror("Failed to erite the message to the device.");
		return errno;
	}
	
	printf("Press ENTER to read back from the device...\n");
	getchar();
	
	printf("Reading from the device...\n");
	ret = read(fd, receive, BUFFER_LENGTH);
	if (ret < 0){
		perror("Failed to read message from the device.");
		return errno;
	}
	printf("The received message is: [%s]\n", receive);
	printf("End of the program...Thank you!\n");
	return 0;
}













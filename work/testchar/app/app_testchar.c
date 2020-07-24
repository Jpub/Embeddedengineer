#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>

#define BUFFER_LENGTH 16                // サンプルのためデータ長を固定
static char receive[BUFFER_LENGTH];     // カーネルドライバからデータを受信するバッファ

int main(){
  int ret, fd;
  char stringToSend[BUFFER_LENGTH];
  printf("Starting device test code example...\n");
  /* デバイスファイルをオープンします。 */
  fd = open("/dev/testchar", O_RDWR);
  if (fd < 0){
    perror("Failed to open the device...");
    return errno;
  }

  printf("Type in a short string to send to the kernel module:\n");
  scanf("%[^\n]%*c", stringToSend);
  printf("Writing message to the device [%s].\n", stringToSend);

  /* デバイスファイルにデータを書き込みます。 */
  ret = write(fd, stringToSend, strlen(stringToSend));
  if (ret < 0){
    perror("Failed to write the message to the device.");
    return errno;
  }

  printf("Press ENTER to read back from the device...\n");
  getchar();
  printf("Reading from the device...\n");

  /* デバイスファイルからデータを読み込みます。 */
  ret = read(fd, receive, BUFFER_LENGTH);
  if (ret < 0){
    perror("Failed to read the message from the device.");
    return errno;
  }

  /* デバイスファイルをクローズします。 */
  ret = close(fd);
  if (ret < 0){
    perror("Failed to read the message from the device.");
    return errno;
  }

  printf("The received message is: [%s]\n", receive);
  printf("End of the program\n");
  return 0;
}

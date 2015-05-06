#include <stdio.h>

#include <sys/types.h>

#include <sys/stat.h>

#include <fcntl.h>

#include <sys/ioctl.h>

#include <unistd.h>

#define DEVICE_FILE_NAME "/proc/battery_notify_csh" // 디바이스 파일
// argv 값을 받아 디바이스 파일의 IOCTL cmd 값으로 사용

int main(int argc, char *argv[]) {

int device;

char wbuf[128] = "Write buffer data";

char rbuf[128] = "Read buffer data";

int n = atoi(argv[1]);
int myvalue=7;
int myreadvalue=0;
pid_t mypid=-1;
pid_t readpid=-1;
device = open(DEVICE_FILE_NAME, O_RDWR | O_NDELAY);

if (device >= 0) {
	printf("Device file Open\n");
//	ioctl(device, n); // argv 값을 디바이스 파일에 cmd 값으로 전달
	//printf("Change your device driver's test_level value : ");
	//scanf("%d",&myvalue);
	mypid=getpid();
	printf("mypid num is %d\n",mypid);
	write(device, &mypid,sizeof(pid_t)); // wbuf 값을 디바이스 파일에 전달
	printf("Write value is %d\n", mypid);
	read(device, &readpid, sizeof(pid_t));
	printf("read value is %d\n", readpid);
}else

perror("Device file open fail");

return 0;

}

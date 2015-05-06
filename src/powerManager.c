#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define DEVICE_FILE_NAME "/dev/sample"  //device driver name
#define PROC_NOTIFY_NAME "/proc/battery_notify_csh"  //notify_csh
#define PROC_THRESHOLD_NAME "/proc/battery_threshold_csh"  //threshold_csh


static void sig_usr(int signo)
{
	if(signo == SIGUSR1)
	{
		printf("절전모드\n");
	}
	else if(signo == SIGUSR2)
	{
		printf("일반모드\n");
	}
	return;
}

int main(int argc,char* argv[])
{
	sigset_t newmask, oldmask, pendmask, zeromask;
	struct sigaction usr1_act, usr2_act, usr1_oact, usr2_oact;
	pid_t mypid;
	int device1,device2,device3,device4;
	int mythreshold=2020;
	// Set up for sigaction
	usr1_act.sa_handler = sig_usr;
	sigemptyset(&usr1_act.sa_mask);
	usr1_act.sa_flags = 0;
	usr1_act.sa_flags |= SA_RESTART; 		/*SVR4, 4.3+BSD*/
	sigaction(SIGUSR1, &usr1_act, &usr1_oact);

	usr2_act.sa_handler = sig_usr;
	sigemptyset(&usr2_act.sa_mask);
	usr2_act.sa_flags = 0;
	usr2_act.sa_flags |= SA_RESTART; 		/*SVR4, 4.3+BSD*/
	sigaction(SIGUSR2, &usr2_act, &usr2_oact);

	// Set up for sigpending
	sigemptyset(&newmask);
	sigemptyset(&zeromask);
	sigaddset(&newmask, SIGINT);
	sigaddset(&newmask, SIGSTOP);
	sigaddset(&newmask, SIGQUIT);
	mypid=getpid(); // get  mypid number
//	device1=open(DEVICE_FILE_NAME,O_RDWR | O_NDELAY); //DEVICE DRIVER
	device2=open(PROC_NOTIFY_NAME,O_RDWR | O_NDELAY); //GET PID
	if(device2 >=0)
	{
		printf("mythreshold number is %d, pid number is %d\n",mythreshold,mypid);
		write(device2,&mypid,sizeof(pid_t));
	}
	else
	{
		perror("Device2 file open fail");
	}
	device3=open(PROC_THRESHOLD_NAME,O_RDWR |O_NDELAY);//GET THRESHOLD
	if(device3 >= 0)
	{
		if(argc > 1)
		{
			mythreshold=atoi(argv[1]);
			
		}
		printf("mythreshold number is %d, pid number is %d\n",mythreshold,mypid);
		write(device3,&mythreshold,sizeof(int));
	}
	else
	{
		perror("Device3 file open fail");
	}



	/* block SIGINT, SIGSTOP, SIGQUIT and save current signal mask */
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
	{
		printf("SIG_BLOCK error\n");
	}
	printf("blocked\n");
	while(1)
	{
		//busy waiting
	}	
	/* reset signal mask which unblocks SIGQUIT */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
	{
		printf("SIG_SETMASK error");
	}
	printf("unblocked\n");

	return 0;
}

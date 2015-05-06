# virtual-power-Manager
to use this powermanager

first, you just type 'make'
second, you have to make node  -> mknod /dev/sample c 574 0
third,  sudo insmod myproc.ko
fourth, run powerManager with threshold  ex)  $ ./powerManager 30 &  <you have to run in background>
fifth, run simulate.sh 
sixth, run batteryStatus

#include <stdlib.h>

int main(){
	
	system("arduino --upload lightControl/lightControl.ino --port /dev/ttyACM0");
	system("arduino --upload lightControl/lightControl.ino --port /dev/ttyACM1");
	return 0;
}

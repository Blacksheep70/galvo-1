#include <iostream>
#include <errno.h>

#include <wiringPiSPI.h>
#include <unistd.h>

using namespace std;

static const int CHANNEL = 0;

void SendData(long data) {
  unsigned char *p = (unsigned char*)&data;
  
  int result = wiringPiSPIDataRW(CHANNEL, p, 3);
  cout << "Write result: " << result << endl;
}

int main() {
  int fd;

  fd = wiringPiSPISetup(CHANNEL, 500000);
  
  cout << "Init result: " << fd << endl;

  unsigned long config = 0x190000 | 0x1 | 0x4;

  SendData(config);
  SendData(0x80000); // one dac
  SendData(0x100001); // power on first dac

  unsigned long DACAddress = 0x00000;
  unsigned int DACValue = 0xffff;
  unsigned long data = DACAddress | DACValue;
  
  SendData(data);
  SendData(0x1D0000);

  sleep(5);
}

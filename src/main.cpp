#include <mbed.h>

volatile int flag = 0;
DigitalOut cs(PC_1);
SPI spi(PF_9,PF_8,PF_7);

void setFlag() {          
  flag = 1;
}
int main() {
  cs=1;
  spi.format(8,3);
  spi.frequency(100000);
	Ticker t;
	t.attach(&setFlag,0.5);
  while(1) {
    if(flag){
      cs=0;
      spi.write(0xA8);//X_L
      int xl =spi.write(0x00);
      if(xl!=0x41){
        printf("Output on x_l give: 0x%X\n",xl);
      }
      cs =1;
      flag=0;
    }
    
  }
}
#include <mbed.h>

volatile int flag = 0;
SPI spi(PF_9, PF_8, PF_7); // mosi, miso, sclk
DigitalOut cs(PC_1);

void setFlag() {          
  flag = 1;
}
void setMode() {          
  cs=0;
  spi.write(0x20);
  spi.write(0xCF);
  cs=1;
}
int main() {
  cs=1;
  setMode();
  spi.format(8,3);
  spi.frequency(100000);
  
  
	Ticker t;
	t.attach(&setFlag,0.05);
  while(1) {
    if(flag){
      cs=0;
      spi.write(0xE8);//X_L
      uint8_t xl =spi.write(0x00);
      cs=1;
      cs=0;
      spi.write(0xE9);
      int8_t xh =spi.write(0x00);
      cs=1;
      int16_t data= xh*256+xl-100;
      printf("Output on x give: %d\n",data);
      flag=0;
    }
    
  }
}
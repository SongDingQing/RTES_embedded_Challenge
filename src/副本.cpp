#include <mbed.h>

volatile int flag = 0;
volatile int steps = 0;
volatile int state = 0;//indicate the moving have started
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
int16_t readData(int code){
      cs=0;
      spi.write(code);
      uint8_t tempL = spi.write(0x00);
      cs=1;
      cs=0;
      spi.write(code+1);
      uint8_t tempH = spi.write(0x00);
      cs=1;
      int16_t temp= tempH*256+tempL;
      return temp;
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
      int16_t dataX=readData(0xA8);
      //printf("Output on x give: %d\n",dataX);
      if(dataX<-12000&&state==0){
        state=1;
      }
      if(dataX>12000&&state==1){
        state=0;
        steps++;
        //printf("The steps moved: %d\n",steps);
        int distance =steps*7;
        printf("the distance moved:%d.%dm\n",distance/10,distance%10);
      }
      flag=0;// reset flag
    }
    
  }
}
/* this is the test to see other two inputs
      int16_t dataY=readData(0xAA);
      int16_t dataZ=readData(0xAC);
      printf("Output on x,y,z give: %7d----%7d---%7d\n",dataX,dataY,dataZ);
      */
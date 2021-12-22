#include <mbed.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define PI 3.14159265358979323846

volatile int flag = 0;
SPI spi(PF_9, PF_8, PF_7); // mosi, miso, sclk
DigitalOut cs(PC_1);

// Initialize for method 1:
int8_t fullTime = 30;
int8_t size = fullTime / 0.25;
int16_t dataSet[120];

// Initialize for method 2:
int steps = 0;
int state = 0;//indicate the moving have started


void setFlag();
void setMode();
int16_t readData(int code);
void calculateDistance2(int16_t dataX);
double calculateDistance1();

int main() {
  cs=1;
  setMode();
  spi.format(8,3);
  spi.frequency(100000);
	Ticker t;
  // Used for method 1
  //t.attach(&setFlag,0.05);

  // Used for method 2
  t.attach(&setFlag,0.25);
  uint8_t iter = 0;

  while(1) {
    if(flag){
      int16_t dataX = readData(0xA8);
      printf("Output temp data: %d\n",dataX);
      dataSet[iter] = dataX;
      iter = iter + 1;

    // Method 1: count steps and calculate final distance using stride.
    calculateDistance2(dataX);

    //Method 2: convert measured data to forward movement velocity and traveled distance.
      if (iter == 120){
          uint8_t i;
        	//for(i = 0; i < 120; i++){
          //  printf("%d\n", dataSet[i]);
	        //}
          double distance = calculateDistance1();
          printf("The final distance is: %d\n", (int)distance);
        //printf("Output data: %d\n",dataSet);
        break;
      }



      flag = 0;
    }
    
  }
}

void setFlag() {          
  flag = 1;
}

void setMode() {          
  cs=0;
  spi.write(0x20);
  spi.write(0xCF);
  cs=1;
}

/*
- 20% - Ability to successfully and continuously measure gyro values from the angular velocity sensor
*/
int16_t readData(int code){
  cs = 0;
  spi.write(0xE8);//X_L
  uint8_t xl =spi.write(0x00);
  cs = 1;
  cs = 0;
  spi.write(0xE9);
  int8_t xh =spi.write(0x00);
  cs = 1;
  int16_t data= xh*256+xl;
  return data;
}

/*
IDEA1:
Gyroscope is a device that produces a positive-going digital output for counter-clockwise rotation around the axis considered.
Since it's could measure the angular, we could convert the raw data into angular velocity.
According to datasheet, the sensitivity of the gyroscope. When FS = 245 dps and typical values at +25 °C,
the transfer factor is 8.75 mdps/digit. (Page 10 of the datasheet). After transformation, we could get the velocity and distance.
- 20% - Ability to convert measured data to forward movement velocity
- 20% - Ability to calculate distance traveled

*/

double calculateDistance1(){
  double realData;
  double tmpDistance;
  double totalDist = 0;
  for (int i = 0; i < 120; i++){
    dataSet[i] = abs(dataSet[i]);
    if (dataSet[i] > 5000){
      // Transform to angle velocity. 
      realData = (0.00875 * dataSet[i]);

      totalDist = totalDist + ((double) 0.25) * (realData / 360) * 2 * PI * 0.4;
    }
  }
  return totalDist;
}

/*
IDEA2:
Count steps and calculate final distance using stride.
*/


void calculateDistance2(int16_t dataX){
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
}






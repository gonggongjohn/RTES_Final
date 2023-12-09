#include "mbed.h"
#include "math.h"
#include "string.h"
// setup the SPI
// PC1 - CS (chip select for gyro)
// PF7 - SCLK (Serial clock)
// PF8 - MISO (Master in Slave out)
// PF9 - MOSI (Master out Slave in)
SPI spi(PF_9, PF_8, PF_7); // mosi, miso, sclk address in the memory
DigitalOut gyro_cs(PC_1); // chip select, the digital output pin
int i = 0; // loop variable for running the loop to collect 40 data points
int dis_x[40]; // array to store values reported by the gyroscope along x-axis
int dis_y[40]; // array to store values reported by the gyroscope along y-axis
int dis_z[40]; // array to store values reported by the gyroscope along z-axis
float dx=0; // angular velocity as recorded along the x-axis
float dy=0; // angular velocity as recorded along the y-axis
float dz=0; // angular velocity as recorded along the z-axis
float anglepitch=0.0; //to calculate the angle of the pitch
float angleroll=0.0;// to calculate the angle of the roll
int thetax=0; // to store the angle displaced along x-axis in radians
int thetay =0; // to store the angle displaced along y-axis in radians
int thetaz=0; // to store the angle displaced along z-axis in radians
int cosineV=0; // stores the cosine component of the angular velocity along x-axis
int sineV=0; // stores the sine component of the angular velocity along x-axis
int resultantV=0; // stores the resultant linear velocity along x-axis using cosineV and sineV.
int linV[40]; // array to store the calculated linear velocity (var resultantV) at all the 40 data points
// starting of the program , to read values from gyroscope
// to calculate linear velocity and distance.

int main() {
  gyro_cs = 1;
  // Setup the SPI for 8 bit data and SPI mode is 3
  spi.format(8,3);
  // second edge capture, with a 1MHz clock rate
  spi.frequency(8000000);
  // initializing the gyroscope
  // Control register 1 (0x20) set to 00001111 -> 0x0F
  // enables all axes
  // turns on the gyroscope
    gyro_cs = 0; // initializing the gyroscope
  wait_us(1000); //wait time given for the communication to be done
  spi.write(32); // this method writes the specified data to the target SPI bus and returns the number of bytes written
  spi.write(15);
  wait_us(1000);
  gyro_cs = 1;
  while(i < 40){ /*while loop to run 40 times as we have to record for 20 seconds of data for every 0.5 seconds */
    // Read data from the gyroscope
    // Read 2 bytes of data (reporting the angular velocity) along the x-axis (address code: 0xE8)
    gyro_cs = 0;
    wait_us(1000);
    spi.write(232);
    short x_axis_low = spi.write(0x00); //to write the 8-bit low values along x-axis
    short x_axis_high = spi.write(0x00); //to write the 8-bit high values along x-axis
    short x_axis = x_axis_high << 8 | x_axis_low; // to store 16 bit number
    wait_us(1000); // given wait time to SPI protocol to read the next value
    gyro_cs = 1;
    // Read 2 bytes of angular velocity data around the Y axis (address code: 0xEA)
    gyro_cs = 0;
    wait_us(1000);
    spi.write(234); //address in the memory for writing the values of Y-axis
    short y_axis_low = spi.write(0x00); //to write the 8-bit low values along y-axis
    short y_axis_high = spi.write(0x00);// to write the 8-bit high values along y-axis
    short y_axis = y_axis_high << 8 | y_axis_low; //to store the whole 16 bit number
    wait_us(1000); // given wait time to SPI protocol to read the next value
    gyro_cs = 1;
    // Read 2 bytes of angular velocity data around the Z axis (address code: 0xEC)
    gyro_cs = 0;
    wait_us(1000);
    spi.write(236); //address in the memory for writing the values of Y-axis
    short z_axis_low = spi.write(0x00); //to write the 8-bit low values along z-axis
    short z_axis_high = spi.write(0x00); //to write the 8-bit high values along z-axis
    short z_axis = z_axis_high << 8 | z_axis_low; // to store 16 bit number together
    wait_us(1000);
    gyro_cs = 1;
    dis_x[i]=abs((x_axis/pow(2,16))*245); // since we need to convert raw values to range between 0-245
    dis_y[i]=abs((y_axis/pow(2,16))*245); // since we need to convert raw values to range between 0-245
    dis_z[i]=abs((z_axis/pow(2,16))*245); // since we need to convert raw values to range between 0-245
/*we are using the formula v=rw(v=linear velocity, r average length from hip to where the microcontroller is placed on the leg, w= angular velocity */
    cosineV= 2*cos(dis_x[i])+2*cos(dis_y[i]); //we are taking average r=2ft
/* to calculate the cosine value according to the formula to calculate the velocity vector along a particular axis */
    sineV= 2*sin(dis_x[i])+2*sin(dis_y[i]); //we are taking average r =2ft
/* to calculate the sine value according to the formula to calculate the velocity vector along a particular axis */
    resultantV= sqrt(pow(cosineV,2)+ pow(sineV,2)); /*to calculate the resultant magnitude of the linear velocity vector along the x-axis from the formula*/
    linV[i]= (int)resultantV; /*array storing the linear velocity at every degree change while walking*/
    printf("Gyro - X: %d Y: %d Z: %d, i: %d\n ",  dis_x[i], dis_y[i], dis_z[i], i);
    printf("linear velocity value : %d \n",linV[i]);
    i++;
    //sampling at 0.5 seconds interval
    wait_us(100000);
    }
    //for displaying linear velocity
    printf("linV = [");
    for (int i = 0; i <40; i++)
    {
        printf("%d,", (int)(linV[i]));
    }
    printf("]\n\r\n\r");
    //for displaying x-axis angular velocities
   printf("dataX = [");
    for (int i = 0; i <40; i++)
    {
        printf("%d,", (int)(dis_x[i]));
    }
    printf("]\n\r\n\r");
    //for displaying y-axis angular velocities
    printf("dataY = [");
    for (int i = 0; i <40; i++)
    {
        printf("%d,", (int)(dis_y[i]));
    }

    printf("]\n\r\n\r");
    //for displaying z-axis angular velocities
    printf("dataZ = [");
    for (int i = 0; i <40; i++)
    {
    printf("%d,", (int)(dis_z[i]));
    }
    printf("]\n\r\n\r");
    //to calculate the distance from the values
    while(1) {
        printf("Out\n");
        for (int i=1; i<40; i++)
        {
        dx+=(dis_x[i])*0.5*0.0174533; /*converting degree/sec into radians swept along x-axis for calculation*/
        dy+=(dis_y[i])*0.5*0.0174533; /* converting degree/sec into radians swept along y-axis for calculation*/
        dz+=(dis_z[i])*0.5*0.0174533; /* / converting degree/sec into radians swept along z-axis for calculation */
        }
        //Since we are walking in only the x-y plane hence we are not considering the values along z-axis for calculating the distance travelled.
        thetax+=dx*0.0000152671; //while calculating, the sensitivity of the gyrcosope is taken to be 1000Mhz
        thetay+=dy*0.0000152671; //while calculating, the sensitivity of the gyrcosope is taken to be 1000Mhz
        // the sine function adds to the accounting of angle change periodically
        thetax+=thetay+ sin(dz*0.00006111); //angle pitch
        thetay+=thetax+ sin(dz*0.00006111); //angle roll
        //printf("%.2f" "%.2f",thetax,thetay);
        //printf("%.2f %.2f %.2f\n", dx,dy,dz);
        float dis=sqrt(pow(thetax,2)+ pow(thetay,2)); //euclidean distance formula
        int s=(int)dis*2.0; //to make up for the negative values, we multiply by 2
        printf("%d\n" , s); //printing the final distance calculated
        break;
    }
}
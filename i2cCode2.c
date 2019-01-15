#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "LSM9DS1_Registers.h"
#include "driverlib/interrupt.h"
#include "myDelay.h"
#include <math.h>

#define LSM9DS1 I2C0_BASE
#define LSM9DS1_ADDRESS_ACCELGYRO 0x6B
#define LSM9DS1_ADDRESS_MAG 0x1E
#define SENSORS_GRAVITY_EARTH 9.80665F              /**< Earth's gravity in m/s^2 */

const float PI = 3.1415;

typedef enum
{
	LSM9DS1_ACCELRANGE_2G                = (0b00 << 3),
	LSM9DS1_ACCELRANGE_16G               = (0b01 << 3),
	LSM9DS1_ACCELRANGE_4G                = (0b10 << 3),
	LSM9DS1_ACCELRANGE_8G                = (0b11 << 3),
} AccRange;

typedef enum
{
      LSM9DS1_GYROSCALE_245DPS             = (0b00 << 3),  // +/- 245 degrees per second rotation
      LSM9DS1_GYROSCALE_500DPS             = (0b01 << 3),  // +/- 500 degrees per second rotation
      LSM9DS1_GYROSCALE_2000DPS            = (0b11 << 3)   // +/- 2000 degrees per second rotation
} GyroScale;

void intiI2C(void);
void initAcc(AccRange);
void initGyro(GyroScale);

int16_t Axx = 0;
int16_t Ayy= 0;
int16_t Azz = 0;
int16_t Gxx = 0;
int16_t Gyy= 0;
int16_t Gzz = 0;

uint8_t accBuffer[6] = {0,0,0,0,0,0};
uint8_t gyroBuffer[6] = {0,0,0,0,0,0};
bool cal = false;

void I2C_write(uint32_t, uint8_t, uint8_t);
uint32_t I2C_read(uint32_t, uint8_t);
void initI2C(void);
void initAcc(AccRange);
void readAcc(void);
void readGyro(void);
float findRoll(void);
float findPitch(void);
float Gres = 0.00875;
float Ares = 0.000061;
float Ax = 0;
float Ay = 0;
float Az = 0;
float Gx = 0;
float Gy = 0;
float Gz = 0;
float roll = 0;
float pitch = 0;

uint32_t mil = 0;

int16_t cal_Gx = 0;
int32_t sum_Gx = 0;
int16_t cal_Gy = 0;
int32_t sum_Gy = 0;
int16_t cal_Gz = 0;
int32_t sum_Gz = 0;

int16_t cal_Ax = 0;
int32_t sum_Ax = 0;
int16_t cal_Ay = 0;
int32_t sum_Ay = 0;
int16_t cal_Az = 0;
int32_t sum_Az = 0;

int main()
{
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	initSysTck();
	initI2C();
	initAcc(LSM9DS1_ACCELRANGE_2G);
	initGyro(LSM9DS1_GYROSCALE_245DPS);
	for(uint16_t x = 0; x<1000; x++)
	{
		readGyro();
		sum_Gx += Gx;
		sum_Gy += Gy;
		sum_Gz += Gz;
	}
	cal_Gx = sum_Gx/1000;
	cal_Gy = sum_Gy/1000;
	cal_Gz = sum_Gz/1000;
	
	for(uint16_t x = 0; x<1000; x++)
	{
		readAcc();
		sum_Ax += Ax;
		sum_Ay += Ay;
		sum_Az += Az;
	}
	cal_Ax = sum_Ax/1000;
	cal_Ay = sum_Ay/1000;
	cal_Az = sum_Az/1000;
	
	cal = true;
	
	while(1)
	{
	readAcc();
	readGyro();
	roll = findRoll();		
	pitch = findPitch();
	mil = millis();
// SysCtlDelay(16000000u/3u);
	}
}

void I2C_write(uint32_t slave_addr, uint8_t reg, uint8_t data)
{
	I2CMasterSlaveAddrSet(LSM9DS1, slave_addr, false); // write mode
	I2CMasterDataPut(LSM9DS1, reg);
	while (I2CMasterBusy(LSM9DS1));
	I2CMasterControl(LSM9DS1, I2C_MASTER_CMD_BURST_SEND_START); //I2C_MASTER_CMD_FIFO_BURST_SEND_START
	while (I2CMasterBusy(LSM9DS1));
	I2CMasterDataPut(LSM9DS1, data);
	I2CMasterControl(LSM9DS1, I2C_MASTER_CMD_BURST_SEND_FINISH);  //I2C_MASTER_CMD_FIFO_BURST_SEND_FINISH
	while (I2CMasterBusy(LSM9DS1));
}

uint32_t I2C_read(uint32_t slave_addr, uint8_t reg)
{
	I2CMasterSlaveAddrSet(LSM9DS1, slave_addr, false);  					// write mode
	I2CMasterDataPut(LSM9DS1, reg);																//specify register to be read
	I2CMasterControl(LSM9DS1, I2C_MASTER_CMD_BURST_SEND_START);		//send control byte and register address byte to slave device
	while(I2CMasterBusy(LSM9DS1));
	I2CMasterSlaveAddrSet(LSM9DS1, slave_addr, true);							// read mode 
  I2CMasterControl(LSM9DS1, I2C_MASTER_CMD_SINGLE_RECEIVE);			//send control byte and read from the register we specified
	while(I2CMasterBusy(LSM9DS1));
	return (I2CMasterDataGet(LSM9DS1));
}

void initI2C(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
	SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
	
	GPIOPinConfigure(GPIO_PB2_I2C0SCL);
	GPIOPinConfigure(GPIO_PB3_I2C0SDA);
	
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
  GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
	
	I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
	
	HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;
	
//	GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
//	GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_OD);
}

void initAcc(AccRange range)
{
	I2C_write(LSM9DS1_ADDRESS_ACCELGYRO, CTRL_REG8, 0x05);					// soft reset accleroometer+gyro
	I2C_write(LSM9DS1_ADDRESS_ACCELGYRO, CTRL_REG2_M, 0x0C);					// soft reset mag
	SysCtlDelay(16000000u/3u);
	// Enable the accelerometer continous
	I2C_write(LSM9DS1_ADDRESS_ACCELGYRO, CTRL_REG5_XL, 0x38);
	I2C_write(LSM9DS1_ADDRESS_ACCELGYRO, CTRL_REG6_XL, 0xC0);
	
	uint8_t reg = I2C_read(LSM9DS1_ADDRESS_ACCELGYRO, CTRL_REG6_XL);
  reg &= ~(0b00011000);
  reg |= range;
	I2C_write(LSM9DS1_ADDRESS_ACCELGYRO, CTRL_REG6_XL, reg);
}


void initGyro(GyroScale scale)
{
	I2C_write(LSM9DS1_ADDRESS_ACCELGYRO, CTRL_REG1_G, 0xC0);				// enable gyro continuous on XYZ
	uint8_t reg = I2C_read(LSM9DS1_ADDRESS_ACCELGYRO, CTRL_REG1_G);
  reg &= ~(0b00011000);
  reg |= scale;
	I2C_write(LSM9DS1_ADDRESS_ACCELGYRO, CTRL_REG1_G, reg);				// 
}

void readAcc(void)
{
	accBuffer[0] = I2C_read(LSM9DS1_ADDRESS_ACCELGYRO, OUT_X_L_XL);
	accBuffer[1] = I2C_read(LSM9DS1_ADDRESS_ACCELGYRO, OUT_X_H_XL);
	accBuffer[2] = I2C_read(LSM9DS1_ADDRESS_ACCELGYRO, OUT_Y_L_XL);
	accBuffer[3] = I2C_read(LSM9DS1_ADDRESS_ACCELGYRO, OUT_Y_H_XL);
	accBuffer[4] = I2C_read(LSM9DS1_ADDRESS_ACCELGYRO, OUT_Z_L_XL);
	accBuffer[5] = I2C_read(LSM9DS1_ADDRESS_ACCELGYRO, OUT_Z_H_XL);
	Axx = (accBuffer[1]<<8) | accBuffer[0];
	Ax = Axx * Ares;
	Ayy = (accBuffer[3]<<8) | accBuffer[2];
	Ay = Ayy * Ares;
	Azz = (accBuffer[5]<<8) | accBuffer[4];
	Az = Azz * Ares;
	if(cal)
	{
		Ax = Ax - cal_Ax;
		Ay = Ay - cal_Ay;
		Az = Az - cal_Az;
	}
}

void readGyro(void)
{
	gyroBuffer[0] = I2C_read(LSM9DS1_ADDRESS_ACCELGYRO, OUT_X_L_G);
	gyroBuffer[1] = I2C_read(LSM9DS1_ADDRESS_ACCELGYRO, OUT_X_H_G);
	gyroBuffer[2] = I2C_read(LSM9DS1_ADDRESS_ACCELGYRO, OUT_Y_L_G);
	gyroBuffer[3] = I2C_read(LSM9DS1_ADDRESS_ACCELGYRO, OUT_Y_H_G);
	gyroBuffer[4] = I2C_read(LSM9DS1_ADDRESS_ACCELGYRO, OUT_Z_L_G);
	gyroBuffer[5] = I2C_read(LSM9DS1_ADDRESS_ACCELGYRO, OUT_Z_H_G);
	Gxx = (gyroBuffer[1]<<8) | gyroBuffer[0];
	Gx = Gxx * Gres;
	Gyy = (gyroBuffer[3]<<8) | gyroBuffer[2];
	Gy = Gyy * Gres;
	Gzz = (gyroBuffer[5]<<8) | gyroBuffer[4];
	Gz = Gzz * Gres; 
	if(cal)
	{
		Gx = Gx - cal_Gx;
		Gy = Gy - cal_Gy;
		Gz = Gz - cal_Gz;
	}
}

float findRoll(void)
{
	float _roll = atan2(Ay, Az);
	_roll  *= 180.0 / PI;
	return _roll;
}

float findPitch(void)
{
	float _pitch = atan2(-Ax, sqrt(Ay * Ay + Az * Az));
	_pitch *= 180.0 / PI;
	return _pitch;
}
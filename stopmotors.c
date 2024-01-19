
#include "mindsensors-motormux.h"
#include "PC_FileIO.c"

void configureAllSensors();

task main()
{
	configureAllSensors();
    MSMotorStop(mmotor_S1_1);
    MSMotorStop(mmotor_S1_2);

}

void configureAllSensors()
{
	SensorType[S4] = sensorEV3_Touch;
	wait1Msec(50);
	SensorType[S3] = sensorEV3_Gyro;
	wait1Msec(50);
	SensorMode[S3] = modeEV3Gyro_Calibration;
	wait1Msec(100);
	SensorMode[S3] = modeEV3Gyro_RateAndAngle;
	wait1Msec(50);

	nMotorEncoder[motorA] = 0;
	nMotorEncoder[motorB] = 0;
	nMotorEncoder[motorC] = 0;
	nMotorEncoder[motorD] = 0;

	// Initialize, for the multiplexer connected to S1
	SensorType[S1] = sensorI2CCustom;
	MSMMUXinit();

	// Reset multiplexer motor encoders
	MSMMotorEncoderReset(mmotor_S1_1);
	MSMMotorEncoderReset(mmotor_S1_2);
}
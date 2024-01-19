/**
 * Authors: Khushi Patel, Meghan Dang, Rubie Luo, Evie Bouganim
 * Description: Final 1A Mechatronics project. 'Thing' is a robotic
 * 				hand that can perform American Sign Language (ASL)
 * 				letters from file input. Each inputted word is screened
 * 				for profanity. After signing all words, 'high five'
 * 				the touch sensor for it to wave goodbye.
 * Version: 4.0
 * Acknowledgements: We would like to thank the MTE 121 and MTE 100
 * 					 teaching team for supporting us and providing
 * 					 some materials throughout the project development.
 */

#include "mindsensors-motormux.h"
#include "PC_FileIO.c"

// Got approval from Prof. Nassar to use global constants here
enum Fingers
{
	THUMB,
	INDEX,
	MIDDLE,
	RING,
	PINKY
};

const int ENCODER_HOME = 0;
const int CLOSE_LIM = 100;
const int HALF_LIM = 35;
const int MOTOR_POWER = 35;
const int TURN_POWER = 13;
const int ROLL_JOINT = 25;
const int WAVE_POWER = 20;

// Configures all sensors
void configureAllSensors();

// Waits for amount of seconds
void waitSeconds(int seconds);

// Moves each finger
void moveFinger(enum Fingers finger, int encoderTarget);

// Rotate wrist joint
void moveRoll(int gyroLimit, int motor_power);

// Reset Position
void resetHand();
void resetNCase();

// Sign letter
void sign(char letter);

// Waves hello
void wave();

// Checks of input is profane
bool profanityCheck(char *input);
char cipherText(char input);
void toLower(char *input);

task main()
{
	configureAllSensors();

	// Open file
	TFileHandle fin;
	bool fileOkay = openReadPC(fin, "sign.txt");

	// If file does not open, display error for 10 seconds and end program
	if (!fileOkay)
	{
		displayString(5, "Error! Could not open file.");
		waitSeconds(5);
		wave();
		return;
	}

	// Wait for enter button to be clicked to begin sign language
	while (!getButtonPress(buttonEnter))
	{
	}
	while (getButtonPress(buttonEnter))
	{
	}
	waitSeconds(3);

	// Declare word input string to store the input
	string wordInput;
	// Create pointer to the first letter of the string, used to divide each char later
	char *letter = &wordInput;
	// Read text from file
	readTextPC(fin, wordInput);
	// Convert the string to lowercase for consistency
	toLower(letter);

	// Check if word contains profanity, if it does, it will not sign.
	if (!profanityCheck(wordInput))
	{
		// If word does not have profanity, iterate through each letter and sign it
		for (int i = 0; i < strlen(wordInput); i++)
		{
			sign(*(letter + i)); // Pointer to the char to sign
			waitSeconds(2);
			if (*(letter + i) != 'n')
			{
				resetHand();
				waitSeconds(0.5);
			}
		}
		displayString(5, "Done! High five to exit :)");
	}

	while (!SensorValue[S4])
	{
	}
	while (SensorValue[S4])
	{
	}
	wave();

	return;
}

/**
 * Configures all sensors
 *
 * @return None
 */
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

/**
 * Waits given amount of time
 *
 * @param seconds Time in seconds to wait
 * @return None
 */
void waitSeconds(int seconds)
{
	clearTimer(T1);
	while (time1[T1] <= seconds * 1000)
	{
	}
}

/**
 * Move a specified finger to desired position
 *
 * @param finger Finger (enum) to move
 * @param encoderTarget Position to move specified finger
 * @return None
 */
void moveFinger(enum Fingers finger, int encoderTarget)
{
	// Switch case statement to move the finger that is passed in the function
	switch (finger)
	{
	case THUMB:
		motor[motorD] = MOTOR_POWER;
		while (nMotorEncoder[motorD] < encoderTarget)
		{
		}
		motor[motorD] = 0;
		break;

	case INDEX:
		motor[motorC] = MOTOR_POWER;
		while (nMotorEncoder[motorC] < encoderTarget)
		{
		}
		motor[motorC] = 0;
		break;

	case MIDDLE:
		motor[motorB] = MOTOR_POWER;
		while (nMotorEncoder[motorB] < encoderTarget)
		{
		}
		motor[motorB] = 0;
		break;

	case RING:
		MSMMotor(mmotor_S1_1, MOTOR_POWER);
		while (MSMMotorEncoder(mmotor_S1_1) < encoderTarget)
		{
		}
		MSMotorStop(mmotor_S1_1);
		break;

	case PINKY:
		MSMMotor(mmotor_S1_2, MOTOR_POWER);
		while (MSMMotorEncoder(mmotor_S1_2) < encoderTarget)
		{
		}
		MSMotorStop(mmotor_S1_2);
		break;
	}
}

/**
 * Move roll joint to specified angle
 *
 * @param encoderLimit Position to rotate to
 * @param motor_power motor power for rotation
 * @return None
 */
void moveRoll(int gyroLimit, int motor_power)
{
	motor[motorA] = motor_power;

	// case for counterclockwise and clockwise rotation
	if (motor_power < 0)
		while (abs(getGyroDegrees(S3)) < gyroLimit)
		{
		}
	else
		while (getGyroDegrees(S3) > gyroLimit)
		{
		}

	motor[motorA] = 0;
}

/**
 * Resets position for all motors
 *
 * @return None
 */
void resetHand()
{
	moveRoll(ENCODER_HOME, TURN_POWER);

	MSMMotor(mmotor_S1_2, -MOTOR_POWER);
	while (MSMMotorEncoder(mmotor_S1_2) > 0)
	{
	}
	MSMotorStop(mmotor_S1_2);

	MSMMotor(mmotor_S1_1, -MOTOR_POWER);
	while (MSMMotorEncoder(mmotor_S1_1) > 0)
	{
	}
	MSMotorStop(mmotor_S1_1);

	motor[motorB] = -MOTOR_POWER;
	while (nMotorEncoder[motorB] > 0)
	{
	}
	motor[motorB] = 0;

	motor[motorC] = -MOTOR_POWER;
	while (nMotorEncoder[motorC] > 0)
	{
	}
	motor[motorC] = 0;

	motor[motorD] = -MOTOR_POWER;
	while (nMotorEncoder[motorD] > 0)
	{
	}
	motor[motorD] = 0;
}

/**
 * Resets specific fingers first for the 'n' sign so that no positional conflicts
 *
 * @return None
 */
void resetNCase()
{
	motor[motorC] = -MOTOR_POWER;
	while (nMotorEncoder[motorC] > 0)
	{
	}
	motor[motorC] = 0;

	motor[motorB] = -MOTOR_POWER;
	while (nMotorEncoder[motorB] > 0)
	{
	}
	motor[motorB] = 0;

	motor[motorD] = -MOTOR_POWER;
	while (nMotorEncoder[motorD] > 0)
	{
	}
	motor[motorD] = 0;

	MSMMotor(mmotor_S1_1, -MOTOR_POWER);
	while (MSMMotorEncoder(mmotor_S1_1) > 0)
	{
	}
	MSMotorStop(mmotor_S1_1);

	MSMMotor(mmotor_S1_2, -MOTOR_POWER);
	while (MSMMotorEncoder(mmotor_S1_2) > 0)
	{
	}
	MSMotorStop(mmotor_S1_2);
}

/**
 * Signs a letter in ASL by moving each finger to position
 *
 * @param letter Letter to sign
 * @return None
 */
void sign(char letter)
{
	switch (letter)
	{
	case 'a':
		moveFinger(INDEX, CLOSE_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(THUMB, HALF_LIM);
		break;

	case 'b':
		moveFinger(THUMB, CLOSE_LIM);
		break;

	case 'c':
		moveRoll(ROLL_JOINT, -TURN_POWER);
		moveFinger(INDEX, HALF_LIM);
		moveFinger(MIDDLE, HALF_LIM);
		moveFinger(RING, HALF_LIM);
		moveFinger(PINKY, HALF_LIM);
		moveFinger(THUMB, HALF_LIM);
		break;

	case 'd':
		moveRoll(ROLL_JOINT, -TURN_POWER);
		moveFinger(THUMB, HALF_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(PINKY, CLOSE_LIM);
		break;

	case 'e':
		moveFinger(THUMB, CLOSE_LIM);
		moveFinger(INDEX, CLOSE_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(PINKY, CLOSE_LIM);
		break;

	case 'f':
		moveFinger(THUMB, HALF_LIM);
		moveFinger(INDEX, HALF_LIM);
		break;

	case 'g':
		moveRoll(ROLL_JOINT, -TURN_POWER);
		moveFinger(MIDDLE, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(THUMB, CLOSE_LIM);
		break;

	case 'h':
		moveRoll(ROLL_JOINT, -TURN_POWER);
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(THUMB, CLOSE_LIM);
		break;

	case 'i':
		moveFinger(INDEX, CLOSE_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(THUMB, HALF_LIM);
		break;

	case 'j':
		moveRoll(ROLL_JOINT, -TURN_POWER);
		moveFinger(INDEX, CLOSE_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(THUMB, HALF_LIM);
		moveRoll(ENCODER_HOME, TURN_POWER);
		break;

	case 'k':
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(THUMB, CLOSE_LIM);
		break;

	case 'l':
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		break;

	case 'm':
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(THUMB, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		moveFinger(INDEX, CLOSE_LIM);
		break;

	case 'n':
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(THUMB, CLOSE_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		moveFinger(INDEX, CLOSE_LIM);
		waitSeconds(2);
		resetNCase();
		break;

	case 'o':
		moveRoll(ROLL_JOINT, -TURN_POWER);
		moveFinger(INDEX, CLOSE_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(THUMB, HALF_LIM);
		break;

	case 'p':
		moveRoll(ROLL_JOINT, -TURN_POWER);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(THUMB, HALF_LIM);
		moveFinger(MIDDLE, HALF_LIM);
		break;

	case 'q':
		moveRoll(ROLL_JOINT, -TURN_POWER);
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		break;

	case 'r':
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(THUMB, CLOSE_LIM);
		break;

	case 's':
		moveFinger(INDEX, CLOSE_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(THUMB, HALF_LIM);
		break;

	case 't':
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		moveFinger(THUMB, CLOSE_LIM);
		moveFinger(INDEX, CLOSE_LIM);
		break;

	case 'u':
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(THUMB, CLOSE_LIM);
		break;

	case 'v':
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(THUMB, HALF_LIM);
		break;

	case 'w':
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(THUMB, CLOSE_LIM);
		break;

	case 'x':
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		moveFinger(THUMB, CLOSE_LIM);
		moveFinger(INDEX, HALF_LIM);
		break;

	case 'y':
		moveFinger(INDEX, CLOSE_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		break;

	case 'z':
		moveRoll(ROLL_JOINT, -TURN_POWER);
		moveFinger(PINKY, CLOSE_LIM);
		moveFinger(RING, CLOSE_LIM);
		moveFinger(MIDDLE, CLOSE_LIM);
		moveFinger(THUMB, CLOSE_LIM);
		moveRoll(0, TURN_POWER);
		break;

	case ' ':
		waitSeconds(2);
		break;
	}
}

/**
 * 'Waves' fingers goodbye
 *
 * @return None
 */
void wave()
{
	int direction = 1;

	for (int count = 1; count <= 4; count++)
	{
		if (count % 2 == 0)
		{
			direction = -1;
			motor[motorC] = WAVE_POWER * direction;
			motor[motorB] = WAVE_POWER * direction;
			MSMMotor(mmotor_S1_2, WAVE_POWER * direction);
			MSMMotor(mmotor_S1_1, WAVE_POWER * direction);
			while (MSMMotorEncoder(mmotor_S1_1) > 0)
			{
			}
			motor[motorC] = 0;
			motor[motorB] = 0;
			MSMMotor(mmotor_S1_2, 0);
			MSMMotor(mmotor_S1_1, 0);
			waitSeconds(0.75);
		}
		else
		{
			direction = 1;
			motor[motorC] = WAVE_POWER * direction;
			motor[motorB] = WAVE_POWER * direction;
			MSMMotor(mmotor_S1_2, WAVE_POWER * direction);
			MSMMotor(mmotor_S1_1, WAVE_POWER * direction);
			while (MSMMotorEncoder(mmotor_S1_1) < 40)
			{
			}
			motor[motorC] = 0;
			motor[motorB] = 0;
			MSMMotor(mmotor_S1_2, 0);
			MSMMotor(mmotor_S1_1, 0);
			waitSeconds(0.75);
		}
	}
}

/**
 * Checks if string input is profane
 *
 * @param input Char pointer to first letter in input word
 * @return whether the word is profane or not
 */
bool profanityCheck(char *input)
{
	// Constant number of words that have been censored.
	const int NUM_PROFANE = 7;
	// Array of characters (string) to store text (MAX OF 21 CHARACTERS)
	char cipheredWord[20] = " ";
	// Initialize string to store array of chars as a string
	string cipheredString = "";

	// Pass each character of the array of chars to cipher it
	for (int i = 0; i < strlen(input); i++)
	{
		cipheredWord[i] = cipherText(input[i]);
	}

	// Convert array of chars into one string
	stringFromChars(cipheredString, cipheredWord);

	// Array of profane words to be censored, all ciphered
	string profaneWords[NUM_PROFANE] = {"kzhp", "xmny", "gnyhm", "xjc", "hzsy", "xqzy", "bmtwj"};

	// Iterate through each word in profanity array, make sure the string does not contain any of them
	for (int i = 0; i < NUM_PROFANE; i++)
	{
		// If the string does contain profanity, display will not sign and return 1.
		if (stringFind(cipheredString, profaneWords[i]) != -1)
		{
			displayString(3, "This word is inappropriate, will not sign. Touch sensor to exit");
			return true;
		}
	}

	return false; // if not profane
}

/**
 * Ciphers text, shifting characters 5 ahead in the alphabet
 *
 * @param input Character to shift
 * @return shifted character
 */
char cipherText(char input)
{
	// Add 5 to each character
	char ciphered = input + 5;

	// If the character has gone past the alphabet, wrap around
	if (input > 122)
	{
		input = input - 26;
	}

	return ciphered;
}

/**
 * Converts string to all lowercase and removes special characters
 * 		'toLower()' function not available with RobotC
 *
 * @param input Char pointer to first character in string
 * @return None
 */
void toLower(char *input)
{
	// Iterate through each letter of the string
	for (int i = 0; i < strlen(input); i++)
	{
		// If character is a special character (i.e, not alphabet), remove.
		if (input[i] < 64 || (input[i] > 90 && input[i] < 97))
		{
			input[i] = ' ';
		}
		// If character is uppercase, change to lowercase.
		else if (input[i] < 90)
		{
			input[i] = input[i] + 32;
		}
	}
}
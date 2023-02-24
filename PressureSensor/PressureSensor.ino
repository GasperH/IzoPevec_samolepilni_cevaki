/* This example demonstrates how to take a standard 3-wire pressure transducer
 *  and read the analog signal, then convert the signal to a readable output and
 *  display it onto an LCD screen.
 *  
 *  Contact Tyler at tylerovens@me.com if you have any questions
 */

#include "Wire.h" //allows communication over i2c devices
#include "LiquidCrystal_I2C.h" //allows interfacing with LCD screens

const int pressureInput = A0; //select the analog input pin for the pressure transducer
const int pressureZero = 95; //analog reading of pressure transducer at 0bar
const int pressureMax = 583; //analog reading of pressure transducer at 6bar
const int pressuretransducermaxPSI = 150; //psi value of transducer being used
const int baudRate = 9600; //constant integer to set the baud rate for serial monitor
const int sensorreadDelay = 250; //constant integer to set the sensor read delay in milliseconds

float pressureValue = 0; //variable to store the value coming from the pressure transducer
float pressureValuebar = 0;

void setup() //setup routine, runs once when system turned on or reset
{
  Serial.begin(baudRate); //initializes serial communication at set baud rate bits per second
}

void loop() //loop routine runs over and over again forever
{
  pressureValue = analogRead(pressureInput); //reads value from input pin and assigns to variable
  Serial.print(pressureValue);
  pressureValuebar = (pressureValue - pressureZero)*6/(pressureMax - pressureZero); //conversion equation to convert analog reading to psi
  Serial.print(pressureValuebar); //prints value from previous line to serial
  Serial.println("bar"); //prints label to serial
  delay(sensorreadDelay); //delay in milliseconds between read values
}

#include <AccelStepper.h>
#include <Servo.h>


 
//User-defined values
long receivedSteps = 0; //Number of steps
long receivedSpeed = 0; //Steps / second
long receivedAcceleration = 0; //Steps / second^2
long initialHoming = -1;
char receivedCommand;
//-------------------------------------------------------------------------------
int directionMultiplier = 1; // = 1: positive direction, = -1: negative direction
bool newData, runallowed = false; // booleans for new data from serial, and runallowed flag
AccelStepper stepper(1, 8, 9);// direction Digital 9 (CCW), pulses Digital 8 (CLK)

int button0 = 11;
int buttonState0 = 0;

int button1 = 12;
int buttonState1 = 0;

int button2 = 13;
int buttonState2 = 0;

const int relay = 7;

Servo servozg;
Servo servosp;
 
void setup(){

    pinMode(button0, INPUT_PULLUP); //stikalo za ukaz, nesklenjeno vrača 1
    pinMode(button1, INPUT_PULLUP); //končno stikalo pri lepilnih trakovih, nesklenjeno vrača 1
    pinMode(button2, INPUT_PULLUP); //končno stikalo stran od lepilnih trakov, nesklenjeno vrača 1
    pinMode(relay, OUTPUT);
    servozg.attach(5);
    servosp.attach(3);
    Serial.begin(9600); //define baud rate
    stepper.setMaxSpeed(1000); //SPEED = Steps / second
    stepper.setAcceleration(800); //ACCELERATION = Steps /(second)^2

    digitalWrite(relay, HIGH); //Streha se spusti, če je slučajno zgoraj
    servosp.write(50); //spodnji servo se zapre, če je slučajno odprt
    delay(1000);
    servozg.write(0); //zgornji servo se odpre, da se lahko vrne na začetno pozicijo
    delay(1000);
    while (!digitalRead(button1)){ //pomična miza se premakne do lepilnih trakov
      stepper.moveTo(initialHoming); //premakne se za initialHoming
      initialHoming--; //initialHoming zmanjša za 1
      stepper.run(); //dejanski premik 
      delay(0.5);}
    stepper.setCurrentPosition(-100); //končno stikalo je pri -100
    while (stepper.currentPosition() != 0){ //premakne mizo na 0
      stepper.moveTo(0);
      stepper.run();
      delay(0.5);
    }
//    while (digitalRead(button1) == 0 and digitalRead(button2) == 0){
////      stepper.run();
//      //stepper.enableOutputs(); //enable pins
//      receivedSpeed = 1000;
//      receivedSteps = -19000;
//      RotateRelative();
//      RunTheMotor();  
//      }
//    //Serial.println(digitalRead(button1)); //print the action
//    Serial.println(button1);
//    runallowed = false;
//    stepper.setCurrentPosition(-100);

//    delay(1500);
//    
//    servozg.write(70); //zgornji servo prime trak
//    delay(1500);
    
    servosp.write(45); //spodnji servo spusti trak
    delay(1500);
      
//    while (stepper.currentPosition() != 18500 and digitalRead(button2) == 0){
//      receivedSteps = 18500;
//      receivedSpeed = 5000;
//      RotateAbsolute();
//      RunTheMotor();}
//
//    delay(1000);

//    if(digitalRead(button2)== 1){
//      while (digitalRead(button1) == 0){
//        receivedSpeed = 1000;
//        receivedSteps = -19000;
//        RotateRelative();
//        RunTheMotor();}
//
//      runallowed = false;
//      stepper.setCurrentPosition(-100);
//
//      delay(1500);
//    
//      servozg.write(70); //zgornji servo prime trak
//      delay(1500);
//    
//      servosp.write(45); //spodnji servo spusti trak
//      delay(1500);
//      
//      while (stepper.currentPosition() != 18500 and digitalRead(button2) == 0){
//        receivedSteps = 18500;
//        receivedSpeed = 5000;
//        RotateAbsolute();
//        RunTheMotor();}
//
//    delay(1000);
//      
//      }
//    
    stepper.disableOutputs(); //disable outputs, so that the stepper doesnt get hot
}

void loop() {
  // put your main code here, to run repeatedly:

}

void RunTheMotor() //function for the motor
{
    if (runallowed == true)
    {
        stepper.enableOutputs(); //enable pins
        stepper.run(); //step the motor (this will step the motor by 1 step at each loop)  
    }
    else //program enters this part if the runallowed is FALSE, we do not do anything
    {
        stepper.disableOutputs(); //disable outputs
        return;
    }
}

void RotateRelative()
{
    //We move X steps from the current position of the stepper motor in a given direction.
    //The direction is determined by the multiplier (+1 or -1)
   
    runallowed = true; //allow running - this allows entering the RunTheMotor() function.
    stepper.setMaxSpeed(receivedSpeed); //set speed
    stepper.move(directionMultiplier * receivedSteps); //set relative distance and direction
}

void RotateAbsolute()
{
    //We move to an absolute position.
    //The AccelStepper library keeps track of the position.
    //The direction is determined by the multiplier (+1 or -1)
    //Why do we need negative numbers? - If you drive a threaded rod and the zero position is in the middle of the rod...
 
    runallowed = true; //allow running - this allows entering the RunTheMotor() function.
    stepper.setMaxSpeed(receivedSpeed); //set speed
    stepper.moveTo(directionMultiplier * receivedSteps); //set relative distance   
}

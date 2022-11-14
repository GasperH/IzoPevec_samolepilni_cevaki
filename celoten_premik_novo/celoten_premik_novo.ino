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
    servosp.write(45);
    servozg.attach(5);
    servosp.attach(3);
    Serial.begin(9600); //define baud rate
    stepper.setMaxSpeed(1000); //SPEED = Steps / second
    stepper.setAcceleration(800); //ACCELERATION = Steps /(second)^2
    digitalWrite(relay, HIGH); //Streha se spusti, če je slučajno zgoraj
    servosp.write(92); //spodnji servo se zapre, če je slučajno odprt
    delay(1000);
    servozg.write(0); //zgornji servo se odpre, da se lahko vrne na začetno pozicijo
    delay(1000);
    servozg.detach(); //odklopi servozg, da počaka odprt na glavno zanko
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
    stepper.disableOutputs(); //nastavi vse pine stepperja na LOW
}
//Ob koncu kalibracije je spodnji servo zaprt, zgornji odprt, miza pa na 0.
void loop() {
    servozg.write(0); //zgornji servo je odprt
    delay(500);
    servozg.attach(5);
    servozg.write(60); //zgornji servo prime lepilni trak
    delay(500);
    servosp.write(0); //spodnji se odpre in spusti trak
    delay(500);
    servosp.detach();
    stepper.enableOutputs();
    stepper.setAcceleration(2000);
    stepper.setMaxSpeed(5000);
    while (stepper.currentPosition() != 18500){ //miza se premakne z lepilnim trakom na drugo stran
      stepper.moveTo(18500); 
      stepper.run();  
      }
    stepper.disableOutputs(); // ko miza pride na končno pozicijo, se izklopi
    delay(1000);
    digitalWrite(relay, LOW); //Streha se dvigne
    servosp.attach(3);
    servosp.write(92); //spodnji se zapre in prime trak na začetku
    delay(500);
    while(digitalRead(button0)){ //čaka na gumb, da se cevak namesti
      delay(1);
      }
    digitalWrite(relay, HIGH); //Streha se spusti
    delay(500);
    while(digitalRead(button0)){ //čaka na gumb, da se lepilni trak nalepi na cevak
      delay(1);
      }    
    servozg.write(60); //ko je gumb pritisnjen se zgornji servo odpre
    servozg.detach();
    delay(500);
    while(digitalRead(button0)){ //čaka na gumb, da se lepilni trak odlepi od servota
      delay(1);
      }  
    while (stepper.currentPosition() != 0){ //miza se premakne nazaj na začetek
      stepper.moveTo(0); 
      stepper.run();  
      }
    
}

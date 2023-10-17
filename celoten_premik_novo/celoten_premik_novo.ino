#include <AccelStepper.h>
#include "Wire.h" //allows communication over i2c devices
#include "LiquidCrystal_I2C.h" //allows interfacing with LCD screens

long initialHoming = -1;
bool newData, runallowed = false; // booleans for new data from serial, and runallowed flag
AccelStepper stepper(1, 13, 11);// direction Digital 9 (CCW), pulses Digital 8 (CLK)
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
long st_obdelanih_cevakov = 0;

byte zeleni_gumb = A1; //zeleni gumb SW1
bool buttonState0 = 0;

byte kon_gumb = A0; //koncno stikalo SW2
bool buttonState1 = 0;

byte streha = 8;
byte premicnoprijemalo = 7;
byte staticnoprijemalo = 9;

char pressureInput = A3; //select the analog input pin for the pressure transducer
const int pressureZero = 95; //analog reading of pressure transducer at 0bar
const int pressureMax = 583; //analog reading of pressure transducer at 6bar

const float pretvorba_poti = 8.49;

float tlak = 0; //zacetna vrednost tlaka
float tlak_v_barih = 0; 
 
void setup(){

    lcd.init();                      // initialize the lcd 
    lcd.backlight();
    pinMode(zeleni_gumb, INPUT); //stikalo za ukaz, nesklenjeno vraca 0
    pinMode(kon_gumb, INPUT); //koncno stikalo pri lepilnih trakovih, nesklenjeno vraca 0
    pinMode(streha, OUTPUT);
    pinMode(premicnoprijemalo , OUTPUT);
    pinMode(staticnoprijemalo , OUTPUT);
    Serial.begin(9600); //define baud rate
    preveriTlak();
    digitalWrite(streha, LOW); //Streha se spusti, ce je slucajno zgoraj
    preveriTlak();
    digitalWrite(staticnoprijemalo, LOW); //Zacetno prijemalo zaprto
    delay(1000);
    preveriTlak();
    digitalWrite(premicnoprijemalo, LOW); //Premicno prijemalo odprto
    delay(1000);
    kalibracija();
    lcd.print("Pritisnite gumb za nastavitev traku.");
    premor_do_zelenega_gumba();//caka na gumb, za prvi start
    lcd.clear();
    preveriTlak();
    digitalWrite(staticnoprijemalo, HIGH); //Zacetno prijemalo odprto da nastavis trak
}
//Ob koncu kalibracije je spodnje prijemalo odprto, zgornje odprto, miza pa na 0.
void loop() {
    preveriTlak();
    digitalWrite(premicnoprijemalo, LOW); //Premicno prijemalo odprto
    delay(500);
    lcd.print("Stevilo dobrih cevakov: " + String(st_obdelanih_cevakov));
    premor_do_zelenega_gumba();
    lcd.clear();
    lcd.print("Nastavi trak in pritisni gumb.");
    premor_do_zelenega_gumba();//caka na gumb, da matej nastima trak
    lcd.clear();
    preveriTlak();
    digitalWrite(premicnoprijemalo, HIGH); //Premicno prijemalo zaprto
    delay(500);
    preveriTlak();
    digitalWrite(staticnoprijemalo, HIGH); //Zacetno prijemalo odprto in spusti trak
    delay(500);
    lcd.print("Za zacetek pritisni gumb");
    premor_do_zelenega_gumba();//caka na gumb, da matej odmakne roko
    lcd.clear(); 
    stepper.enableOutputs();
    stepper.setAcceleration(1000);
    stepper.setMaxSpeed(2000);
    preveriTlak();
    long pot = 2115*pretvorba_poti;
    stepper.moveTo(pot);
    while (stepper.currentPosition() != pot){ //miza se premakne z lepilnim trakom na drugo stran
      stepper.run(); 
      preveriTlak(); 
      }
    preveriTlak();
    digitalWrite(staticnoprijemalo, LOW); //Zacetno prijemalo zaprto in prime trak na zacetku
    delay(1500);
    stepper.setAcceleration(300);
    stepper.setMaxSpeed(300);
    long pot1 = pot + 8*pretvorba_poti;
    stepper.moveTo(pot1);
    while (stepper.currentPosition() != pot1){ //nateg lepila na 2160mm
      stepper.run();
      preveriTlak(); 
      }
    stepper.disableOutputs(); // ko miza pride na koncno pozicijo, se izklopi
    lcd.print("ce je trak lepo potegnjen, pritisni gumb.");
    premor_do_zelenega_gumba();//caka na gumb, da pregledas ce se je kul potegnl
    lcd.clear();
    preveriTlak();
    digitalWrite(streha, HIGH); //Streha se dvigne
    delay(500);
    lcd.print("Namest cevak in pritisni gumb.");
    premor_do_zelenega_gumba();//caka na gumb, da se cevak namesti
    lcd.clear();
    preveriTlak();
    digitalWrite(streha, LOW); //Streha se spusti
    delay(500);
    lcd.print("Stisni cevak, da se lepilni trak prilepi nanj.");
    premor_do_zelenega_gumba();//caka na gumb, da se lepilni trak nalepi na cevak
    lcd.clear();    
    preveriTlak();
    digitalWrite(premicnoprijemalo, LOW); //Premicno prijemalo odprto
    delay(1000);
    lcd.print("Odstranite cevak.");
    premor_do_zelenega_gumba();//caka na gumb, da se lepilni trak odlepi od prijemala
    lcd.clear();
    lcd.print("Dober cevak - zeleni gumb, slab cevak - koncni gumb");
    delay(1000);
    byte pogoj = 0;
    while(!pogoj){
      if(digitalRead(zeleni_gumb)){
        st_obdelanih_cevakov = st_obdelanih_cevakov + 1;
        pogoj = 1;
      }
      if(!digitalRead(kon_gumb)){
        pogoj = 1;        
      }
    }  
    lcd.clear();
    stepper.enableOutputs(); //Stepper se prizge
    stepper.setAcceleration(2000);
    stepper.setMaxSpeed(5000);
    stepper.moveTo(0);
    while (stepper.currentPosition() != 0){ //miza se premakne nazaj na zacetek
      stepper.run();
      preveriTlak();  
      }
    
}

void preveriTlak(){
  tlak = analogRead(pressureInput); //reads value from input pin and assigns to variable
  tlak_v_barih = (tlak - pressureZero)*6/(pressureMax - pressureZero); //conversion equation
  if (tlak_v_barih < 0.5){
    lcd.print("Prenizek zracni tlak");
    while (tlak_v_barih < 0.5){
      delay(250);
      tlak = analogRead(pressureInput); //reads value from input pin and assigns to variable
      tlak_v_barih = (tlak - pressureZero)*6/(pressureMax - pressureZero); //conversion equation
    }
    lcd.clear();
  }
}

void kalibracija(){
  stepper.setMaxSpeed(1000); //SPEED = Steps / second
  stepper.setAcceleration(800); //ACCELERATION = Steps /(second)^2
  float cas = 0;
  while (cas < 1){ //pomicna miza se premakne do lepilnih trakov
      stepper.moveTo(initialHoming); //premakne se za initialHoming
      initialHoming--; //initialHoming zmanjsa za 1
      stepper.run(); //dejanski premik 
      preveriTlak();
      if (digitalRead(kon_gumb) == 0){
        cas = cas + 1;       
      }
      }
  stepper.setCurrentPosition(-100); //koncno stikalo je pri -100
  stepper.moveTo(0);
  while (stepper.currentPosition() != 0){ //premakne mizo na 0
    stepper.run();
  }
  stepper.disableOutputs(); //nastavi vse pine stepperja na LOW  
}

void premor_do_zelenega_gumba(){
  while(!digitalRead(zeleni_gumb)){
    delay(1);
  }
}

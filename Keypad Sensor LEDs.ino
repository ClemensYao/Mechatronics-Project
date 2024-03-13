#include <Keypad.h> //calls upon keypad function

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //four columns
const int echoPin = 24; //sensor pin in
const int trigPin = 22; //sensor pin out
int ledPins[] = {52, 50, 48, 46, 44, 42, 40}; //Initialize Pin Array
int ledPinCount = 7; //Number of LEDs present
int thisPin = 0; //Specifies the pin index
int count = 0; //set count to zero
long target; //initializes a long integer
long duration, cm; //establish variables for duration of the ping and the distance result
int lightMode = 0; //Variable to determine if even or odd LEDs are on

char keys[ROW_NUM][COLUMN_NUM] = { //sets keypad matrix into array
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM ); //uses keypad function
String inputString; //initializes a string
char key = keypad.getKey(); // use object name and dot operator to access getKey() function which returns the key that is pressed.

//New variables
int GlobalDelay = 50; //Runs entire code at 50ms delay
int State = 0;
bool SetTarget = false;
bool Measure = false;
bool Start = true;
int PreRangeCounter = 0;
int TargetRangeCounter = 0;
int thisPinPR;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); //sets data to be sent

  //Prints Introduction and Instructions
  Serial.println("****************************");
  Serial.println("Aloha! Project #1: Group C"); //introduction
  Serial.println("Press *: Start of measurement");
  Serial.println("Press #: End of measurement");
  Serial.println("****************************");
  inputString.reserve(10); //length of inputInt

  //Establishes LED pins to output
  for(count = 0; count < 7; count++){
    pinMode(ledPins[count], OUTPUT);
  }

  //Establishes pin modes for the sensor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  char key = keypad.getKey();
  //If key is pressed set a target and stop measuring if target already established

  if (Start) {
    if (key == '*') {
      Serial.println("Input a number between 3 and 49 and press A to continue");
      SetTarget = true;
      Start = false;
    }
  }

  if (key == '#') {
    OffLED();
    Serial.println("New measurement");
    Serial.println("Input a number between 3 and 49 and press A to continue");
    Measure = false;
    SetTarget = true;
  }

  //Set a target
  if (SetTarget) {
    if (key >= '0' && key <= '9' && SetTarget) {     
      inputString += key;              
    }
  if (key == 'A'){
    target = inputString.toInt(); 
    inputString = ""; 
    if (target < 3 || target > 49) {
      Serial.print("Wrong input! ");
      Serial.println("Input a number between 3 and 49"); 
    }
  else {
    Serial.print("Your target distance is: "); 
    Serial.print(target); 
    Serial.print("cm"); 
    Serial.println();               
    delay(1000); 
    SetTarget = false;  
    Measure = true;      
    }
  }
  }

  //Tell code to take a measurement and print if condition is true
  if (Measure) {
    digitalWrite(trigPin, LOW); 
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(trigPin, LOW);

    duration = pulseIn(echoPin, HIGH); 
    cm = microsecondsToCentimeters(duration); 
    Serial.print(cm); 
    Serial.print("cm");
    Serial.println(); 


  //If distance above 50 and below 300 turn on all LEDs
  if (cm >= 50 && cm <= 300) { 
    State = 1;
    WholeRange();
    ResetPR();
    ResetTR();
  }

  //If distance too low or too high turn on all LEDs
  if (cm > 300 || cm < 3) {
    State = 2;
    OffLED();
    ResetPR();
    ResetTR();
  }

  //PreTarget Range LED trail w/ 1 second delay
  if (cm > target && cm < 50) { 
    if (State != 3){ //check previous state of LEDs, if not current state reset lights
      State = 3;
      OffLED();
    }
    ResetTR();
    PreRangeCounter++;
    if (PreRangeCounter >= 1000/GlobalDelay) {
      digitalWrite(ledPins[thisPinPR], LOW);
      thisPinPR++;
      if (thisPinPR == 7){
        thisPinPR = 0;
      }
      PreRangeCounter = 0;
    }
    digitalWrite(ledPins[thisPinPR], HIGH);

  }

  // Target Range LED trail w/ .1 second delay
  if (cm <= target && cm >= 3) { 
    if (State != 4){ 
      State = 4;
      OffLED();
    }
    ResetPR();
    TargetRangeCounter++;
    for(thisPin = 0; thisPin < ledPinCount; thisPin++) {
      if((thisPin % 2) == lightMode){
        digitalWrite(ledPins[thisPin], HIGH);
      }
      else{
        digitalWrite(ledPins[thisPin], LOW);
      }
    }
    //Switches the light mode from even to odd
    if (TargetRangeCounter >= 100/GlobalDelay){
      lightMode = abs(lightMode--);
      TargetRangeCounter = 0;
    }
  }   
  }
  if (!SetTarget && !Measure) {
    OffLED();
  }
  delay(GlobalDelay);
}//end void loop

//Functions
void OffLED() {
  // Turns off all LEDs
  for(thisPin = 0; thisPin < ledPinCount; thisPin++){
    digitalWrite(ledPins[thisPin], LOW);
  }
}
void WholeRange() {
  // Turns on all LEDs
  for(thisPin = 0; thisPin < ledPinCount; thisPin++){
    digitalWrite(ledPins[thisPin], HIGH);
  }
}
void ResetPR() {
  thisPinPR = 0;
  PreRangeCounter = 0;
}
void ResetTR() {
  TargetRangeCounter = 0;
}
long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}
#include <multiCameraIrControl.h>
const int LEDPin[8]={3, 7, 14, 16, 19, 4, 13, 15}; //7segment led pins
const int UP = 10; //For Up Button
const int DOWN = 17; //For Down Button
const int segPin[4]={2, 5, 6, 12}; //7segment dig pin
const int START = 9;  //For start/stop Switch
Sony Nex=8; //For Ir Out
int counter = 5; //Default shutter time 5s
int rate = 200; //Default speed of digit change on Button press
int shot = 0; //Shot count
void setup() {
  // Set up
  pinMode(LEDPin[0], OUTPUT);  //A
  pinMode(LEDPin[1], OUTPUT);  //B
  pinMode(LEDPin[2], OUTPUT);  //C
  pinMode(LEDPin[3], OUTPUT);  //D
  pinMode(LEDPin[4], OUTPUT);  //E
  pinMode(LEDPin[5], OUTPUT);  //F
  pinMode(LEDPin[6], OUTPUT);  //G
  pinMode(LEDPin[7], OUTPUT);  //DP
  pinMode(UP, INPUT);  //UP
  pinMode(DOWN, INPUT);  //DOWN
  pinMode(segPin[0], OUTPUT);  //DIG1
  pinMode(segPin[1], OUTPUT);  //DIG2
  pinMode(segPin[2], OUTPUT);  //DIG3
  pinMode(segPin[3], OUTPUT);  //DIG4
  pinMode(START, INPUT);  //START/Stop switch for staging
}
void DisplaySingleDigit(int digit)
{
 //Display Single Digit on 7 segment display
 //Conditions for displaying segment a
 if(digit!=1 && digit != 4)
 digitalWrite(LEDPin[0],HIGH);
 
 //Conditions for displaying segment b
 if(digit != 5 && digit != 6)
 digitalWrite(LEDPin[1],HIGH);
 
 //Conditions for displaying segment c
 if(digit !=2)
 digitalWrite(LEDPin[2],HIGH);
 
 //Conditions for displaying segment d
 if(digit != 1 && digit !=4 && digit !=7)
 digitalWrite(LEDPin[3],HIGH);
 
 //Conditions for displaying segment e 
 if(digit == 2 || digit ==6 || digit == 8 || digit==0)
 digitalWrite(LEDPin[4],HIGH);
 
 //Conditions for displaying segment f
 if(digit != 1 && digit !=2 && digit!=3 && digit !=7)
 digitalWrite(LEDPin[5],HIGH);
  //Conditions for displaying segment g
 if (digit!=0 && digit!=1 && digit !=7)
 digitalWrite(LEDPin[6],HIGH);
 }
 void turnOff()
{
  //Clear 7 segment display
  digitalWrite(LEDPin[0],LOW);
  digitalWrite(LEDPin[1],LOW);
  digitalWrite(LEDPin[2],LOW);
  digitalWrite(LEDPin[3],LOW);
  digitalWrite(LEDPin[4],LOW);
  digitalWrite(LEDPin[5],LOW);
  digitalWrite(LEDPin[6],LOW);
}


void displayDigit(int digit, int t){
  //Display a 4 digit number (digit) for (t) ms
  int InDigit[4]; 
  unsigned long StartT = millis(); // Record start time of this function
  unsigned long EndT = StartT + t; //End condition of this function
  
  for(int i = 3; i >= 0; i--) {
    //Store a 4 digit number in a array
      if(digit <= 0){
      InDigit[i] = 0;}
      else {InDigit[i] = digit % 10;
      digit /= 10;}
      }
  while(unsigned long NowT = millis() < EndT){
    //7 segment display take turn to display their digit respectively
    for (int b=0; b<4; b++){
       turnOff();
      for(int a=0; a<4; a++){
        digitalWrite(segPin[a],HIGH);
        }
        digitalWrite(segPin[b],LOW);
      DisplaySingleDigit(InDigit[b]);
        delay(5); //Refresh Frequency ~50Hz
      }
    }
  } 

void loop() {
int digit = 0; //reset digit display
  if(digitalRead(START) == LOW){
  //for setting up the counter
      displayDigit(counter,5);
      if (digitalRead(UP) == LOW){
        if(counter < 9999){
	      counter++;
      }
        displayDigit(counter,rate);
        if(rate>=5){
          //increase rate when long press
          rate = rate-5;
        }
        }
      else if (digitalRead(DOWN) == LOW){
       if(counter > 0){
	       counter--;
       }
        displayDigit(counter,rate);
        if(rate>=5){
          //increase rate when long press
          rate = rate-5;
        }
      }
      else {
        //reset when no button press
        rate=200;
        shot = 0;
        }

  }
  else if (digitalRead(START) == HIGH){ 
    //shutter bulb mode
	
        //counter should be set to shutter - 1 if shutter time on camera is pre-set
		
      displayDigit(shot,3000); //Display shot count, buffer time is set to 3s
      Nex.shutterNow();//Open shutter when blub mode is chosen on camera / Take a photo
	  
      digit = counter;
         while(digit>0){
          displayDigit(digit,1000);
          digit--;
          }
       Nex.shutterNow();//Close shutter when blub mode is chosen on camera / Do nothing
       shot++;

  }
 }


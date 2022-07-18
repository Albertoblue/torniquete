#define MAX_BITS 100                 // max number of bits 
#define WEIGAND_WAIT_TIME  3000      // time to wait for another weigand pulse.  
 
unsigned char databits[MAX_BITS];    // stores all of the data bits
unsigned char bitCount;              // number of bits currently captured
unsigned char flagDone;              // goes low when data is currently being captured
unsigned int weigand_counter;        // countdown until we assume there are no more bits
 
unsigned long facilityCode=0;        // decoded facility code
unsigned long cardCode=0;            // decoded card code

int LED_GREEN = 11;
int DOOR=8;
int LED_RED = 12;
int BEEP_BEEP = 10;
String code="";
String data;

// interrupt that happens when INTO goes low (0 bit)
void ISR_INT0() {
  //Serial.print("0");   // uncomment this line to display raw binary
  bitCount++;
  flagDone = 0;
  weigand_counter = WEIGAND_WAIT_TIME;  
 
}
 
// interrupt that happens when INT1 goes low (1 bit)
void ISR_INT1() {
  //Serial.print("1");   // uncomment this line to display raw binary
  databits[bitCount] = 1;
  bitCount++;
  flagDone = 0;
  weigand_counter = WEIGAND_WAIT_TIME;  
}
 
void setup() {
  pinMode(DOOR,OUTPUT);
  digitalWrite(DOOR,HIGH);
  pinMode(LED_RED, OUTPUT);  
  pinMode(LED_GREEN, OUTPUT);  
  pinMode(BEEP_BEEP, OUTPUT);  
  digitalWrite(LED_RED, HIGH); // High = Off
  digitalWrite(BEEP_BEEP, LOW); // High = off
  digitalWrite(LED_GREEN, LOW);  // Low = On
  pinMode(2, INPUT);     // DATA0 (INT0)
  pinMode(3, INPUT);     // DATA1 (INT1)
 
  Serial.begin(9600);
 
  // binds the ISR functions to the falling edge of INTO and INT1
  attachInterrupt(0, ISR_INT0, FALLING);  
  attachInterrupt(1, ISR_INT1, FALLING);
 
 
  weigand_counter = WEIGAND_WAIT_TIME;
}
 
void loop(){


  readCard();
 
  
   if(Serial.available() !=0){
    data=Serial.readStringUntil('\n');
    if(data=="Adelante"){
      digitalWrite(DOOR,LOW);
      digitalWrite(BEEP_BEEP,HIGH);
      delay(1000);
      digitalWrite(BEEP_BEEP,LOW);
      delay(2000);
      digitalWrite(DOOR,HIGH);
    }
    else{
      digitalWrite(BEEP_BEEP,HIGH);
      delay(200);
      digitalWrite(BEEP_BEEP,LOW);
      delay(200);
      digitalWrite(BEEP_BEEP,HIGH);
      delay(200);
      digitalWrite(BEEP_BEEP,LOW);
      delay(200);
      digitalWrite(BEEP_BEEP,HIGH);
      delay(200);
      digitalWrite(BEEP_BEEP,LOW);
    }
   
  }
}
  

  


void readCard(){

  // This waits to make sure that there have been no more data pulses before processing data
  if (!flagDone) {
    if (--weigand_counter == 0)
      flagDone = 1;  
  }
 
  // if we have bits and we the weigand counter went out
  if (bitCount > 0 && flagDone) {
    unsigned char i;
 
    if (bitCount == 32) {
      // 35 bit HID Corporate 1000 format
      // facility code = bits 2 to 14
      for (i=0; i<32; i++) {

         code=code+databits[i];
      }

      printBits();
     
    }
 
     // cleanup and get ready for the next card
     bitCount = 0;
     facilityCode = 0;
     cardCode = 0;
     code="";
     for (i=0; i<MAX_BITS; i++) 
     {
       databits[i] = 0;
     }
  }
}



  

 
void printBits() {
      Serial.println(code);
 
}

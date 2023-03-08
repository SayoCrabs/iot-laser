#include <Arduino.h>
int trigger = 14;
int groundair = 13;
int pin = 27;

const int length = 2;
const int length2 = 3;
int start[length2] = {1,1,0};
int sequence[length];

int airSequence[length] = {1,0};
int groundSequence[length] = {0,1};

void setup() {
  Serial.begin(9600); 
  pinMode(pin, OUTPUT);
  pinMode(groundair, INPUT);
  pinMode(trigger, INPUT);
  if(digitalRead(groundair) == 0 ){
    sequence[0] = airSequence[0];
    sequence[1] = airSequence[1];
  }else{
    sequence[0] = groundSequence[0];
    sequence[1] = groundSequence[1];
  }
}

void SendRS232Sequence(int startsequence[],int lengthstart,int sequencearray[], int lengthseq)//, int length) 
{
   for (int i = 0; i < lengthstart; i++){
    
    digitalWrite(pin, startsequence[i]);
    delay(20);
  }
  for (int i = 0; i < lengthseq; i++){
    
    digitalWrite(pin, sequencearray[i]);
    delay(20);
  }
}
 
void loop() {
if(digitalRead(trigger) == 1){
  SendRS232Sequence(start,length2,sequence,length);
  }
} 

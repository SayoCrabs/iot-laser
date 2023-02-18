#include <Arduino.h>
int pin = 2;

const int length = 2;
const int length2 = 3;
int start[length2] = {1,1,0};
int sequence[length] = {1,0};

void setup() {
  Serial.begin(9600); 
  pinMode(pin, OUTPUT);

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
  SendRS232Sequence(start,length2,sequence,length);
  //  digitalWrite(pin, HIGH);
  //  delay(1);
  //  digitalWrite(pin, LOW);
  //  delay(1);
}

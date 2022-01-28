#include <ezButton.h>

ezButton button1(2);  // create ezButton object that attach to pin 6;
ezButton button2(4);  // create ezButton object that attach to pin 7;

void setup() {
  Serial.begin(19200);
  button1.setDebounceTime(50); // set debounce time to 50 milliseconds
  button2.setDebounceTime(50); // set debounce time to 50 milliseconds
}

void loop() {
  button1.loop(); // MUST call the loop() function first
  button2.loop(); // MUST call the loop() function first

int btn1State = button1.getState();
int btn2State = button2.getState();
if(btn1State==0){
  String msg="{"+String(1)+"}";
  char *to_send = &msg[0];
  Serial.println(to_send);
}
else if(btn2State==0){
  String msg="{"+String(2)+"}";
  char *to_send = &msg[0];
  Serial.println(to_send);  
}
else{
  String msg="{"+String(0)+"}";
  char *to_send = &msg[0];
  Serial.println(to_send);
 }
  Serial.flush();   
  delay(30);
}

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
  String msg="{"+String(btn1State)+","+String(btn2State)+"}";
  char *to_send = &msg[0];
  Serial.write(to_send);
  Serial.println();
}

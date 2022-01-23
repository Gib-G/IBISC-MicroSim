#include <ezButton.h>

ezButton button1(2);  // create ezButton object that attach to pin 6;
ezButton button2(4);  // create ezButton object that attach to pin 7;

void setup() {
  Serial.begin(9600);
  button1.setDebounceTime(50); // set debounce time to 50 milliseconds
  button2.setDebounceTime(50); // set debounce time to 50 milliseconds
}

void loop() {
  button1.loop(); // MUST call the loop() function first
  button2.loop(); // MUST call the loop() function first

  int btn1State = button1.getState();
  int btn2State = button2.getState();
  Serial.print("{");
  Serial.print(btn1State);
  Serial.print(",");
  Serial.print(btn2State);
  Serial.print("}");
  Serial.println();
}

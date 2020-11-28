#include <IRremote.h>
#include <EEPROM.h>

#define fan 4
#define light 3
#define buz 13
#define IRpin 2

bool bo = true, bu = false, li = true, fa = true;

IRrecv irrecv(IRpin);
decode_results results;


void setup() {
  pinMode (buz, OUTPUT);
  pinMode (light, OUTPUT);
  pinMode (fan, OUTPUT);

  updateState();

  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver

  buzzer(1000, true);
}


void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX); // Print the Serial 'results.value'
    delay(5);

    //system test
    if (results.value == 0xFF02FD || results.value == 0x20DF22DD || results.value == 0x25) {
      buzzer(100, true);
      delay(100);
      buzzer(100, true);
    }

    //buzzer on/off
    else if (results.value == 0xFFB04F || results.value == 0x20DF08F7 || results.value == 0x0) {
      digitalWrite(buz, bu);
      bu = !bu;
    }
    else if (results.value == 0xFF6897 || results.value == 0x20DFC03F || results.value == 0x11) {
      bo = false;
      buzzer(100, true);
    }
    else if (results.value == 0xFF9867 || results.value == 0x20DF40BF || results.value == 0x10) {
      bo = true;
      buzzer(200, true);
    }


    //relay
    else if (results.value == 0xFF629D || results.value == 0x20DFC23D || results.value == 0x3B) {
        li=true;
        digitalWrite(light,li);
        buzzer(50,bo);
        
        delay(5000);

        li=false;
        buzzer(50,bo);
        digitalWrite(light,li);
    }

    else if (results.value == 0xFFA25D || results.value == 0x20DF10EF || results.value == 0xc) {
      if (li || fa) {

        li = false;
        fa = false;
        digitalWrite(light, li);
        digitalWrite(fan, fa);

        buzzer(50, bo);

        EEPROM.update(0, li);
        EEPROM.update(1, fa);

        delay(500);
      }
    }

    else if (results.value == 0xFFE21D || results.value == 0x20DF906F || results.value == 0xD) {
      if (!li || !fa) {

        li = true;
        fa = true;
        digitalWrite(light, li);
        digitalWrite(fan, fa);
        buzzer(50, bo);
        EEPROM.update(0, li);
        EEPROM.update(1, fa);

        delay(500);
      }
    }

    else if (results.value == 0xFF30CF || results.value == 0x20DF8877 || results.value == 0x1) {
      li = !li;
      digitalWrite(light, li);

      buzzer(50, bo);

      EEPROM.update(0, li);

      delay(500);
    }
    else if (results.value == 0xFF18E7 || results.value == 0x20DF48B7 || results.value == 0x2) {
      fa = !fa; 
      digitalWrite(fan, fa);
      
      buzzer(50, bo);
      EEPROM.update(1, fa);

      delay(500);
    }

    irrecv.resume();   // Receive the next value
  }
}

//------------------------function-----------
void updateState() {
  digitalWrite(light, EEPROM.read(0));
  digitalWrite(fan, EEPROM.read(1));
  li = EEPROM.read(0);
  fa = EEPROM.read(1);
}

void buzzer(int a, bool b) {
  if (b) {
    digitalWrite(buz, 1);
    delay(a);
    digitalWrite(buz, 0);
  }
}

#include <IRremote.h>
#include <EEPROM.h>

/*
  address-var
    0-fanSpeed
    1-relay1
    2-relay2
    3-relay3
    4-sound
*/


#define buz 13
#define IRpin 2

#define relay1 10
#define relay2 11
#define relay3 12

#define swi A0

#define rs1 A1
#define rs2 A2
#define rs3 A3

int fanSpeed = 1;
bool bs = true, bz = true, r1 = false, r2 = false, r3 = false, en = true, eeprom = true;

//manual switch checktiog
bool ms1 = false, ms2 = false, ms3 = false;

//extra for lock remote
int i = 0;
bool ex = false;

IRrecv irrecv(IRpin);
decode_results results;


void setup() {
  pinMode (buz, OUTPUT);

  pinMode (swi, INPUT);
  pinMode (rs1, INPUT);
  pinMode (rs2, INPUT);
  pinMode (rs3, INPUT);

  pinMode (relay1, OUTPUT);
  pinMode (relay2, OUTPUT);
  pinMode (relay3, OUTPUT);

  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver

  buzzer(1000, true);

  if (digitalRead(swi))  {
    updateState();
  }
}


void loop() {

  //remote mode on
  onSound();
  while (digitalRead(swi)) {

    //ir disable
    if (irrecv.decode(&results) && !en ) {
      delay(5);
      disableMode();
      irrecv.resume();
    }

    //ir enable
    else if (irrecv.decode(&results) && en ) {
      Serial.println(results.value, HEX);
      delay(5);
      buttonFunctionMode();
      irrecv.resume();
    }
    manualWithIr();
    //    Serial.println((String)"auto="+digitalRead(swi)+"\tm1="+digitalRead(rs1)+"\tm2="+digitalRead(rs2)+"\tm=3"+digitalRead(rs3));
    //    Serial.println((String)"relay1="+r1+"\trelay2="+r2+"\trelay3="+r3);
  }

  //manual mode on
  onSound();
  while (!digitalRead(swi)) {
    manualWithoutIr();
  }

}






//--------------------------------------function-----------------------------------------


void updateState() {
  fanSpeed = EEPROM.read(0);
  r1 = EEPROM.read(1);
  r2 = EEPROM.read(2);
  r3 = EEPROM.read(3);
  bs = EEPROM.read(4);

  ms1 = digitalRead(rs1);
  ms2 = digitalRead(rs2);
  ms3 = digitalRead(rs3);

  digitalWrite(relay1, r1);
  digitalWrite(relay2, r2);
  digitalWrite(relay3, r3);
}

void buzzer(int a, bool b) {
  if (b) {
    digitalWrite(buz, 1);
    delay(a);
    digitalWrite(buz, 0);
  }
}

void onSound() {
  buzzer(150, true);
  delay(400);
  buzzer(550, true);
}


void disableMode() {

  //enter (system test)
  if (results.value == 0xFF02FD) {
    buzzer(100, true);
    delay(100);
    buzzer(100, true);

    i = 0;
    ex = false;
  }

  //-----------<> enable
  else if (results.value == 0xFFC23D) {
    ex = true;
  }
  else if (results.value == 0xFFFFFFFF && ex ) {
    delay(100);
    if (i == 20) {
      en = true;
      buzzer(100, true);
      delay(400);
      buzzer(500, true);
    }
    i++;
  }

  //-----------</>enable
}


void buttonFunctionMode() {

  //enter (system test)
  if (results.value == 0xFF02FD || results.value == 0x25) {
    buzzer(100, true);
    delay(100);
    buzzer(100, true);

    i = 0;
    ex = false;
  }

  //numbers-------------------------

  //button(fan speed state) 0
  else if (results.value == 0xFFB04F || results.value == 0x0) {
    for (int i = 0; i < fanSpeed; i++) {
      buzzer(70, bs);
      delay(130);
    }
  }

  // button(on/off) 1
  else if (results.value == 0xFF30CF || results.value == 0x1) {
    r1 = !r1;
    digitalWrite(relay1, r1);
    if (eeprom) {
      EEPROM.update(1, r1);
    }
    buzzer(50, bs);
    delay(100);
  }

  // button(on/off) 2
  else if (results.value == 0xFF18E7 || results.value == 0x2) {
    r2 = !r2;
    digitalWrite(relay2, r2);
    if (eeprom) {
      EEPROM.update(2, r2);
    }
    buzzer(50, bs);
    delay(100);
  }

  //button(on/off) 3
  else if (results.value == 0xFF7A85 || results.value == 0x3) {
    r3 = !r3;
    digitalWrite(relay3, r3);
    if (eeprom) {
      EEPROM.update(3, r3);
    }
    buzzer(50, bs);
    delay(100);
  }

  // button(on) 4
  else if (results.value == 0xFF10EF || results.value == 0x4) {
    if (eeprom && !r1) {
      EEPROM.update(1, true);
    }
    r1 = true;
    digitalWrite(relay1, r1);
    buzzer(50, bs);
    delay(100);
  }

  // button(on) 5
  else if (results.value == 0xFF38C7 || results.value == 0x5) {
    if (eeprom && !r2) {
      EEPROM.update(2, true);
    }
    r2 = true;
    digitalWrite(relay2, r2);
    buzzer(50, bs);
    delay(100);
  }

  //button(on) 6
  else if (results.value == 0xFF5AA5 || results.value == 0x6) {
    if (eeprom && !r3) {
      EEPROM.update(3, true);
    }
    r3 = true;
    digitalWrite(relay3, r3);
    buzzer(50, bs);
    delay(100);
  }

  // button(off) 7
  else if (results.value == 0xFF42BD || results.value == 0x7) {
    if (eeprom && r1) {
      EEPROM.update(1, false);
    }
    r1 = false;
    digitalWrite(relay1, r1);
    buzzer(50, bs);
    delay(100);
  }

  // button(off) 8
  else if (results.value == 0xFF4AB5 || results.value == 0x8) {
    if (eeprom && r2) {
      EEPROM.update(2, false);
    }
    r2 = false;
    digitalWrite(relay2, r2);
    buzzer(50, bs);
    delay(100);
  }

  //button(off) 9
  else if (results.value == 0xFF52AD || results.value == 0x9) {
    if (eeprom && r3) {
      EEPROM.update(3, false);
    }
    r3 = false;
    digitalWrite(relay3, r3);
    buzzer(50, bs);
    delay(100);
  }
  //numbers end-----------------------


  //vol- (fanSpeed)
  else if (results.value == 0xFF6897 || results.value == 0x11) {
    fanSpeed--;
    if (fanSpeed < 1) {
      fanSpeed = 1;
    }
    //
    if (eeprom) {
      EEPROM.update(0, i);
    }
    for (int i = 0; i < fanSpeed; i++) {
      buzzer(50, bs);
      delay(100);
    }
  }

  //vol+ (fanSpeed)
  else if (results.value == 0xFF9867 || results.value == 0x10) {
    fanSpeed++;
    if (fanSpeed > 4) {
      fanSpeed = 4;
    }
    //
    if (eeprom) {
      EEPROM.update(0, i);
    }
    for (int i = 0; i < fanSpeed; i++) {
      buzzer(50, bs);
      delay(100);
    }
  }


  //power (all relay off)
  else if (results.value == 0xFFA25D || results.value == 0xC) {
    if (r1 || r2 || r3) {
      r1 = false;
      r2 = false;
      r3 = false;
      digitalWrite(relay1, r1);
      digitalWrite(relay2, r2);
      digitalWrite(relay3, r3);

      if (eeprom) {
        EEPROM.update(1, r1);
        EEPROM.update(2, r2);
        EEPROM.update(3, r3);
      }
      buzzer(50, bs);
      delay(100);
    }
  }


  //stop (temporary light)
  else if (results.value == 0xFF629D) {

    if (eeprom && r1) {
      EEPROM.update(1, false);
    }

    r1 = true;
    buzzer(50, bs);
    digitalWrite(relay1, r1);

    delay(7000);

    r1 = false;
    buzzer(50, bs);
    digitalWrite(relay1, r1);

    delay(100);
  }


  //mute (buzzer on/off)
  else if (results.value == 0xFFE21D || results.value == 0xD) {
    bs = !bs;

    if (eeprom) {
      EEPROM.update(4, bs);
    }

    if (bs) {
      onSound();
    }
    else {
      buzzer(500, true);
    }

  }

  //mode (eprom en/dis)
  else if (results.value == 0xFF22DD) {
    eeprom = !eeprom;
    if (eeprom) {
      onSound();
    }
    else {
      buzzer(500, true);
    }
  }

  //-----------------------eq (enable)
  else if (results.value == 0xFFC23D) {
    ex = true;
  }
  else if (results.value == 0xFFFFFFFF && ex ) {
    delay(100);
    if (i == 20) {
      en = false;
      buzzer(500, true);
    }
    i++;
  }
}


void manualWithIr() {

  //manual swithch 1 with ir
  if (digitalRead(rs1) != ms1) {
    if (digitalRead(rs1)) {
      r1 = true;
    }
    else {
      r1 = false;
    }
    digitalWrite(relay1, r1);
    delay(200);
    if (eeprom) {
      EEPROM.update(1, r1);
    }
  }
  ms1 = digitalRead(rs1);

  //manual switch 2 with ir
  if (digitalRead(rs2) != ms2) {
    if (digitalRead(rs2)) {
      r2 = true;
    }
    else {
      r2 = false;
    }
    digitalWrite(relay2, r2);
    delay(200);
    if (eeprom) {
      EEPROM.update(2, r2);
    }
  }
  ms2 = digitalRead(rs2);

  //manual switch 3 with ir
  if (digitalRead(rs3) != ms3) {
    if (digitalRead(rs3)) {
      r3 = true;
    }
    else {
      r3 = false;
    }
    digitalWrite(relay3, r3);
    delay(200);
    if (eeprom) {
      EEPROM.update(3, r3);
    }
  }
  ms3 = digitalRead(rs3);
}


void manualWithoutIr() {
  //relay 1
  if (digitalRead(rs1) && !r1) {
    r1 = true;
    digitalWrite(relay1, r1);
    delay(100);
    buzzer(50, bs);
  }
  else if (!digitalRead(rs1) && r1) {
    r1 = false;
    digitalWrite(relay1, r1);
    delay(100);
    buzzer(50, bs);
  }

  //relay 2
  if (digitalRead(rs2) && !r2) {
    r2 = true;
    digitalWrite(relay2, r2);
    delay(100);
    buzzer(50, bs);
  }
  else if (!digitalRead(rs2) && r2) {
    r2 = false;
    digitalWrite(relay2, r2);
    delay(100);
    buzzer(50, bs);
  }

  //relay 3
  if (digitalRead(rs3) && !r3) {
    r3 = true;
    digitalWrite(relay3, r3);
    delay(100);
    buzzer(50, bs);
  }
  else if (!digitalRead(rs3) && r3) {
    r3 = false;
    digitalWrite(relay3, r3);
    delay(100);
    buzzer(50, bs);
  }

}

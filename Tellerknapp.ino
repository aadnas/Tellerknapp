// Tis is test software
#include <SPI.h>
#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
#define DEBOUNCE 10  // Debouncer for knapper, 5+ ms er normalt nok. Test ut

#define ledPin 13
#define hvit 28

byte buttons[] = {54, 55, 56, 57, 58}; // Analoge pinner 0-5 er ogs? kjent som pin 14-19 p? Arduino UNO
#define solenoidButton 59
#define NUMBUTTONS sizeof(buttons) // Denne macroen forteller programmet hvor mange knapper vi har ved ? lese av hvor mange tall vi har definert i "buttons[]"
volatile byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS]; // Vi holder rede p? om en knapp er "just pressed", "just released", eller "currently pressed"

long buttonPushTime; // Variabel som lagrer millis() n?r en knapp trykkes inn
long buttonPushTime2; // Variabel som lagrer millis() n?r en knapp trykkes inn
long buttonDelay = 500; // Delay f?r en verdi begynner ? rulle dersom en knapp holdes inne
int solstate = 0;
enum operatingState {EN, TO, TRE, FIRE};
operatingState opState = EN;
int tid2 = 40;
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2); // Forteller LCD-bibliotket layout p? v?rt display
  lcd.clear(); // Wiper displayet for eventuell gammel informasjon
  lcd.setCursor(0, 0);
  lcd.print("Test");
  pinMode(ledPin, OUTPUT);
  pinMode(hvit, OUTPUT);
  byte i;

pinMode(solenoidButton, INPUT);


  // Setter knappepinnene til input og enabler innebyggede pull-up resistorer p? disse pinnene
  for (i = 0; i < NUMBUTTONS; i++) {
    pinMode(buttons[i], INPUT);
    digitalWrite(buttons[i], HIGH);
  }
  noInterrupts();
  TCCR3A = 0;
  TCCR3B = 0;

  //TCNT1 = 49911;
  TCCR3B |= (1 << CS32);
  TCCR3B |= (1 << CS30);
  TIMSK3 |= (1 << TOIE3);


  // INTERRUPT TIMER
  // Kj?rer timer2 interrupt hvert 15. ms
  TCCR2A = 0;
  TCCR2B = 1 << CS22 | 1 << CS21 | 1 << CS20;

  //Timer2 Overflow Interrupt Enable
  TIMSK2 |= 1 << TOIE2;

  interrupts();
}
SIGNAL(TIMER2_OVF_vect) {
  check_switches();
}
SIGNAL(TIMER3_OVF_vect) {
  teller();
  digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
}
unsigned long antall;
void teller() {
  antall = antall + 1;
  TCNT3 = 49910;
}

void loop() {
  Serial.println(antall);
  //      KNAPPER
  for (byte i = 0; i < NUMBUTTONS; i++) {

    // Det som skjer i det en knapp trykkes inn
    if (justpressed[i]) {
      justpressed[i] = 0;
      //      printTempScreen();
      buttonPushTime = millis(); // Setter en delay f?r verdien l?per n?r en knapp er trykket inn kontinuerlig
      buttonPushTime2 = millis(); // Setter en delay f?r verdien l?per n?r en knapp er trykket inn kontinuerlig
    }
    if (justreleased[i]) {
      switch (i) {
        case 0:
          lcd.clear();
          sett();
          opState = EN;
          break;
        case 1:
          hoeyre();
          break;
        case 2:
          opp();
          antall = 0;
          opState = FIRE;
          break;
        case 3:
          venstre();
          antall = 0;
          opState = TO;
          break;
        case 4:
          ned();
          break;
          // switch SLUTT
      }
      justreleased[i] = 0;
    } // if (justreleased) SLUTT
    if (pressed[i]) {
      buttonPushTime = millis(); // Denne srger for at skjermen ikke skifter tilbake til RUN_PID s lenge en knapp er trykket inn
      switch (i) {
        case 0:
          //           setValue();
          break;
        case 1:
          if (buttonPushTime2 + buttonDelay < millis()) {
            //  lcd.clear();
            //  changeValueDown();
          }
          break;
        case 2:
          //          changeScreenRight();
          break;
        case 3:
          if (buttonPushTime2 + buttonDelay < millis()) {
            //  lcd.clear();
            //  changeValueUp();
          }
          break;
        case 4:
          //          changeScreenLeft();
          break;
      } // switch SLUTT

    } // if (pressed) SLUTT
  } // Knapper SLUTT
  //===============================================
  //Det virker som ønsket uten denne delen
  solstate = digitalRead(solenoidButton);
  if (solstate == HIGH){
    antall = 0;
    if(opState = TO){
      if(antall <= 7){
        digitalWrite(hvit, HIGH);
      }
      else {
        digitalWrite(hvit, LOW);
      }
    }
  }
  //===============================================
  switch (opState) {
    case EN:
      en();
      break;
    case TO:
      to();
      break;
    case TRE:
      tre();
      break;
    case FIRE:
      fire();
      break;
  }
}
void en() {

}
void to() {
  if (opState = TO) {
    if (antall >= 4) {
      lcd.setCursor(0, 1);
      lcd.print("Tekst 1");
      if (antall >= 10) {
        lcd.setCursor(0, 0);
        lcd.print("Tekst 2");
      }
      if (antall >= 15) {
        antall = 0;
        lcd.clear();
        opState = TRE;
      }
    }
  }
}
void tre() {
  lcd.setCursor(6, 0);
  lcd.print("Tekst 3");
}
void fire() {
  if (opState = FIRE) {
    if (antall >= tid2) {
      lcd.setCursor(0, 1);
      lcd.print("HUHEI!");
    }
  }
}
void opp() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("opp");
}
void ned() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ned");
}
void venstre() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("venstre");
}
void hoeyre() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("hoeyre");
}
void sett() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("sett");

}
void check_switches() {
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;

  if (millis() < lasttime) {
    // we wrapped around, lets just try again
    lasttime = millis();
  }

  if ((lasttime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return;
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();

  for (index = 0; index < NUMBUTTONS; index++) {
    currentstate[index] = digitalRead(buttons[index]);   // read the button
    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
        // just pressed
        justpressed[index] = 1;
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
        // just released
        justreleased[index] = 1;
      }
      pressed[index] = !currentstate[index];  // remember, digital HIGH means NOT pressed
    }
    previousstate[index] = currentstate[index];   // keep a running tally of the buttons
  }
}

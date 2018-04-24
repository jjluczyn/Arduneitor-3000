#include <LiquidCrystal.h>

//LCD variables
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
char BLANKLINE[16] = "               ";

//Communication variables
char messageRead[16];
boolean connectionState = true;

// Alarm variables
boolean alarmState = false;
int alarmIter = 0;
boolean lastAlarmIterState = true;
const int ALARM_PIN = 10;

void setup() {
  lcd.begin(16, 2);

  Serial.begin(9600);

  pinMode(ALARM_PIN, OUTPUT);
}

void loop() {
  if(connectionState){
    if(Serial.available()){
    writeLCD("Leyendo Mensaje.",1);
		if(Serial.available()>15){
			Serial.readBytes(messageRead,16);
			processMessage(messageRead,16);
		}
	} else {
		writeLCD("No Mensaje Disp.",1);
	}
  } else {
	  connectionLost();
  }

  // Update alarm state
  updateAlarmState();

  delay(100);
}

void connectionLost(){
    writeLCD("Conexion Perdida.",1);
}

void writeLCD(String message, int line){
  lcd.setCursor(0,line);
  lcd.print(BLANKLINE);
  lcd.setCursor(0,line);
  lcd.print(message);
}

void processMessage(char* message, int size){
	switch(message[0]){
		case 'T':
      updateTemperature(parse2CharsToInt(message[1],message[2]),parse2CharsToInt(message[3],message[4]));
			break;
    case 'A':
      switchAlarm(message[1] == '1');
      break;
	}
}

void updateTemperature(uint16_t n1,uint16_t n2){
    String aux = String("Temp: ")+String(n1)+String("/")+String(n2);
    writeLCD(aux,0);
}

void switchAlarm(boolean state){
    alarmState = state;
}

void updateAlarmState(){
    //String aux = String("I: ")+String(alarmIter)+String(" B: ")+String(alarmState);
    //writeLCD(aux,0);
    if(alarmState){
      alarmIter = alarmIter +1;
      if(alarmIter >= 5){
          lastAlarmIterState = !lastAlarmIterState;
          alarmIter = 0;
      }
      digitalWrite(ALARM_PIN, lastAlarmIterState? HIGH: LOW);
    } else {
        digitalWrite(ALARM_PIN, LOW);
        lastAlarmIterState = true;
        alarmIter = 0;
    }
}

uint16_t parse2CharsToInt(char c1, char c2){
  return c1<<8 | c2;
}


#include <LiquidCrystal.h>

// The notes frecuency, R 0 means none
#define  c3    7634
#define  d3    6803
#define  e3    6061
#define  f3    5714
#define  g3    5102
#define  a3    4545
#define  b3    4049
#define  c4    3816    // 261 Hz 
#define  d4    3401    // 294 Hz 
#define  e4    3030    // 329 Hz 
#define  f4    2865    // 349 Hz 
#define  g4    2551    // 392 Hz 
#define  a4    2272    // 440 Hz 
#define  a4s   2146
#define  b4    2028    // 493 Hz 
#define  c5    1912    // 523 Hz
#define  d5    1706
#define  d5s   1608
#define  e5    1517    // 659 Hz
#define  f5    1433    // 698 Hz
#define  g5    1276
#define  a5    1136
#define  a5s   1073
#define  b5    1012
#define  c6    955
#define  R     0


// Music
// Melody 1: Star Wars Imperial March
int melody1[] = {  a4, R,  a4, R,  a4, R,  f4, R, c5, R,  a4, R,  f4, R, c5, R, a4, R,  e5, R,  e5, R,  e5, R,  f5, R, c5, R,  g5, R,  f5, R,  c5, R, a4, R};
int beats1[]  = {  50, 20, 50, 20, 50, 20, 40, 5, 20, 5,  60, 10, 40, 5, 20, 5, 60, 80, 50, 20, 50, 20, 50, 20, 40, 5, 20, 5,  60, 10, 40, 5,  20, 5, 60, 40};
// Melody 2: Star Wars Theme
int melody2[] = {  f4,  f4, f4,  a4s,   f5,  d5s,  d5,  c5, a5s, f5, d5s,  d5,  c5, a5s, f5, d5s, d5, d5s,   c5};
int beats2[]  = {  21,  21, 21,  128,  128,   21,  21,  21, 128, 64,  21,  21,  21, 128, 64,  21, 21,  21, 128 }; 
int MELODY_SIZE = sizeof(melody1) / 2; // Melody length, for looping.
long tempo = 10000;
int pause = 1000;
int rest_count = 50; // Mini h4ck
// Global music vars
int toneM = 0;
int beat = 0;
long duration  = 0;
int potVal = 0;
int i = 0; //Current position

//LCD variables
const int rs = 8, en = 9, dd4 = 4, dd5 = 5, dd6 = 6, dd7 = 7;
LiquidCrystal lcd(rs, en, dd4, dd5, dd6, dd7);
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
      updateTemperature(message);
			break;
    case 'A':
      switchAlarm(message[1] == '1');
      break;
	}
}

void updateTemperature(String message){
    String n1 = getValue(message,':',1);
    String n2 = getValue(message,':',2);
    String aux = "Temp: " + n1 + "/" + n2;
    writeLCD(aux,0);
}

void switchAlarm(boolean state){
    alarmState = state;
}

void updateAlarmState(){
    //String aux = String("I: ")+String(alarmIter)+String(" B: ")+String(alarmState);
    //writeLCD(aux,0);
    if(alarmState){
      playTone();
    } else {
        i = 0;
    }
}

uint16_t parse2CharsToInt(char c1, char c2){
  return c1<<8 | c2;
}

// https://stackoverflow.com/questions/9072320/split-string-into-string-array
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void playTone() {
  toneM = melody1[i];
  beat = beats1[i];
  duration = beat * tempo; // Set up timing
	
  long elapsed_time = 0;
  if (toneM > 0) { // if this isn't a Rest beat, while the tone has 
    //  played less long than 'duration', pulse speaker HIGH and LOW
    while (elapsed_time < duration) {
 
      digitalWrite(ALARM_PIN,HIGH);
      delayMicroseconds(toneM / 2);
 
      // DOWN
      digitalWrite(ALARM_PIN, LOW);
      delayMicroseconds(toneM / 2);
 
      // Keep track of how long we pulsed
      elapsed_time += (toneM);
    } 
  }
  else { // Rest beat; loop times delay
    for (int j = 0; j < rest_count; j++) { // See NOTE on rest_count
      delayMicroseconds(duration);  
    }                                
  }
  delayMicroseconds(pause);
  i++;
  if(i == MELODY_SIZE) i = 0;
}

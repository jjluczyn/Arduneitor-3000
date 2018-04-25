#include <Key.h>
#include <Keypad.h>
#include <Keyboard.h>
#include <LiquidCrystal.h>

//Keyboard variables
const byte rows = 4; //four rows
const byte cols = 4; //three columns
char keys[rows][cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[rows] = {31, 33, 35, 37}; //connect to the row pinouts of the keypad
byte colPins[cols] = {39, 41, 43, 45}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

KeypadEvent lastPressed;
boolean readPressed = true;


//LCD variables
const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
char BLANKLINE[16] = "               ";

//Communicartion variables
char almacen[16];
boolean connectionState = false;

//BL variables
int processState;
    // 0-Initialization;
    // 1-Communication;
int objTemperature;
float auxTemp;

void setup() {
    processState = 0;

    objTemperature = 0;

    lcd.begin(16, 2);

    Serial1.begin(9600);
    delay(50);
    initTermometer();
}

void loop() {
    switch(processState){
        case 0:
            writeLCD("Temp. objetivo:",0);
            char charRead;
            charRead= 'X';
            while(charRead!='A'){
                charRead = 'X';
                while(charRead=='X') {
                    charRead = keypad.getKey();
                    
                }
                switch(charRead){
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        int num;
                        num = charRead - '0';
                        objTemperature = (objTemperature*10) + num;
                        break;
                    case 'B':
                        objTemperature = objTemperature/10;
                        break;
                }
                writeLCD(String(objTemperature)+String(" C"),1);
                delay(200); //Para que de tiempo a retirar el dedo.
            }
            processState = 1;
            initTermometer();
            break;
        case 1:
            int currTemperature;
            currTemperature = getTemperature();

            writeLCD(String("T. act. ")+String(currTemperature),0);
            writeLCD(String("T. obj. ")+String(objTemperature),1);


            int sendTempCur = currTemperature;
            int sendTempObj = objTemperature;

            String s1 = String(sendTempCur);
            String s2 = String(sendTempObj);
            Serial1.write('T');
            Serial1.write(':');
            for(int i=0; i<s1.length(); i++){
              Serial1.write(s1.charAt(i));
            }
            Serial1.write(':');
            for(int i=0; i<s1.length(); i++){
              Serial1.write(s2.charAt(i));
            }
            for(int i = 0; i < 16 - 3 - s1.length() - s2.length(); i++){
             Serial1.write(' '); 
            }
            if (currTemperature>=objTemperature){
                Serial1.write("A1----/----/----");
            }
            break;
    }
    delay(400);
}

void initTermometer(){
  float aux = getOhms();
  float temp = aux*3.3;
  auxTemp = 24 - temp;
}

int getTemperature(){
  float aux = getOhms();
    int celc = aux*3.3+auxTemp;
    return celc;
}

void writeLCD(String message, int line){
    lcd.setCursor(0,line);
    lcd.print(BLANKLINE);
    lcd.setCursor(0,line);
    lcd.print(message);
}

void keypadEvent(KeypadEvent key) {
  switch (keypad.getState()) {
    case PRESSED:
      lastPressed = key;
      readPressed = true;
      break;
  }
}

float getOhms(){
  int raw= 0;
  int Vin= 5;
  float Vout= 0;
  float r1= 100;
  float r2= 0;
  float buffer= 0;
  while(raw==0){
    raw= analogRead(0);
  }
  buffer= raw * Vin;
  Vout= (buffer)/1024.0;
  buffer= (Vin/Vout) -1;
  r2= r1 * buffer;
  return r2;
}


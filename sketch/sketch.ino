#include <LiquidCrystal.h>
#include <Servo.h>
#include <Keypad.h>

//*******************Configuracion pantallas LCD**********************
LiquidCrystal pantalla(8,9,4,5,6,7);
LiquidCrystal pantallaLuces(51,47,39,41,43,45);
//*******************************************************************

//*******************Configuracion teclado**********************
const byte columnas = 3, filas = 4;
char teclas[filas][columnas] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
char enie = 0xEE;
byte pinesFila[filas] = {28, 30, 32, 34};
byte pinesColumna[columnas] = {22, 24, 26};
Keypad teclado = Keypad(makeKeymap(teclas), pinesFila, pinesColumna, filas, columnas);
//**************************************************************

//*******************Configuracion ultrasonico**********************
int pinUltraIn = 10;
int pinUltraOut = 3;
int cmIn = 0;
int cmOut = 0;
//******************************************************************

//*******************Configuracion servomotor***********************
int pinServoIn = 12;
int pinServoOut = 13;
Servo servoIn;
Servo servoOut;
//******************************************************************

String passwd = "";
int contadorIntentos = 0;
//verificador de estadia
boolean ingreso = false;

void setup() {
  Serial1.begin(9600);
  pantalla.begin(16, 2);
  pantallaLuces.begin(20,4);
  pinMode(11, OUTPUT);
  servoIn.attach(pinServoIn);
  servoIn.write(0);
  servoOut.attach(pinServoOut);
  servoOut.write(0);
}

void loop() {
  
  if(!ingreso){
    //Puerta entrada
    cmIn = 0.01723 * leerUltrasonico(pinUltraIn, pinUltraIn);
    if(cmIn <= 150){
      validacionContra();
    }else{
      mensajeBienvenida();
    }
  }else{
    //control de luces
    controlLuces();

    //puerta de salida
    cmOut = 0.01723 * leerUltrasonico(pinUltraOut, pinUltraOut);
    if(cmOut <= 150){ 
      movimientoServo(servoOut); 
      ingreso = false;
      pantallaLuces.clear();
    }
  }
}

void mensajeBienvenida(){
  pantalla.print("CASA INTELIGENTE");
  pantalla.setCursor(0, 1);
  pantalla.print("ACE1");
  delay(1500);  
  pantalla.clear();
  pantalla.print("ACE1-N-G10-S1");
  delay(1500);  
  pantalla.clear();
}

void controlLuces(){
  int cuartos[4] = {analogRead(A0), analogRead(A1), analogRead(A2), analogRead(A3)};
  pantallaLuces.clear();
  for(int i = 0; i <4; i++){
    pantallaLuces.setCursor(0,i);
    pantallaLuces.print("Habitacion ");
    pantallaLuces.print(i+1,DEC);
    pantallaLuces.print(": ");
    if(cuartos[i] == 10){ 
      pantallaLuces.print("OFF"); 
    }
    else{ 
      pantallaLuces.print("ON"); 
    }
  }
  delay(1000);
}

void validacionContra(){
  pantalla.clear();
  pantalla.print("Ingrese su");
  pantalla.setCursor(0, 1);
  pantalla.print("constrase");
  pantalla.print(enie);
  pantalla.print("a");

  passwd = "";

  leerPasswd();

  if(passwd == "202110"){
    contadorIntentos = 0;
    pantalla.clear();
    pantalla.print("Bienvenido a");
    pantalla.setCursor(0, 1);
    pantalla.print("casa ^_^");
    movimientoServo(servoIn);
    ingreso = true;
    delay(1000);
    pantalla.clear();
  }
  else{
    contadorIntentos += 1;
    if(contadorIntentos == 3){
      contadorIntentos = 0;
      pantalla.clear();
      pantalla.print("Acceso no");
      pantalla.setCursor(0, 1);
      pantalla.print("autorizado");
      unsigned long inicioAlarma = millis();
      while(millis() < inicioAlarma + 20000){
        alarma();
      }
    }
    else{
      pantalla.clear();
      pantalla.print("Error en");
      pantalla.setCursor(0, 1);
      pantalla.print("constrase");
      pantalla.print(enie);
      pantalla.print("a");
      delay(1000);
      pantalla.clear();
    }
    validacionContra();
  }  
}

void movimientoServo(Servo servo){
  for(int i = 0; i < 90; i++){
    servo.write(i);
    delay(15);
  }
  delay(3000);
  for(int i = 90; i >= 0; i--){
    servo.write(i);
    delay(15);
  }  
}

void leerPasswd(){
  while(true){
    char tecla = teclado.getKey();
    if(tecla != NO_KEY){ 
      if(tecla != '*'){ 
        passwd += tecla; 
      } 
    }
    if(tecla == '*'){ 
      break; 
    }
  }
}

void alarma(){
  digitalWrite(11, HIGH);
  delay(1000);
  digitalWrite(11, LOW);
  delay(1000);
}

long leerUltrasonico(int triggerPin, int echoPin){
  pinMode(triggerPin, OUTPUT);
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  pinMode(echoPin, INPUT);
  return pulseIn(echoPin, HIGH);
}

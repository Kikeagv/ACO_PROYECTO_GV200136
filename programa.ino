#include <Wire.h>
#include <Adafruit_LiquidCrystal.h>
#include <Servo.h>

// Definición de pines para los sensores y actuadores
const int pinPIR = 2;
const int pinTemp = A0;
const int pinGas = A1;
const int pinServo = 5;
const int pinBuzzer = 6;
const int pinReleFoco = 4;
const int pinReleAire = 7;

// Umbrales para activar los actuadores
const int umbralTemp = 25;  // Temperatura en grados Celsius para activar el aire acondicionado
const int umbralGas = 300;  // Umbral para calidad del aire mala

// Crear objeto de pantalla I2C
Adafruit_LiquidCrystal lcd(0x27);

// Crear objeto Servo
Servo servoMotor;

void setup() {
  Serial.begin(9600);
  pinMode(pinPIR, INPUT);
  servoMotor.attach(pinServo);
  pinMode(pinBuzzer, OUTPUT);
  pinMode(pinReleFoco, OUTPUT);
  pinMode(pinReleAire, OUTPUT);
  
  lcd.begin(16, 2);
  lcd.setBacklight(LOW);
  lcd.clear();
  lcd.print("Sistema Listo");
  delay(2000);
  lcd.clear();
  lcd.print("Estado normal");
}

void loop() {
  static bool isNormal = true;
  bool estadoCambiado = false;
  int valorPIR = digitalRead(pinPIR);
  int valorTempRaw = analogRead(pinTemp);
  float temperaturaC = (valorTempRaw * (5.0 / 1023.0) - 0.5) * 100.0;
  int valorGas = analogRead(pinGas);

  // Control del servo por calidad del aire
  if (valorGas > umbralGas) {
    servoMotor.write(90);
    if (isNormal || !estadoCambiado) {
      lcd.clear();
      lcd.print("Aire Malo: Servo");
      estadoCambiado = true;
      isNormal = false;
    }
  } else {
    servoMotor.write(0);
  }

  // Control del aire acondicionado por temperatura
  if (temperaturaC > umbralTemp) {
    digitalWrite(pinReleAire, HIGH);
    if (isNormal || !estadoCambiado) {
      lcd.clear();
      lcd.print("Temp Alta: Aire ON");
      estadoCambiado = true;
      isNormal = false;
    }
  } else {
    digitalWrite(pinReleAire, LOW);
  }

  // Control del foco y buzzer por movimiento
  if (valorPIR) {
    digitalWrite(pinReleFoco, HIGH);
    digitalWrite(pinBuzzer, HIGH);
    if (isNormal || !estadoCambiado) {
      lcd.clear();
      lcd.print("Mov Detectado");
      estadoCambiado = true;
      isNormal = false;
    }
    delay(1000);  // Mantener el buzzer encendido por 1 segundo
    digitalWrite(pinBuzzer, LOW);
  } else {
    digitalWrite(pinReleFoco, LOW);
  }

  // Revertir a estado normal si no hay alertas
  if (!estadoCambiado && !isNormal) {
    lcd.clear();
    lcd.print("Estado normal");
    isNormal = true;
  }

  delay(1000);  // Espera para la próxima lectura, reduce o elimina según necesidades de respuesta
}

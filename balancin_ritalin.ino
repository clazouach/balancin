#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

Adafruit_MPU6050 mpu;

//giroscopia
//ScL A5
//sda  A4
//vcc
//Gnd


// Pines de control del motor izquierdo
int motorLeft1 = 7;
int motorLeft2 = 6;
int motorEnableLeft = 8; // Pin ENA para controlar la velocidad del motor izquierdo

// Pines de control del motor derecho
int motorRight1 = 4;
int motorRight2 = 5;
int motorEnableRight = 3; // Pin ENB para controlar la velocidad del motor derecho

// Variables para el control del equilibrio
float angle;
float setpoint = -0.6; // Ángulo objetivo para el equilibrio
float error;
float lastError = 0;
float Kp =12; // Ganancia proporcional 12
float Kd = 0.07; // Ganancia derivativa  1

void setup() {
  Wire.begin();
  Serial.begin(9600);

  if (!mpu.begin()) {
    Serial.println("No se pudo encontrar un sensor MPU6050. ¡Verifica las conexiones!");
    while (1);
  }

  pinMode(motorLeft1, OUTPUT);
  pinMode(motorLeft2, OUTPUT);
  pinMode(motorEnableLeft, OUTPUT);

  pinMode(motorRight1, OUTPUT);
  pinMode(motorRight2, OUTPUT);
  pinMode(motorEnableRight, OUTPUT);
}

void loop() {
  sensors_event_t a, g, temp;

  mpu.getEvent(&a, &g, &temp);

  // Calcular el ángulo a partir de los datos del sensor
  //angle = atan2(-a.acceleration.y, a.acceleration.z) * 180 / PI;
  angle = atan2(-a.acceleration.y, 10) * 180 / PI;

  // Calcular el error
  error = setpoint - angle;

  // Calcular la velocidad de los motores en función del error
  int motorSpeedLeft = Kp * error + Kd * (error - lastError);

  int motorSpeedRight = Kp * error + Kd * (error - lastError);


  // Limitar las velocidades máximas
  motorSpeedLeft = constrain(motorSpeedLeft, -250, 250);
  motorSpeedRight = constrain(motorSpeedRight, -250, 250);
 Serial.println(motorSpeedLeft);
 Serial.println(motorSpeedRight);

  // Control de dirección de los motores
  if (motorSpeedLeft > 0) {
    digitalWrite(motorLeft1, HIGH);
    digitalWrite(motorLeft2, LOW);
  } else {
    digitalWrite(motorLeft1, LOW);
    digitalWrite(motorLeft2, HIGH);
  }

  if (motorSpeedRight > 0) {
    digitalWrite(motorRight1, HIGH);
    digitalWrite(motorRight2, LOW);
  } else {
    digitalWrite(motorRight1, LOW);
    digitalWrite(motorRight2, HIGH);
  }

  // Control de velocidad de los motores
  motorSpeedLeft = abs(motorSpeedLeft);
  motorSpeedRight = abs(motorSpeedRight);

  analogWrite(motorEnableLeft, motorSpeedLeft); // Control de velocidad izquierda
  analogWrite(motorEnableRight, motorSpeedRight); // Control de velocidad derecha

  // Guardar el error actual como el error anterior para el próximo ciclo
  lastError = error;

  // Imprimir el ángulo y el error
  Serial.print("Ángulo: ");
  Serial.print(angle);
  Serial.print(" Error: ");
  Serial.println(error);

  delay(10); // Pequeña pausa para estabilidad
}



// #include <SoftwareSerial.h>
// #include <Arduino.h>
#include <Servo.h>

#define RECEPTOR_CH3_PIN A1 // Canal 3 de velocidad
#define RECEPTOR_CH6_PIN A2 // Canal 6 de Auto/Manual
int digIN1 = 9;
int digIN2 = 10;


const unsigned long TIEMPO_PASO_TEST = 1000;

int direccion = 0;
int direccion_ahora = 0;

long baud = 9600;

// Voltaje

volatile int test = 0;
float iteracion_paso_test = 0;


int vel_real = 0;
int vAhora_real = 0;

float voltage_final = 0;
volatile int velocidad = 0;
int vAhora = 995;
int velmin = 0;
int incremento = 10;
int voltage_max = 6;


int iteraciones_escritura = 50;
int escribe = 0;

const int lecturas = 5;   // Total lecturas V
int index = 0;            // El indice de la lectura actual
float readings[lecturas]; // Lecturas de la entrada analogica
float Vtotal = 0.0;       // Total
float Vmedia = 0.0;       // Promedio

unsigned long timeout_pwm_micros = 50000;

volatile unsigned long siguientePaso = 0; // millis en los que tiene que saltar al siguiente paso

void test_funcion();

#define PIN_VOLTAJE A0
double VREF = 1.09; // valor de referencia interno del Atmega 328p, 1.1V teóricos
double R1 = 330000;  // 1M
double R2 = 6800;    // 100K

void setup()
{
  analogReference(INTERNAL);
  Serial.begin(baud);

  pinMode(digIN1, OUTPUT);

  pinMode(RECEPTOR_CH3_PIN, INPUT);
  pinMode(RECEPTOR_CH6_PIN, INPUT);

  Serial.print("Voltaje Máximo: ");
  Serial.print(voltajeMaximo());
  Serial.println("V");
  Serial.println("");




}


float voltajeMaximo()
{
  return VREF / (R2 / (R1 + R2));
}

float leerVoltaje()
{
  int valor = analogRead(PIN_VOLTAJE);
  //Serial.println(valor);
  double v = (valor * VREF) / 1024.0;
  return (v / (R2 / (R1 + R2)));
}



bool switchActivado()
{
  unsigned long value1 = pulseIn(RECEPTOR_CH6_PIN, HIGH, timeout_pwm_micros);
  unsigned long value2 = pulseIn(RECEPTOR_CH6_PIN, HIGH, timeout_pwm_micros);
  if ((value1 > 1600) && (value2 > 1600))
  {
    return false;
  }
  return true;
}

void printSerial()
{
  escribe++;
  if (escribe == iteraciones_escritura)
  {
    escribe = 0;


    vel_real = map(velocidad, 995, 1989, 0, 100 );
    vAhora_real = map(vAhora, 995, 1989, 0, 100 );

    Serial.print("Limitador activado:");
    Serial.println(switchActivado());
    Serial.print("V: ");
    Serial.println(voltage_final);

    Serial.print("Vel: ");

    Serial.print(velocidad);
    Serial.print("  >> ");

    Serial.println(vel_real);
    Serial.print("vAhora: ");

    Serial.print(vAhora);

    Serial.print("  >> ");
    Serial.println(vAhora_real);
  }
}

void loop()
{
  voltage_final = leerVoltaje();
  velocidad = pulseIn(RECEPTOR_CH3_PIN, HIGH);
  if (voltage_final < voltage_max)
  {
    vAhora = vAhora - incremento;
    if (vAhora < 995) {
      vAhora = 995;
    }
  }
  else if (vAhora < velocidad)
  {
    vAhora = vAhora + incremento;


  }
  else
  {
    vAhora = velocidad;
  }

//vAhora = velocidad;

  if (switchActivado())
  {
    direccion = 0;

  }
  else
  {
    direccion = 1;

  }

  if (direccion!=direccion_ahora && vAhora < 1000) {
    direccion_ahora = direccion;
  }


  if (direccion_ahora == 0) {
    vAhora_real = map(vAhora, 995, 1989, 255, 0 );
    // ESC.writeMicroseconds(vAhora_real);
    digitalWrite(digIN1, HIGH);
    
    if (vAhora_real>15){
    analogWrite(digIN2, vAhora_real);
    }else{
      analogWrite(digIN2, 0);
    }
  }
  else
  {
    vAhora_real = map(vAhora, 995, 1989, 0, 255 );
    digitalWrite(digIN1, LOW);

  if (vAhora_real<240){
    analogWrite(digIN2, vAhora_real);
    }else{
      analogWrite(digIN2, 255);
    }

  }


  printSerial();
}

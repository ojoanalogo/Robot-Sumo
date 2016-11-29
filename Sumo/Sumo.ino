#include <Arduino.h>

/*
  @Proyecto:    Robot Sumo
  @Autor:       Alfonso Reyes Cortés
  @Placa:       Arduino UNO R3
  @Descripción: Código para robot sumo, ahora en una versión mejorada y sin bugs
*/

/*
* Definición de variables
*/
int pinSharpD         = A0;  // Sharp delantero
int pinSharpT         = A1;  // Sharp trasero
int qrdA              = A2;  // QRD Frontal
int qrdB              = A3;  // QRD Trasero
int pinLed            = 13;  // Led indicador encendido
int pinActivacion     = 6;   // Pin de activación por LDR
int pinMotorD1        = 10;  // Pin Motor D1
int pinMotorD2        = 9;   // Pin Motor D2
int pinMotorI1        = 7;   // Pin Motor I1
int pinMotorI2        = 8;   // Pin Motor I2

int valNegro          = 20; // Valor maximo del color negro

/*
*  Condicionales
*/
boolean debug    = false;      // Indica el modo pruebas del código
boolean activado = false;     // Indica si el sumo ha sido activado para competir

/*
* setup(#)
* - Se dispara en cuanto la placa es encendida, aquí configuramos que pines usaremos y como lo haremos
*/

void setup() {
  /*
  * pinMode le dice a la placa como usaremos un pin, requiere 2 parametros siendo el primero de tipo INT y el segundo un ENUM
  * ENUM = OUTPUT (Salida) | INPUT (Entrada)
  *
  */
  pinMode(pinLed, OUTPUT);
  pinMode(pinActivacion, INPUT);
  pinMode(pinMotorD1, OUTPUT);
  pinMode(pinMotorD2, OUTPUT);
  pinMode(pinMotorI1, OUTPUT);
  pinMode(pinMotorI2, OUTPUT);
  // Inicia la comunicación por puerto serial a 9600 baudios (default, es un estandar desde los años 90 para comunicación via serial)
  Serial.begin(9600);
  Serial.println("Robot Sumo iniciando..."); // Que inicie la acción, oh ci
}

/*
*  loop(#)
* - Es el metodo que siempre esta en ejecución
*/
void loop() {
  if(!activado) {
    activado = true;
    delay(1200);
    atras(0.3);
  }
  if (debug) {
    logicaDebug();
  }
  else {
    logicaSumo();
  }
  delay(20); 
}

/**
 * logicaSumo(#)
 * - Maneja la lógica principal del robot y decisiones
 */
void logicaSumo() {
  int vSharpD = obtenerDistancia(pinSharpD);
  int vSharpT = obtenerDistancia(pinSharpT);
  if (!puedeAvanzar()) {  // Fuga a todo gas xdxdxd
    if (leerAnalogo(qrdA) >= valNegro) {
      Serial.println("QRD FRONTAL, FUGA ATRAS");
      atras(0.5);
      return;
    }
  } else {
  if ((vSharpD >= 40) || (vSharpT >= 40)) {
    giroIzquierda(0.01);
  } else {
      adelante(1);
  }
}
}

/**
 * logicaDebug(#)
 * - Lógica de pruebas del robot sumo para analizar valores de los sensores
 */
void logicaDebug() {
  int vSharpD = obtenerDistancia(pinSharpD);
  int vSharpT = obtenerDistancia(pinSharpT);
  int vQrdA = analogRead(qrdA);
  //int vQrdB = analogRead(qrdB);
  Serial.print("\n");
  Serial.println("============ MODO DEBUG ============");
  Serial.println("Esta activado? => " + estaActivado());
  Serial.println("Hay un objeto al frente? => " + hayObjeto());
  Serial.println("Se encuentra en area segura? => " + areaSegura());
  Serial.println("Valor Sharp Frontal IR => " + (String)vSharpD);
  Serial.println("Valor Sharp Frontal 2 IR => " + (String)vSharpT);
  Serial.print("qrdA => " + (String)vQrdA + " | ");
  //Serial.print("qrdB => " + (String)vQrdB);
  for (int i = 0; i<10; i++) {
    Serial.print("\n");
  }
  delay(3000);
}

String estaActivado() {
  if (activado) {
    return "SI";
  }
  return "NO";
}

String areaSegura() {
  if (puedeAvanzar()) {
    return "SI";
  }
  return "NO";
}

String hayObjeto() {
  int vSharpD = obtenerDistancia(pinSharpD);
  int vSharpT = obtenerDistancia(pinSharpT);
  if (vSharpT <= 40) {
    return "SI";
  }
  if (vSharpD <= 40) {
    return "SI";
  }
  return "NO";
}

/*
*  puedeAvanzar(#)
*  - Indica si el robot debe avanzar o no
* @returns False o True - Dependiendo de la lectura del sensor QRD
*/
boolean puedeAvanzar() {
  if (leerAnalogo(qrdA) >= valNegro) {
    return false;
  }
  return true;
}

/*
*  giroDerecha(double)
* Acciona un giro a la derecha sobre su propio eje
* @param tiempo - El tiempo a hacer la acción
*/
void giroDerecha(double tiempo) {
  digitalWrite(pinMotorI1, LOW);
  digitalWrite(pinMotorI2, HIGH);
  digitalWrite(pinMotorD1, HIGH);
  log("Movimiento - giroDerecha");
  delay(tiempo * 1000);
  parar();
}

/*
*  giroIzquierda(double)
* Acciona un giro a la izquierda sobre su propio eje
* @param tiempo - El tiempo a hacer la acción
*/
void giroIzquierda(double tiempo) {
  digitalWrite(pinMotorD1, LOW);
  digitalWrite(pinMotorD2, HIGH);
  digitalWrite(pinMotorI1, HIGH);
  digitalWrite(pinMotorI2, LOW);

    log("Movimiento - giroIzquierda");
  delay(tiempo * 1000);
  parar();
}

/*
* adelante(double)
* Mueve los motores en sentido a las agujas del reloj
* @param tiempo - El tiempo a hacer la acción
*/
void adelante(double tiempo){
  digitalWrite(pinMotorD1, HIGH);
  digitalWrite(pinMotorI1, HIGH);
  digitalWrite(pinMotorD2, LOW);
  digitalWrite(pinMotorI2, LOW);
  log("Movimiento - Adelante");
  delay(tiempo * 1000);
  parar();
}

/*
*  atras(double)
* Mueve los motores en sentido contrario a las agujas del reloj
* @param tiempo - El tiempo a hacer la acción
*/
void atras(double tiempo){
  digitalWrite(pinMotorD1, LOW);
  digitalWrite(pinMotorD2, HIGH);
  digitalWrite(pinMotorI1, LOW);
  digitalWrite(pinMotorI2, HIGH);
  log("Movimiento - Atras");
  delay(tiempo * 1000);
  parar();
}
/*
* parar(#)
* Cancela el pulso enviado a los motores para parar el robot
*/
void parar() {
  digitalWrite(pinMotorD2, LOW);
  digitalWrite(pinMotorI2, LOW);
  digitalWrite(pinMotorI1, LOW);
  digitalWrite(pinMotorD1, LOW);
  log("Movimiento - Parar");
}


/*
* obtenerDistancia(int)
* Obtiene la distancia en centimetros de lo que detecta el sensor SHARP
* @param pin - El pin a consultar
* @returns distancia(double) - Distancia en centimetros
*/
double obtenerDistancia(int pin) {
  int valorAnalogo = leerAnalogo(pin);
  if (valorAnalogo < 16)  valorAnalogo = 16;
  return 2076.0 / (valorAnalogo - 10);
}

/*
* leerAnalogo(int)
* Envia a la consola serial un texto
*/
int leerAnalogo(int pin) {
  return analogRead(pin);
}

/*
* log(String)
* Envia a la consola serial un texto
*/
void log(String palabra) {
    Serial.println(palabra);
}

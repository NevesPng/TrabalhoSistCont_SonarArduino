#include <Servo.h>

Servo servo;

const int SERVO_PIN      = 9;
const int BTN_ESQUERDA   = 2;
const int BTN_DIREITA    = 3;
const int TRIG_PIN       = 10;
const int ECHO_PIN       = 11;

const int PASSO              = 2;
const int DELAY_AUTOMATICO   = 20;
const int DELAY_MS           = 10;
const int DISTANCIA_ALERTA   = 30;
const unsigned long TIMEOUT_MANUAL = 5000;

int  angulo        = 0;
int  direcao       = 1;
bool modoManual    = false;
unsigned long ultimoBotao = 0;

float medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracao = pulseIn(ECHO_PIN, HIGH, 30000);
  if (duracao == 0) return -1;
  return duracao * 0.0343 / 2.0;
}

void setup() {
  Serial.begin(9600);
  servo.attach(SERVO_PIN);
  servo.write(angulo);

  pinMode(BTN_ESQUERDA, INPUT);
  pinMode(BTN_DIREITA,  INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  Serial.println("Sistema iniciado. Modo: AUTOMATICO");
}

void loop() {
  bool btnEsq = digitalRead(BTN_ESQUERDA) == HIGH;
  bool btnDir = digitalRead(BTN_DIREITA)  == HIGH;

  if (btnEsq || btnDir) {
    if (!modoManual) {
      modoManual = true;
      Serial.println("[MANUAL]");
    }
    ultimoBotao = millis();

    if (btnEsq) angulo = max(0,   angulo - PASSO);
    if (btnDir) angulo = min(180, angulo + PASSO);
    servo.write(angulo);
    delay(DELAY_MS);
  }

  if (modoManual && (millis() - ultimoBotao >= TIMEOUT_MANUAL)) {
    modoManual = false;
    Serial.println("[AUTOMATICO]");
  }

  if (!modoManual) {
    angulo += direcao * PASSO;

    if (angulo >= 180) { angulo = 180; direcao = -1; }
    if (angulo <= 0)   { angulo = 0;   direcao =  1; }

    servo.write(angulo);
    delay(DELAY_AUTOMATICO);
  }

  float distancia = medirDistancia();

  if (distancia > 0 && distancia <= DISTANCIA_ALERTA) {
    Serial.print(">>> OBJETO | ");
  } else {
    Serial.print("Livre | ");
  }
  Serial.print(angulo);
  Serial.print("deg | ");
  if (distancia > 0) { Serial.print(distancia, 1); Serial.println(" cm"); }
  else                { Serial.println("fora de alcance"); }
}

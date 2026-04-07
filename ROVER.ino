#include <Servo.h>

Servo servo;

const int SERVO_PIN      = 9;
const int BTN_ESQUERDA   = 2;
const int BTN_DIREITA    = 3;
const int TRIG_PIN       = 10;
const int ECHO_PIN       = 11;

const int PASSO              = 2;
const int DELAY_AUTOMATICO   = 20;   // ms entre cada passo no modo auto
const int DELAY_MS           = 10;   // ms entre leituras de botão
const int DISTANCIA_ALERTA   = 30;   // cm
const unsigned long TIMEOUT_MANUAL = 5000; // ms para voltar ao automático

int  angulo        = 0;
int  direcao       = 1;       // 1 = crescendo, -1 = decrescendo
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

  // --- Detecção de botão → entra/mantém modo manual ---
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

  // --- Timeout: volta ao automático após 5 s sem botão ---
  if (modoManual && (millis() - ultimoBotao >= TIMEOUT_MANUAL)) {
    modoManual = false;
    Serial.println("[AUTOMATICO]");
  }

  // --- Varredura automática ---
  if (!modoManual) {
    angulo += direcao * PASSO;

    if (angulo >= 180) { angulo = 180; direcao = -1; }
    if (angulo <= 0)   { angulo = 0;   direcao =  1; }

    servo.write(angulo);
    delay(DELAY_AUTOMATICO);
  }

  // --- Sensor ultrassônico ---
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
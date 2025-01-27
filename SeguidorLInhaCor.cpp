// Definição dos pinos de controle do motor
#define M1 6
#define M2 9
#define dir1 5
#define dir2 10

#define S0 2
#define S1 3
#define S2 11
#define S3 12
#define sensorOut 13

// Definição dos pinos dos sensores de linha
#define pin_S1 A1
#define pin_S2 A2
#define pin_S3 A3

// Definindo os pinos do Trig e Echo
const int trigPin = 7;
const int echoPin = 8;

// Variável para armazenar o tempo de duração do pulso e a distância calculada
long duration;
int distance;

// Variáveis de controle
int velocidade = 100;
bool podeMover = false;
unsigned long lastColorCheck = 0;
const unsigned long colorCheckInterval = 100; // Intervalo para ler o sensor de cor

void setup(){

  pinMode(A0, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(dir1, OUTPUT);
  pinMode(dir2, OUTPUT);

  digitalWrite(dir1, LOW);
  digitalWrite(dir2, LOW);

  pinMode(pin_S1, INPUT);
  pinMode(pin_S2, INPUT);
  pinMode(pin_S3, INPUT);

  // Configurando os pinos Trig como saída e Echo como entrada
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.begin(9600);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(sensorOut, INPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);  // Definindo a escala do sensor de cor
}

void loop() {
  // Garantir que o Trig esteja baixo por pelo menos 2 microssegundos
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Enviar pulso de 10 microssegundos no pino Trig
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Ler o tempo que o pulso leva para voltar no pino Echo
  duration = pulseIn(echoPin, HIGH);
  
  // Calculando a distância (velocidade do som = 343 m/s ou 0.034 cm/µs)
  distance = duration * 0.034 / 2; // Dividido por 2 porque o pulso vai e volta

  int Sensor1 = analogRead(pin_S1);
  int Sensor2 = analogRead(pin_S2);
  int Sensor3 = analogRead(pin_S3);

  
  if (podeMover) {
    // Lógica do seguidor de linha
    if(distance<=15) {
      parar();
    }
    else if(((Sensor1 > 500) && (Sensor2 < 500) && (Sensor3 > 500)) || ((Sensor1 < 500) && (Sensor2 < 500) && (Sensor3 < 500))){
      frente();
    }
    else if(((Sensor1 < 500) && (Sensor2 < 500) && (Sensor3 > 500)) || ((Sensor1 < 500) && (Sensor2 > 500) && (Sensor3 > 500))) {
      direita();
    }
    else if(((Sensor1 > 500) && (Sensor2 < 500) && (Sensor3 < 500)) || ((Sensor1 > 500) && (Sensor2 > 500) && (Sensor3 < 500))) {
      esquerda();
    }
    else if((Sensor1 > 500) && (Sensor2 > 500) && (Sensor3 > 500)) {
      parar();
    }
  } else {
    parar();
  }

  // Verifica a cor a cada intervalo definido
  if (millis() - lastColorCheck >= colorCheckInterval) {
    lastColorCheck = millis();
    detectarCor();
  }

  delay(100);  // Pequeno atraso para evitar sobrecarga
}

void detectarCor() {
  int redValue, greenValue, blueValue;

  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  redValue = pulseIn(sensorOut, LOW);

  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  blueValue = pulseIn(sensorOut, LOW);

  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  greenValue = pulseIn(sensorOut, LOW);

  Serial.print("Red: ");
  Serial.print(redValue);
  Serial.print(" | Green: ");
  Serial.print(greenValue);
  Serial.print(" | Blue: ");
  Serial.println(blueValue);

  // Checa a cor detectada e altera o estado de movimento do carrinho
  if ((redValue >= 57 && redValue <= 136) && (greenValue >= 175 && greenValue <= 319) && (blueValue >= 139 && blueValue <= 267)) {
    Serial.println("Cor detectada: Vermelho");
    podeMover = false;
    digitalWrite(A5, LOW);
    digitalWrite(A0, LOW);
  } else if ((redValue >= 140 && redValue <= 240) && (greenValue >= 110 && greenValue <= 190) && (blueValue >= 150 && blueValue <= 240)) {
    Serial.println("Cor detectada: Verde");
    podeMover = true;
    digitalWrite(A5, HIGH);
    digitalWrite(A0, LOW);
  } else if ((redValue >= 35 && redValue <= 87) && (greenValue >= 38 && greenValue <= 105) && (blueValue >= 60 && blueValue <= 143)) {
    Serial.println("Cor detectada: Amarelo");
    digitalWrite(A0, HIGH);
  } else {
    Serial.println("Cor não identificada");
    digitalWrite(A0, LOW);
  }
}

void frente(){
  analogWrite(M1, velocidade);
  analogWrite(M2, velocidade);
}

void direita(){
  analogWrite(M1, velocidade);
  analogWrite(M2, 0);
}

void esquerda(){
  analogWrite(M1, 0);
  analogWrite(M2, velocidade);
}

void parar(){
  analogWrite(M1, 0);
  analogWrite(M2, 0);
}
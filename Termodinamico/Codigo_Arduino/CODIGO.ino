
///////////////////////////////////////////
#include <PID_v1.h>
#include <LiquidCrystal.h> // Biblioteca para o LSET
LiquidCrystal lcd(8, 9, 10, 11, 12, 13);



// Configurações dos pinos
#define loadR 6            // Pino que ativa o TRIAC
#define zeroCrossPin 2      // Pino do zero-crossing
#define tempPin A0          // Pino para leitura da temperatura (sensor LM35, por exemplo)

// Variáveis para o controle PID
double setpoint = 40.0;    // Temperatura desejada em graus Celsius
double input = 0.0;        // Temperatura atual
double output = 128.0;     // Potência calculada (PWM entre 20 e 237)

// Ajustes do PID (ajuste conforme necessário)
double Kp = 3.18, Ki = 0.56, Kd = 1.45;
PID myPID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

// Variável de controle para o TRIAC
bool triacEnabled = false; // Define se o TRIAC deve ser ativado

void setup() {
  Serial.begin(9600);
  pinMode(loadR, OUTPUT);
  pinMode(zeroCrossPin, INPUT);

  // Configura a interrupção para o zero-crossing
  attachInterrupt(digitalPinToInterrupt(zeroCrossPin), zero_cross_int, RISING);

  // Inicializa o PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(20, 255); // PWM entre 0 (mínimo) e 255 (máximo)

  lcd.begin(16, 2); // Configura o LCD como 16 colunas e 2 linhas
  lcd.print("Temp:");        // Exibe o título na primeira linha
  lcd.setCursor(0, 1);       // Move o cursor para a segunda linha
  lcd.print("Potencia:"); 


}

void loop() {
  // Leitura da temperatura do sensor
  int sensorValue = analogRead(tempPin);  // Lê o valor analógico
  float voltage = sensorValue * (5.0 / 1023.0);  // Converte para tensão
  float tempLida = voltage * 100.0;  // Converte a tensão para temperatura em Celsius

  // Atualiza o PID
  input = tempLida; // Atualiza a variável input do PID com a temperatura lida
  myPID.Compute();

  // Controle do TRIAC
  if (tempLida <= setpoint) {
    triacEnabled = true; // Permite ativar o TRIAC
  } else {
    output = 0;         // Define a potência no mínimo
    triacEnabled = false; // Não permite ativar o TRIAC
  }

  // Define loadR como LOW se o output for 0
  if (output == 0) {
    digitalWrite(loadR, LOW);
  }

  // Leitura de comandos pela serial
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n'); // Lê até nova linha
    

    if (command.startsWith("SET:")) {
      String value = command.substring(4); // Pega o valor após "SET:"
      double newSetpoint = value.toDouble();

      if (newSetpoint > 0 && newSetpoint <= 99) { // Limita valores válidos do setpoint
        setpoint = newSetpoint; // Atualiza o setpoint
        // Recalcula o PID imediatamente após alterar o setpoint
        myPID.Compute();
  }}}

  // Formatação e envio dos dados pela Serial
  char outputStr[3];
  // Formata o PWM (output) com 3 dígitos, incluindo zeros à esquerda
  sprintf(outputStr, "%03d", (int)output);

  // Log da temperatura e setpoint
  Serial.print(tempLida, 2);
  Serial.print("|");
  Serial.println(outputStr); 
  
   // Imprime a temperatura com 2 casas decimais
  
  int intensidade1 = (int)((output / 255.0) * 100);
  // Atualiza o LCD
  lcd.setCursor(6, 0); // Move o cursor para o final de "Temp:"
  lcd.print(tempLida); // Exibe a temperatura com 1 casa decimal
  lcd.print("C "); // Adiciona "C" para indicar graus Celsius

  lcd.setCursor(10, 1); // Move o cursor para o final de "Potencia:"
  lcd.print(intensidade1); // Exibe a potência (intensidade em %)
  lcd.print("%"); // Adiciona "%" para indicar porcentagem



 delay(200);// Aguarda 100 ms antes da próxima leitura
}

// Função de interrupção para controle do TRIAC
void zero_cross_int() {
  if (triacEnabled && output > 0) { // Só ativa o TRIAC se permitido e potência válida
    // Calcula o tempo de atraso para o ângulo de disparo
    int powertime = (260 - output) * 39; // 39 ~= (10000µs / 256)

    // Atraso para ajustar o ângulo de disparo
    delayMicroseconds(powertime);

    // Envia o pulso ao TRIAC
    digitalWrite(loadR, HIGH);
    delayMicroseconds(10); // Pulso curto para disparar o TRIAC
    digitalWrite(loadR, LOW);
  } else {
    digitalWrite(loadR, LOW); // Garante que o TRIAC esteja desligado
  }
}

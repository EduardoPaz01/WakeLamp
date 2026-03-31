// Variáveis de estado
static volatile int estadoLed = LOW;  
static volatile unsigned long ultimoTempo = 0; // para debounce
const unsigned long debounceDelay = 300;        // 100ms de debounce

void IRAM_ATTR botaoISR() {
  unsigned long agora = millis();

  // (debounce for software)
  if (agora - ultimoTempo > debounceDelay) {
    estadoLed = !estadoLed;   // inverte estado
    digitalWrite(25, estadoLed);
    
    digitalWrite(2, estadoLed);
    ultimoTempo = agora;
  }
}

void setup() {
  Serial.begin(9600);

  pinMode(34, INPUT_PULLUP);  
  pinMode(25, OUTPUT);
  pinMode(2, OUTPUT);

  digitalWrite(25, estadoLed);
  digitalWrite(2, estadoLed);

  // Configura interrupção para borda de subida ou descida
  attachInterrupt(digitalPinToInterrupt(34), botaoISR, FALLING);
}

void loop() {
  // O loop fica livre para outras tarefas
}

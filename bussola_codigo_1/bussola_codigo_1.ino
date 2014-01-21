//More infor at www.lusorobotica.com
#include <Wire.h> 
 
int compassAddress = 0x42 >> 1;  // no datasheet diz que o � o 0x42 
                                 // mas como a biblioteca wire s� utiliza 7 bits
                                 // fazemos um shift para ficar com os most significant bits
int reading = 0;
int led = 9;
int brightness = 0;

// The LED brightness goes between 0-255, so we need to map a range of 360 to a range of 255
float ratio = 0.7; // 256 / 360 = 0.708
 
void setup() 
{ 
  Wire.begin();                
  Serial.begin(9600);

  // set pin for led output
  pinMode(led, OUTPUT);
} 
  
void loop() 
{ 
  // begin a transmission to the compass
  Wire.beginTransmission(compassAddress);
  Wire.write('A');             // no datasheet diz k � o comando para pedir o angulo
  Wire.endTransmission();      // end transmission to transmit the 
 
  // tarefa 2: esperar que os dados sejam processados
  delay(10);                   // o datasheet sugere pelo menos 6000 microsegundos
  
  // tarefa 3: pedir ao compasso
  Wire.requestFrom(compassAddress, 2);    // pedir dois bytes ao compasso
  
  // tarefa 4: receber dados do compasso 
  if(2 <= Wire.available())    // se estiverem disponiveis os 2 bytes
  { 
    reading = Wire.read();  // recebe a parte alta apagando o que estava antes
    reading = reading << 8;    // shifta a parte alta para o seu lugar
    reading += Wire.read(); // recebe a parte baixa
    reading /= 10;           // tirar de comentario se pretender receber s� em graus em vez de decimas de graus
    Serial.print(reading);   // mostra o valor para a consola
    
    // This should be brightest at 360 degrees, and off at 0 degrees
    Serial.print(" : ");
    Serial.print(ratio);
    Serial.print(" : ");
    Serial.println(reading * ratio);
    analogWrite(led, reading * ratio);
    
 
  }
  delay(100);                  // espera antes de fazer um novo ciclo
}

#include <Wire.h>
#include <WiFi.h>
#include <ESP32_MailClient.h>
#include <MPU6050.h>

const char* rede = "YOUR_NETWORK_NAME";             
const char* password = "YOUR_PASSWORD";      
const char* emailSender = "YOUR_EMAIL";      
const char* emailRecipient = "RECIPIENT_EMAIL"; 
const char* smtpServer = "smtp.gmail.com";   
const int smtpPort = 587;                     
const char* emailPassword = "YOUR_EMAIL_PASSWORD"; 

MPU6050 mpu;
bool fallDetected = false;

void setup() {
  Serial.begin(115200);
  WiFi.begin(rede, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");

  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("Falha ao conectar ao MPU6050");
    while (1);
  }
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  

  if (az < -16000) { 
    if (!fallDetected) {
      fallDetected = true;
      Serial.println("Queda detectada!");
      sendEmail();
    }
  } else {
    fallDetected = false;
  }

  delay(1000);
}

void sendEmail() {
  SMTPData smtpData;
  smtpData.setLogin(smtpServer, smtpPort, emailSender, emailPassword);
  smtpData.setSender(emailSender);
  smtpData.setPriority("High");
  smtpData.setSubject("Alerta de Queda");
  smtpData.setMessage("Uma queda foi detectada! Verifique a situação.", true);
  smtpData.addRecipient(emailRecipient);

  if (!MailClient.sendMail(smtpData)) {
    Serial.println("Falha ao enviar email. Erro: " + MailClient.smtpErrorMessage());
  } else {
    Serial.println("Email enviado com sucesso!");
  }
}

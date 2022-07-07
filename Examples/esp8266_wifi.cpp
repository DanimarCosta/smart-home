// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

// Coloque as credenciais da sua rede
const char* ssid = "minharedewifi;
const char* password = "minhasenhawifi";

#define DHTPIN 5     // Digital pin D1 connected to the DHT sensor

//defino tipo de sensor em uso:
#define DHTTYPE    DHT11     // DHT 11


DHT dht(DHTPIN, DHTTYPE);

// Temperatura e humidades atual
float t = 0.0;
float h = 0.0;

// Criando um webserver
AsyncWebServer server(80);


unsigned long previousMillis = 0;    // vai gravar ultima medida

// atualiza a cada 10 segundos
const long interval = 10000;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .dht-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h3>Temperatura e umidade da sala sala servidor</h3>
  <p>
    <i class="fas fa-thermometer-half" style="color:#059e8a;"></i> 
    <span class="dht-labels">Temperatura</span> 
    <span id="temperature">%TEMPERATURE%</span>
    <sup class="units"> °C</sup>
  </p>
  <p>
    <i class="fas fa-tint" style="color:#00add6;"></i> 
    <span class="dht-labels">Umidade</span>
    <span id="humidity">%HUMIDITY%</span>
    <sup class="units">%</sup>
  </p>

  <p>
    <i class="fab fa-youtube" style="font-size:1.0rem;color:red;"></i>
    <span style="font-size:1.0rem;">Inscreva se:</span>
    <a href="https://www.youtube.com/channel/UCbQCNuVYocAGOWQkn1FwGjg" target="_blank" style="font-size:1.0rem;">TechMundo inscreva se no nosso canal</a>
  </P>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("temperature").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/temperature", true);
  xhttp.send();
}, 10000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("humidity").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/humidity", true);
  xhttp.send();
}, 10000 ) ;
</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "TEMPERATURE"){
    return String(t);
  }
  else if(var == "HUMIDITY"){
    return String(h);
  }
  return String();
}

void setup(){
  // porta serial para debugar
  Serial.begin(115200);
  dht.begin();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  // Mostra i IP que ele pegou na tela serial
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(t).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(h).c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Salva a ultima leitura feita
    previousMillis = currentMillis;
    // Ler a tempeatura em Graus Celsius
    float newT = dht.readTemperature();
      // Se falhar em ler o sensor de temeratura
    if (isnan(newT)) {
      Serial.println("Falha ao ler o sensor de temperatura!");
    }
    else {
      t = newT;
      Serial.println(t);
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // Se falhar em ler o sensor umidade 
    if (isnan(newH)) {
      Serial.println("Falha ao ler o Sensor umidade");
    }
    else {
      h = newH;
      Serial.println(h);
    }
  }
}

const char* MQTT_BROKER_ADRESS = "test.mosquitto.org";
const uint16_t MQTT_PORT = 1883;
const char* MQTT_CLIENT_NAME = "ESP32Client_1";
const char* topicSend = "topic/prueba";

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void SuscribeMqtt()
{
	mqttClient.subscribe("ST_1103");
	// mqttClient.subscribe("hello/world");
}

String payload;
void PublisMqtt(unsigned long data)
{
	payload = "";
	payload = String(data);
	mqttClient.publish(topicSend, (char*)payload.c_str());
	Serial.println("Mensaje enviado");
}

void PublisMqtt(String data)
{
	payload = "";
	payload = data;
	if(mqttClient.publish(topicSend, (char*)payload.c_str())){
		Serial.println("Mensaje enviado");
	} else{
		Serial.println("Error de envío");
	}
}

void publishJson(){
  JsonDocument doc;

  doc["S_id"]="TL_073";
  doc["S_type"]="TL";
  doc["Str_id"]="ST_1103";
  doc["Str_name"]="Calle Catedral de Toledo";
  doc["loc"]["lat"]=40.3636491;
  doc["loc"]["long"]=-3.7584933;
  doc["loc"]["zona"]="Latina";
  doc["loc"]["barrio"]="Lucero";

  char* str="";
  switch (state){
    case Gr:
      str="Gr";
      break;
    case Yr:
      str="Yr";
      break;
    case Rr1:
      str="Rr1";
      break;
    case Rg:
      str="Rg";
      break;
    case Rr2:
      str="Rr2";
      break;
    default:
      str="Error";
      break;
  }
  doc["data"]["state"]=str;
  doc["data"]["TL_type"]="mixed";
  if (state!=Gr){
    doc["data"]["time"]=stateTime[state-1];
  }
  doc["data"]["wait"]=waiting?1:0;
  doc["data"]["lock"]=emergencia?1:0;

  String output;
  serializeJson(doc, output);

  PublisMqtt(output);
}

void recieveMsg(byte* payload){
  JsonDocument doc;
  deserializeJson(doc, payload);

  int boton = doc["boton"];
	int docEmergencia = doc["emergencia"];
	int cooldown = doc["cooldown"];
	int timeYr = doc["timeYr"];
	int timeRr1 = doc["timeRr1"];
	int timeRg = doc["timeRg"];
	int timeRr2 = doc["timeRr2"];

  if (boton>0){
    waiting=true;
    digitalWrite(ESPERA, HIGH);
  }
	if (docEmergencia>0){
		state=Gr;
		emergencia=true;
		waiting=false;
		digitalWrite(ESPERA, LOW);
		publishJson();
	} else{
		emergencia=false;
	}
	if (cooldown>0){
		cooldownTime=cooldown;
	}
	if (timeYr>0){
		stateTime[0]=timeYr;
	}
	if (timeRr1>0){
		stateTime[1]=timeRr1;
	}
	if (timeRg>0){
		stateTime[2]=timeRg;
	}
	if (timeRr2>0){
		stateTime[3]=timeRr2;
	}
}

String content = "";
void OnMqttReceived(char* topic, byte* payload, unsigned int length) 
{
	Serial.print("Received on ");
	Serial.print(topic);
	Serial.print(": ");

	content = "";	
	for (size_t i = 0; i < length; i++) {
		content.concat((char)payload[i]);
	}
	Serial.print(content);
	Serial.println();

	recieveMsg(payload);
}
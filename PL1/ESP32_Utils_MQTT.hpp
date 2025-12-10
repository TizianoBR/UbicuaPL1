void InitMqtt() 
{
	mqttClient.setServer(MQTT_BROKER_ADRESS, MQTT_PORT);
	SuscribeMqtt();
	mqttClient.setCallback(OnMqttReceived);
}


void ConnectMqtt()
{
	while (!mqttClient.connected())
	{
		Serial.print("Starting MQTT connection...");
		if (mqttClient.connect(MQTT_CLIENT_NAME))
		{
			SuscribeMqtt();
			Serial.println("Connected");
		}
		else
		{
			Serial.print("Failed MQTT connection, rc=");
			Serial.print(mqttClient.state());
			Serial.println(" try again in 5 seconds");

			delay(5000);

			Serial.printf("\n--- Scan @ %lu ms ---\n", millis());
  		int n = WiFi.scanNetworks();
  		for (int i = 0; i < n; i++) {
    	Serial.printf("%2d: SSID='%s'  Chan=%d  RSSI=%d\n", i+1, WiFi.SSID(i).c_str(), WiFi.channel(i), WiFi.RSSI(i));
  		}
		}
	}
}


void HandleMqtt()
{
	if (!mqttClient.connected())
	{
		ConnectMqtt();
	}
	mqttClient.loop();
}
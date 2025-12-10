void ConnectWiFi_STA(bool useStaticIP = false)
{
   Serial.println("");
   WiFi.setAutoReconnect(false);
   WiFi.disconnect(true, true);  // borra credenciales + BSSID
   delay(500);

   esp_wifi_stop();
   delay(300);

   esp_wifi_deinit();
   delay(300);

   WiFi.mode(WIFI_OFF);
   delay(500);
   WiFi.mode(WIFI_STA);
  //  WiFi.setCountry("ES");
   if(useStaticIP) WiFi.config(ip, gateway, subnet);
   WiFi.begin(ssid, password);
   WiFi.disconnect(true);
   Serial.printf("SSID leído por ESP32 = '%s'\n", ssid);
   Serial.printf("PASS leído por ESP32 = '%s'\n", password);
   Serial.printf("Long SSID = %d\n", strlen(ssid));
   Serial.printf("Longitud pass = %d\n", strlen(password));

   delay(1000);

   Serial.println("Escaneando redes...");
   int n = WiFi.scanNetworks();
   Serial.print("Redes encontradas: ");
   Serial.println(n);
   for (int i = 0; i < n; ++i) {
     Serial.print(i + 1);
     Serial.print(": ");
     Serial.print(WiFi.SSID(i));
     Serial.print("  Canal: ");
     Serial.print(WiFi.channel(i));
     Serial.print("  RSSI: ");
     Serial.println(WiFi.RSSI(i));
   }

   while (WiFi.status() != WL_CONNECTED) 
   { 
     delay(100);  
     Serial.print('.'); 
   }

  //  unsigned long startAttemptTime = millis();
  //  while (WiFi.waitForConnectResult() != WL_CONNECTED && 
  //      millis() - startAttemptTime < 15000)      // 15 segundos
  //  {
  //    delay(100);
  //    Serial.print('.');
  //  }

   Serial.println(WiFi.status());
 
   Serial.println("");
   Serial.print("Iniciado STA:\t");
   Serial.println(ssid);
   Serial.print("IP address:\t");
   Serial.println(WiFi.localIP());
}

void ConnectWiFi_AP(bool useStaticIP = false)
{ 
   Serial.println("");
   WiFi.mode(WIFI_AP);
   while(!WiFi.softAP(ssid, password))
   {
     Serial.println(".");
     delay(100);
   }
   if(useStaticIP) WiFi.softAPConfig(ip, gateway, subnet);

   Serial.println("");
   Serial.print("Iniciado AP:\t");
   Serial.println(ssid);
   Serial.print("IP address:\t");
   Serial.println(WiFi.softAPIP());
}
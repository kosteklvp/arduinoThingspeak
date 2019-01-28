#include <dht11.h>
#include <UIPEthernet.h> 
                                                  
#define CzujnikTempWilgPin 2    
#define CzujnikOpadAnalogPin A0
#define CzujnikOpadDigitalPin 4
#define CzujnikRuchPin 8

byte mac[] = { 0x54, 0x34, 0x41, 0x30, 0x30, 0x31 }; 
EthernetClient client;
char server[] = "api.thingspeak.com"; 
    
int wartosc_A0;        
int wartosc_D0;   
int wilgotnosc;
int temperatura;
int ruch; 

static uint32_t timer;
dht11 DHT11; 
int interwal = 30000;                             
    
void setup()
{
  Serial.begin(9600);                             
  Ethernet.begin(mac);
  
  pinMode(CzujnikOpadDigitalPin, INPUT);          /
  pinMode(CzujnikRuchPin, INPUT);  

  Serial.print("IP Address        : ");
  Serial.println(Ethernet.localIP());
  Serial.print("Subnet Mask       : ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Default Gateway IP: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS Server IP     : ");
  Serial.println(Ethernet.dnsServerIP());
}

void loop()
{
  DHT11.read(CzujnikTempWilgPin);                 

  ruch = 0;

  wilgotnosc = DHT11.humidity; 
  Serial.print("Wilgotnosc (%): ");             
  Serial.print((float)wilgotnosc, 0);
  temperatura = DHT11.temperature;
  Serial.print("     Temperatura (C): ");           
  Serial.println((float)temperatura, 0);
  
           //odczyt danych z czujnika ruchu
  if(digitalRead(CzujnikRuchPin) == HIGH){                                    
    Serial.println("Wykryto ruch");
    ruch = 1; 
  } else {
    Serial.println("Nie wykryto ruchu");
  }
  
  wartosc_A0 = analogRead(CzujnikOpadAnalogPin);  
  wartosc_D0 = digitalRead(CzujnikOpadDigitalPin);
 
  if (wartosc_D0 == LOW) {
    Serial.print("DESZCZ PADA Z INTENSYWNOŚCIĄ ");
    Serial.println(wartosc_A0);
  } else {
    Serial.println("DESZCZ NIE PADA");
  }
  
  if (client.connect(server, 80)) {
    client.print("GET /update?");
    client.print("key=POZ0YSHN2VGMKS05");
    
    client.print("&field1=");
    client.print(temperatura);
    
    client.print("&field2=");
    client.print(wilgotnosc);
    
    client.print("&field3=");
    client.print(wartosc_D0);
    
    if(wartosc_D0==LOW) {
      client.print("&field4=");
      client.print(wartosc_A0);
    }
        
    client.print("&field5=");
    client.print(ruch);
    
    client.println( " HTTP/1.1");
    client.print( "Host: " );
    client.println(server);
    client.println( "Connection: close" );
    client.println();
    client.println();
    client.stop();
    Serial.println("-> Pomysłnie przesłano dane" );
    ruch = 0;
    
  }
  else {
    Serial.println("--> wystąpił błąd z połaczeniem/n");
  }

  delay(interwal);
} 

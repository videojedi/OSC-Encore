

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OSCBundle.h>

// you can find this written on the board of some Arduino Ethernets or shields
byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 

// NOTE: Alternatively, you can assign a fixed IP to configure your
//       Ethernet shield.
      byte ip[] = { 192, 168, 000, 177 };
      byte e2IP[] = {192, 168, 000, 175 };
      


int serverPort  = 8000; //TouchOSC (incoming port)
int e2port = 9878;    //TouchOSC (outgoing port)
int ledPin =  13;       //pin 13 on Arduino Uno. Pin 6 on a Teensy++2
int ledState = LOW;
float value = 0;
String inData;

//Create UDP message object
EthernetUDP Udp;
EthernetClient client;

void setup(){
  Serial.begin(9600); //9600 for a "normal" Arduino board (Uno for example). 115200 for a Teensy ++2 
  Serial.println("OSC test");

  // start the Ethernet connection:
  // NOTE: Alternatively, you can assign a fixed IP to configure your
  //       Ethernet shield.
Ethernet.begin(mac, ip);   
  //if (Ethernet.begin(mac) == 0) {
    //Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    //while(true);
  //}
  // print your local IP address:
  Serial.print("Arduino IP address: ");
  for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
    Serial.print(Ethernet.localIP()[thisByte], DEC);
    Serial.print(".");  
    }
    Serial.println("");

if (client.connect(e2IP,e2port)) {
  
Serial.println("Connected");
     
}

  Udp.begin(serverPort);
}
void loop(){
  //process received messages
  OSCMsgReceive();
} 

void OSCMsgReceive(){
  OSCBundle msgIN;
  int size;
  if((size = Udp.parsePacket())>0){
    
    while(size--)
      msgIN.fill(Udp.read());

    if(!msgIN.hasError()){
      
      msgIN.route("/cue",preset);
      msgIN.route("/test1",funcValue);
    }
    else Serial.println("Error...");
  }
}

void preset(OSCMessage &msg, int addrOffset){
  Serial.println("/cue");
  float value = msg.getFloat(0);
  //OSCMessage msgOUT("/OnOff/toggle1");
if (client.connected()) {
  client.print("preset -r ");
  Serial.print("preset -r ");
    client.println(value);
    Serial.println(value);
    delay(1000);
    if (ackn() == 0) {
      Serial.println("Success!");
      ATRN();
    }
      else Serial.println("Failed!");
    }
}





void funcValue(OSCMessage &msg, int addrOffset ){
  Serial.println("Test1");
  float value = msg.getFloat(0);
  //OSCMessage msgOUT("/Fader/Value");

  Serial.print("Value = : ");
  Serial.println(value);

}

void ATRN() {

  if (client.connected()) {
    client.println("atrn 60");
    Serial.println("atrn 60");


  }

}


int ackn() {

  inData = "";    //Clear buffer
  Serial.println("ack?");
  int error;

  while (client.available()) {
    char c = client.read();
    inData += c;
    if (inData.endsWith("-e "))
    {
      client.setTimeout(100);
      error = client.parseInt();
      Serial.print("Error:");
      Serial.println(error);
      break;
    }

    else if (c == '\n')
    {

      Serial.print("Recieved:");
      Serial.println(inData);
      inData = "";    //Clear buffer
    }
  }
  inData = "";
  return error;
}

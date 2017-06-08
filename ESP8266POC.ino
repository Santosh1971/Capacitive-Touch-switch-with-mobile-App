/*

Observations 
1. Power supply  12V  ;1A   ,  5V;  .3A ,   3.3v; 500 ma 
2.store the last status of relays in EEPROM when the status changes. Reload the same when system restarted when the system is switched off and then switched on.
//3.check what happens when mobile hotspot is switched off and then switched on.  Does ESP8266 intimate back to controller so that the Esp8266 can be reset and continously look for hotspot network for which it has been set.
//4. At the very start up or the factory reset the system will be in AP  or hotspot mode. With fixed hotspot name and no password. 
4. Use the system in AP mode always, anybody can download and use the app in the system WiFi range. if we want to restrict to few user.. change the password.
//5.there should be a switch on the system to set the system to factory reset.
//6. In mobile app there should be provision to set the system from server mode to client mode by providing the mobile hotspot name and password. 
7.optocoupler based sensing the stats of the load which is updated when it communicate with system.as the status can be changed using manual switch.
8.external LED Indication of system connected etc.
9. Wifi based FOTA.  Upgrade software using Android app by downloading from websites. 
10. Model no and system identification both software and hardware to be stored in system which needs to be be communicated to mobile apps
//11.there should be provision to add 4 to 6 mobile phones  to control. System will pole all the network to see if there is any command from other hotspot. 
11. Any no. of mobile can be connected with ESP8266 network and control
12 DALI  compatible systems can be connected.
13 . DALI based communication port option.
14. Profile storage.which will get activated as soon as owners mobile comes in the range of ESP8266 range. 
15. The no of rooms and the lights I'd should be  configurable in mobile apps.
*/




//Leonardo
//Serial_ & dbgTerminal = Serial;
//HardwareSerial & espSerial = Serial1;

////UNO & M328P
//#include <SoftwareSerial.h>
//SoftwareSerial dbgTerminal(10, 11); // RX, TX
//HardwareSerial & espSerial = Serial;


//MEGA2560 
HardwareSerial & monitor = Serial;
HardwareSerial & esp_serial = Serial1;

// set pin numbers:
const int ledPin =  13;      // the number of the LED pin, Onboard LED , active high

int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor

const int buttonPin = 2;     // the number of the pushbutton pin
int buttonState = 0;         // variable for reading the pushbutton status



const int relay1 =  22;
const int relay2 =  23;
const int relay3 =  24;
const int relay4 =  25;

//const int ESP8266_CHPD = 8;

// Variables will change:
int ledState = HIGH;             // ledState used to set the LED .. Indicate that ESP8266 is in Network

int Relay1State = LOW;             // Relay1State   
int Relay2State = LOW;             // Relay2State 
int Relay3State = LOW;             // Relay3State 
int Relay4State = LOW;             // Relay4State 

#define BUFFER_SIZE 128
char buffer[BUFFER_SIZE];


int result = 0;
int OK = 1;
int TIMEOUT = 2 ;
int CANCEL = 3;
int NOT_FOUND = 4;
int YES =1;
int NO =0;
int FactoryRESET = YES;  // ESP8266 HOTSPOT NO;
  
int i;
int m=0;
int lp = 0;

char inputChar[500];
int len = 0;


// My Android Mobile Hotspot ( without internet)- "SANTOSH_Mobile" ( assigned IP to EPS8266 - 192.168.43.194 ) , very fast
// stand alone Hotspot  ( without Internet ) "Lava_W520_56D2"  ( assigned IP to EPS8266 - 192.168.169.100 ) , 2-3 sec delay , delay but doesnot misses the command
// Router Wifi network ( with Internet ) "SANTOSH_Network" ( assigned IP to EPS8266 - 192.168.0.50 ) , very fast

String ssid ="SANTOSH_BB";// "SANTOSH_Mobile"; //  //  //   //"Lava_W520_56D2"; //       
String pwd = "snch2000@";             //
//String serverip="192.168.1.37";   // 106.51.240.161
String port="80";  //"80";
String cmd;
String myIP;

int currentID;

int ch_id, packet_len;
char *pb;


void(* resetFunc) (void) = 0;                          //declare reset function at address 0

void setup() { 

// initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT); 
 
  pinMode(ledPin, OUTPUT); 
  pinMode(relay1, OUTPUT); 
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  
  //pinMode(ESP8266_CHPD, OUTPUT);
  //digitalWrite(ESP8266_CHPD,HIGH);


  digitalWrite(relay1,HIGH);   // For comman Anaode LED
  digitalWrite(relay2,HIGH);  // For comman Anaode LED
//  digitalWrite(relay1,LOW);
//  digitalWrite(relay2,LOW);
  digitalWrite(relay3,LOW);
  digitalWrite(relay4,LOW);


      
  monitor.begin(115200);           // Serial monitor
  esp_serial.begin(115200);             // ESP8266  
  //while (!dbgTerminal) {
   // ; // wait for serial port to connect. Needed for Leonardo only
  //}

  monitor.println(F("\nESP8266 WebServer Test 1.00"));
  //hardReset();
  //   delay(1000);
  
  
  /* RESET Module */


    esp_serial.println("AT+RST");                     // Software Reset 
    monitor.print("\nAT+RST 115200");

  result = CheckResponse();                         // Check "OK"   ok=1 , Timeout=2
  if(result == OK){
    Serial.print(" OK");
    delay(300);
  }
  else{
    Serial.print(" ERROR");
    delay(2000);       
    resetFunc();   
  }



  SET_WiFi();
    
 
      
    
  digitalWrite(ledPin,ledState);  
  //delay(3000);
           
}// Setup





void loop() {


  // read the value from the sensor:
  sensorValue = analogRead(sensorPin); 
    
  // turn the ledPin on
//  digitalWrite(ledPin, HIGH);  
  // stop the program for <sensorValue> milliseconds:
//  delay(sensorValue);          
  // turn the ledPin off:        
//  digitalWrite(ledPin, LOW);   
  // stop the program for for <sensorValue> milliseconds:
//  delay(sensorValue);



  // read the state of the pushbutton value:
//  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed.
  // if it is, the buttonState is HIGH:
//  if (buttonState == HIGH) {     
    // turn LED on:    
  //  digitalWrite(ledPin, HIGH);  
 // } 
//  else {
    // turn LED off:
 //   digitalWrite(ledPin, LOW); 
  //}

   
  esp_serial.readBytesUntil('\n', buffer, BUFFER_SIZE);
  
  if(strncmp(buffer, "+IPD,", 5)==0) {
    
     //monitor.print("\nBuffer");
    // request: +IPD,ch,len:data
    sscanf(buffer+5, "%d,%d", &ch_id, &packet_len);
    if (packet_len > 0) {
      //monitor.print("\nlen > 0");
      // read serial until packet_len character received
      // start from :
      pb = buffer+5;
      while(*pb!=':') pb++;
      pb++;
      
      if (strncmp(pb, "GET /led", 8) == 0) {
        monitor.print(millis());
        monitor.print(" : ");
        monitor.println(buffer);
        monitor.print( "get led from ch :" );
        monitor.println(ch_id);
 
        delay(100);
        clearSerialBuffer();
             
       if (ledState == LOW)
          ledState = HIGH;
       else
          ledState = LOW;
        digitalWrite(ledPin, ledState);
        
        //delay(3000);
        homepage(ch_id);        
        
      } 
      
      if (strncmp(pb, "GET /USER", 9) == 0) {
        monitor.print(millis());
        monitor.print(" : ");
        monitor.println(buffer);
        monitor.print( "get USER from ch :" );
        monitor.println(ch_id);
 
        delay(100);
        clearSerialBuffer();
         FactoryRESET =NO;  
   
        homepage(ch_id);        
        SET_WiFi();
       
      } 

     if (strncmp(pb, "GET /ESP", 8) == 0) {
        monitor.print(millis());
        monitor.print(" : ");
        monitor.println(buffer);
        monitor.print( "get ESP from ch :" );
        monitor.println(ch_id);
 
        delay(100);
        clearSerialBuffer();
         FactoryRESET =YES;  
   
        homepage(ch_id);        
       SET_WiFi();
      }       
      
      if (strncmp(pb, "GET /?relay1on", 14) == 0) {
        monitor.print(millis());
        monitor.print(" : ");
        monitor.println(buffer);
        monitor.print( "get led from ch :" );
        monitor.println(ch_id);
        delay(100);
        clearSerialBuffer();            
      //  digitalWrite(relay1, HIGH);
         digitalWrite(relay1, LOW); // For comman Anaode LED
        Relay1State=HIGH;
        homepage(ch_id);              
      } 
           
      if (strncmp(pb, "GET /?relay1off", 14) == 0) {
        monitor.print(millis());
        monitor.print(" : ");
        monitor.println(buffer);
        monitor.print( "get led from ch :" );
        monitor.println(ch_id);
        delay(100);
        clearSerialBuffer();            
       // digitalWrite(relay1, LOW);
        digitalWrite(relay1, HIGH); // For comman Anaode LED
        Relay1State=LOW;
        homepage(ch_id);              
      } 
      
      if (strncmp(pb, "GET /?relay2on", 14) == 0) {
        monitor.print(millis());
        monitor.print(" : ");
        monitor.println(buffer);
        monitor.print( "get led from ch :" );
        monitor.println(ch_id);
        delay(100);
        clearSerialBuffer();            
     //   digitalWrite(relay2, HIGH);
       digitalWrite(relay2, LOW);// For comman Anaode LED
        Relay2State=HIGH;
        homepage(ch_id);              
      } 
            
      if (strncmp(pb, "GET /?relay2off", 14) == 0) {
        monitor.print(millis());
        monitor.print(" : ");
        monitor.println(buffer);
        monitor.print( "get led from ch :" );
        monitor.println(ch_id);
        delay(100);
        clearSerialBuffer();            
    //    digitalWrite(relay2, LOW);
       digitalWrite(relay2, HIGH); // For comman Anaode LED
        Relay2State=LOW;
        homepage(ch_id);              
      } 
      
      
      if (strncmp(pb, "GET /?relay3on", 14) == 0) {
        monitor.print(millis());
        monitor.print(" : ");
        monitor.println(buffer);
        monitor.print( "get led from ch :" );
        monitor.println(ch_id);
        delay(100);
        clearSerialBuffer();            
        digitalWrite(relay3, HIGH);
      //  digitalWrite(relay3, LOW);
        Relay3State=HIGH;
        homepage(ch_id);              
      } 
            
      if (strncmp(pb, "GET /?relay3off", 14) == 0) {
        monitor.print(millis());
        monitor.print(" : ");
        monitor.println(buffer);
        monitor.print( "get led from ch :" );
        monitor.println(ch_id);
        delay(100);
        clearSerialBuffer();            
        digitalWrite(relay3, LOW);
     //   digitalWrite(relay3, HIGH);
        Relay3State=LOW;
        homepage(ch_id);              
      } 
      
      if (strncmp(pb, "GET /?relay4on", 14) == 0) {
        monitor.print(millis());
        monitor.print(" : ");
        monitor.println(buffer);
        monitor.print( "get led from ch :" );
        monitor.println(ch_id);
        delay(100);
        clearSerialBuffer();            
        digitalWrite(relay4, HIGH);
     //   digitalWrite(relay4, LOW);
        Relay4State=HIGH;
        homepage(ch_id);              
      } 
            
      if (strncmp(pb, "GET /?relay4off", 14) == 0) {
        monitor.print(millis());
        monitor.print(" : ");
        monitor.println(buffer);
        monitor.print( "get led from ch :" );
        monitor.println(ch_id);
        delay(100);
        clearSerialBuffer();            
       digitalWrite(relay4, LOW);
     //   digitalWrite(relay4, HIGH);
        Relay4State=LOW;
        homepage(ch_id);              
      } 
      
       //else if (strncmp(pb, "GET / ", 6) == 0) {
      if (strncmp(pb, "GET / ", 6) == 0) {
        monitor.print(millis());
        monitor.print(" : ");
        monitor.println(buffer);
        monitor.print( "get Status from ch:" );
        monitor.println(ch_id);
        
        delay(100);
        clearSerialBuffer();
        homepage(ch_id);             
      }
       
    }
    
  }
//  monitor.print(buffer);
  clearBuffer();
  
  lp++;
  monitor.print( "\nLoop "); 
  monitor.print(lp);
    
}





void homepage(int ch_id) {
  String Header;

  Header =  "HTTP/1.1 200 OK\r\n";
  Header += "Content-Type: text/html\r\n";
  Header += "Connection: close\r\n";  
//  Header += "Refresh: 5\r\n";
  
  String Content;
 
  
 // Content = "<html><header><title>ESP8266 Webserver</title></header>";
 
 // Content += "<center> <p> <h1 style=\"color: #FF0; background: #000\">ESP8266 WiFi Web Server 0.22  </h1></p>";
 /* 
  Content += "<a href=\"/?relay1on\"> <button style=\"width:320px;height:120px\"> <font size=\"7\">Device 1 ON </font> </button> </a>";
  Content += "<a href=\"/?relay1off\"> <button style=\"width:320px;height:120px\"> <font size=\"7\">Device 1 OFF </font> </button> </a> <br/><br/>";
  
  Content += "<a href=\"/?relay2on\"> <button style=\"width:200px;height:50px\"> <font size=\"4\">Device 2 ON </font> </button> </a>";
  Content += "<a href=\"/?relay2off\"> <button style=\"width:200px;height:50px\"> <font size=\"4\">Device 2 OFF </font> </button> </a> <br/><br/>";
  
  Content += "<a href=\"/?relay3on\"> <button style=\"width:200px;height:50px\"> <font size=\"4\">Device 3 ON </font> </button> </a>";
  Content += "<a href=\"/?relay3off\"> <button style=\"width:200px;height:50px\"> <font size=\"4\">Device 3 OFF </font> </button> </a> <br/><br/>";
  
  Content += "<a href=\"/?relay4on\"> <button style=\"width:200px;height:50px\"> <font size=\"4\">Device 4 ON </font> </button> </a>";
  Content += "<a href=\"/?relay4off\"> <button style=\"width:200px;height:50px\"> <font size=\"4\">Device 4 OFF </font> </button> </a> <br/><br/>";
  
  Content += "<a href=\"/led\"> <button style=\"width:200px;height:50px\"> <font size=\"4\">LED </font> </button> </a><br/><br/>";
  
  
  Content += "<hr> <p> By <a href=\"http://androidcontrol.blogspot.com\"></p><p style=\"font-family:arial;color:blue;font-size:28px;\">Android Control Blogspot</p></a>";
  
 // Content += "</body></html>";
  */
 // Content += "D";
  Content = "LED1=";
  Content += String(ledState);
  Content += ", ";
  Content += "RLY1=";
  Content += String(Relay1State);
  Content += ", ";
  Content += "RLY2=";
  Content += String(Relay2State);
  Content += ", ";
  Content += "POT1=";
  Content += String(sensorValue);
  Content += ", ";
  Content += "SW1=";
  Content += String(buttonState);
  Content += ", ";
  Content += "Network=";
  if (FactoryRESET ==YES){
  Content += "ESP";
  }
   if (FactoryRESET ==NO){
  Content += ssid;
  } 

  
  
//Content += "</body></html>";
  
  Header += "Content-Length: ";
  Header += (int)(Content.length());
  Header += "\r\n\r\n";
  
 START_SENT: 
  esp_serial.print("AT+CIPSEND=");
  esp_serial.print(ch_id);
  esp_serial.print(",");
  esp_serial.println(Header.length()+Content.length());
  
  monitor.print("AT+CIPSEND=");
  monitor.print(ch_id);
  monitor.print(",");
  monitor.println(Header.length()+Content.length());
  delay(10);                                        //*** effect Web page
 
  
  
  // for debug buffer serial error
  //while (espSerial.available() >0 )  {
  //  char c = espSerial.read();
  //  dbgTerminal.write(c);
  //  if (c == '>') {
  //      espSerial.print(Header);
  //      espSerial.print(Content);
  //  }
  //}
  
  if (esp_serial.find(">")) {
      esp_serial.print(Header);
      esp_serial.print(Content);
      delay(10);     
   } 

}



void clearSerialBuffer(void) {
       while ( esp_serial.available() > 0 ) {
         esp_serial.read();
       }
}


void clearBuffer(void) {
       for (int i = 0;i<BUFFER_SIZE;i++ ) {
         buffer[i]=0;
       }
}


void SET_WiFi(void) {
 
 SET_MODE:
  
  if(FactoryRESET == YES){
        Serial.print("\n Switch to Factory RESET");
        esp_serial.println("AT+CWMODE=3");                // Set Mode  1= Sta, 2= AP, 3=both // (changed to station mode from ap mode, and removed the html lines, because not need for the app.)
        monitor.print("\nAT+CWMODE=3");                  // Access point mode ,,  add   RENAME_AP:   and  comment out DISCONNECT_AP:  and CONNECT_WIFI: 

                                                        //Now you can run the server example from above and connect–note that the module always has the
                                                        //IP 192.168.4.1 when acting as an AP.
       result = CheckResponse();                         // Check "OK" 
      if(result == OK){
        Serial.print(" OK");
        delay(200);
      }
      else{
        Serial.print(" ERROR");
        delay(1000);
        goto SET_MODE;
      }
        delay(300);
        
RENAME_AP:

      cmd = "AT+CWSAP=" + String("\"") + "SANTOSH_ESP8266" + String("\"") + String(",")+String("\"") + "snch2000" + String("\"")+ String(",")+3+ String(",")+0;
      esp_serial.println(cmd);                    // Rename access point
      monitor.print("\n"+cmd);

                                                                                                                          //          Channel  Freq. Channel Freq.
                                                                                                                        //                   1 2412  8 2447
                                                                                                                          //                 2 2417  9 2452
                                                                                                                            //               3 2422  10  2457
                                                                                                                         //                  4 2427  11  2462
                                                                                                                         //                  5 2432  12  2467
                                                                                                                          //                 6 2437  13  2472
                                                                                                                           //                7 2442  14  2484

      result = CheckResponse();                         // Check "OK" 
      if(result == OK){
        Serial.print(" OK");
        delay(300);
      }
      else{
        Serial.print(" ERROR");
        delay(1000);       
        goto RENAME_AP;  
      }

      
      }
      else{
        Serial.print("\nSwitch to User Hot Spot Network");
        esp_serial.println("AT+CWMODE=1");                // Set Mode  1= Sta, 2= AP, 3=both // (changed to station mode from ap mode, and removed the html lines, because not need for the app.)
        monitor.print("\nAT+CWMODE=1");
        result = CheckResponse();                         // Check "OK" 
      if(result == OK){
        Serial.print(" OK");
        delay(200);
      }
      else{
        Serial.print(" ERROR");
        delay(1000);
        goto SET_MODE;
      }
        delay(300);

DISCONNECT_AP:
     esp_serial.println("AT+CWQAP");                    // Disconnect AP 
      monitor.print("\nAT+CWQAP");
  
      result = CheckResponse();                         // Check "OK" 
      if(result == OK){
        Serial.print(" OK");
        delay(300);
      }
      else{
        Serial.print(" ERROR");
        delay(1000);       
        goto DISCONNECT_AP;  
      }
  
  
  
  CONNECT_WIFI:
  
      cmd = "AT+CWJAP=" + String("\"") + ssid + String("\"") + String(",")+String("\"") + pwd + String("\"");
      esp_serial.println(cmd);                    // Connect AP 
      monitor.print("\n"+cmd);
  
      result = CheckResponse();                         // Check "OK" 
      if(result == OK){
        Serial.print(" OK");
        delay(300);      
      }
      else{
        Serial.print(" ERROR");
        delay(1000);  
        goto CONNECT_WIFI;  
          
      }

 
              
      }
  

 


      
  
      
     
 SET_MULTIPLE:
     esp_serial.println("AT+CIPMUX=1");                 // Multiple Connnect 
      monitor.print("\nAT+CIPMUX=1");
  
      result = CheckResponse();                         // Check "OK" 
      if(result == OK){
        Serial.print(" OK");
        delay(300);
      }
      else{
        Serial.print(" ERROR");
        delay(1000);       
        goto SET_MULTIPLE;  
      }      
      
    
  START_SERVER:
      cmd = "AT+CIPSERVER=" + String("1") + String(",") + port;
      
      esp_serial.println(cmd);                          // Start Server 
      monitor.print("\n"+cmd);
  
      result = CheckResponse();                         // Check "OK" 
      if(result == OK){
        Serial.print(" OK");
        delay(200);
      }
      else{
        Serial.print(" ERROR");        
        delay(1000); 
      }      
      
     
  CHECK_IP:
       
      esp_serial.println("AT+CIFSR");                 // Multiple Connnect 
      monitor.print("\nAT+CIFSR");
      
      readSerial();  
      //monitor.print(inputChar);   
      monitor.print(inputChar);  
      readSerial();  
      //monitor.print(inputChar);
      monitor.print(inputChar);  
      readSerial();  
      monitor.print(inputChar);
      
      for(unsigned char i=0;i<2;i++){     
        esp_serial.println("AT+CIPCLOSE");                // Close All Socket 
        monitor.print("\nAT+CIPCLOSE");
        result = CheckResponse();                         // Check "OK" 
        if(result == OK){
          Serial.print(" OK");
          delay(200);
        }
        else{
          Serial.print(" ERROR");
          delay(200);       
       //   goto START_SERVER;  
        }          
     }


     
    
}


int readSerial(){
  
  int i = 0;
  while(1){ 
    while (esp_serial.available() > 0){    
      char inChar = esp_serial.read();
      
      if(i > 0){
        if (inChar == '\n'){
          inputChar[i] = '\0';
          esp_serial.flush();
          return 0;
          //break;
        }
      }
      
      if(inChar!='\r'){
        inputChar[i] = inChar;
        i++;
      }
      
    }
  } 
  
}


int CheckResponse(){
  
  //for(m=0;m<500;m++){
  for(m=0;m<500;m++){
      if(esp_serial.read()=='K'){         // Check "OK"      
      return OK;
      }
      delay(10);
  }
  return TIMEOUT;  
}







/*
Set          Inquiry         Test            Execute
AT+<x>=<…>  AT+<x>?         AT+<x>=?        AT+<x>
AT+CWMODE=<mode>  AT+CWMODE?  AT+CWMODE=? -
Set the network mode  Check current mode  Return which modes supported  -
Commands

carefully there are must be no any spaces between the " and IP address or port
Commands  Description Type  Set/Execute Inquiry test  Parameters  Examples
AT+RST  restart the module  basic - - - -
AT+CWMODE wifi mode wifi  AT+CWMODE=<mode>  AT+CWMODE?  AT+CWMODE=? 1= Sta, 2= AP, 3=both
AT+CWJAP  join the AP wifi  AT+ CWJAP =<ssid>,< pwd > AT+ CWJAP?  - ssid = ssid, pwd = wifi password
AT+CWLAP  list the AP wifi  AT+CWLAP      
AT+CWQAP  quit the AP wifi  AT+CWQAP  - AT+CWQAP=?  
AT+ CWSAP set the parameters of AP  wifi  AT+ CWSAP= <ssid>,<pwd>,<chl>, <ecn>  AT+ CWSAP?    ssid, pwd, chl = channel, ecn = encryption  Connect to your router: ：AT+CWJAP="YOURSSID","helloworld"; and check if connected: AT+CWJAP?
AT+ CIPSTATUS get the connection status TCP/IP  AT+ CIPSTATUS     
AT+CIPSTART set up TCP or UDP connection  TCP/IP  1)single connection (+CIPMUX=0) AT+CIPSTART= <type>,<addr>,<port>; 2) multiple connection (+CIPMUX=1) AT+CIPSTART= <id><type>,<addr>, <port>  - AT+CIPSTART=? id = 0-4, type = TCP/UDP, addr = IP address, port= port Connect to another TCP server, set multiple connection first: AT+CIPMUX=1; connect: AT+CIPSTART=4,"TCP","X1.X2.X3.X4",9999
AT+CIPSEND  send data TCP/IP  1)single connection(+CIPMUX=0) AT+CIPSEND=<length>; 2) multiple connection (+CIPMUX=1) AT+CIPSEND= <id>,<length>    AT+CIPSEND=?    send data: AT+CIPSEND=4,15 and then enter the data
AT+CIPCLOSE close TCP or UDP connection TCP/IP  AT+CIPCLOSE=<id> or AT+CIPCLOSE   AT+CIPCLOSE=? 
AT+CIFSR  Get IP address  TCP/IP  AT+CIFSR    AT+ CIFSR=? 
AT+ CIPMUX  set mutiple connection  TCP/IP  AT+ CIPMUX=<mode> AT+ CIPMUX?   0 for single connection 1 for mutiple connection
AT+ CIPSERVER set as server TCP/IP  AT+ CIPSERVER= <mode>[,<port> ]     mode 0 to close server mode, mode 1 to open; port = port  turn on as a TCP server: AT+CIPSERVER=1,8888, check the self server IP address: AT+CIFSR=?
+IPD  received data     
*/

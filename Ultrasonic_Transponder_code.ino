/* 
Ultrasonic Transponder Arduino Code v.1.0
Author: Kenny Alas - kenny.alas.972@my.csun.edu
Date: 9/10/2023
Description 

This is a Transmitting and Receiving Code on a Arduino Mega Using 2 UART PORTS (Pins 16-19).
This code is capable of sending a predetermined message (LIMIT 32 CHAR) across the TX1 line and receives it on the RX2 line 
While this code executes the total time of execution is being tracked, as well as the total time it takes to transmit and 
the total time it takes to receive. The total execution time is then subtracted from the sum of the transmit and receive time
to calculate the time of transmition. THis is then divided by 2 and multiplied by the speed of sound in ft/s to obtain the distance.

*/

/* All these variables correspond to calculating distance */
/***********************************************************************************************************/
const unsigned long speed_sound = 1.122;    // Speed of UltraSounic Wave is ~ 1.122 ft/ms
const unsigned long speed_light = 3E6;      // Speed of Light ~ 3E6 m/ms
// This keeps track of the overall time of code execution
unsigned long ExecStart;
unsigned long ExecStop;
unsigned long ExecTime = ExecStop - ExecStart;

// This tracks the total time it takes to transmit a message
unsigned long SendTime_start;
unsigned long SendTime_finish;
unsigned long SendTime = SendTime_finish - SendTime_start;

// This tracks the total time it takes to receive a message
unsigned long RcvdTime_start;
unsigned long RcvdTime_finish;
unsigned long RcvdTime = RcvdTime_finish - RcvdTime_start;

unsigned long TravelTime = ExecTime - (SendTime + RcvdTime); // in ms
unsigned long Distance = (TravelTime/2)*speed_sound; // in ft

/***********************************************************************************************************/
/* All These variable are for the following: delays, error check, flags, and storing messages */


int d = 11;                               // This is the delay variable for actual transmition. Can be ommited for wire testing. 

int bit_error_cnt = 0;                    // This is the # of errros found in the transmition message 
bool FLAG = false;                        // This flag is used for checking errors in message 
bool key = false;                         // This flag keeps the distance and total time from printing unless a correct message is received

char sent_mes[] = "ULTRASONIC TRANSPONDER";   // This is the message we send 
char rcvd_mes[32];                            // string to store message received 

// This runs once and sets up all Serial ports and pins
void setup() {

  Serial.begin(1200,SERIAL_8N2);                    // This belongs to UART0 Which connects to computer 
  Serial1.begin(1200, SERIAL_8N2);                  // This belongs to UART1 which is transmitting the message 
  Serial2.begin(1200, SERIAL_8N2);                  // This belongs to UART2 which is receiving the transmitted message 
  Serial.println("*********** Program Ready **********");
  Serial.println("");

}

// Belongs to UART 1
void transmit_tx1(){
  SendTime_start = millis();
  for ( int i = 0; i < sizeof(sent_mes); i++){
    Serial1.write(sent_mes[i]);
    delay(d);
  }
  SendTime_finish = millis();
}
// The above function transmit() transmits the preselected message char by char to the receiver. The delay is to allow receiver to store characters. 11ms 

// Belongs to UART 2 
void receive_rx2(){
  RcvdTime_start = millis(); 
  for(int ndx = 0; ndx < 32 ; ndx ++){
      int x = Serial2.read();   
      char data = x;
      rcvd_mes[ndx] = data;
      delay(d); 
  }
  RcvdTime_finish = millis();
  check();
  if (FLAG == false){
    Serial.print("Received Message: ");
    Serial.println(rcvd_mes);
    key = true;
  }
  FLAG = false;
}
// The above function received() reads incoming data, converts it into correct data type, saves it and prints the message.

// This is for testing purposes to use it send the same message back from other transponder. 
void check(){
  for ( int ndx = 0 ; ndx < sizeof(sent_mes) ; ndx++ ){
    if ( sent_mes[ndx] != rcvd_mes[ndx] ){
      FLAG = true;
      bit_error_cnt = bit_error_cnt + 1;
    }
  }
  if (FLAG == true){
    Serial.println("# of char errors:");
    Serial.println(bit_error_cnt);
    bit_error_cnt = 0;
  }
}

//clears the received message string
void clear_rcvd(){
  for( int i = 0 ; i < sizeof(rcvd_mes) ; i++){
    rcvd_mes[i] = '\0';
  }
}

void loop() {
  ExecStart = millis();

  // Message size check.
  if (sizeof(sent_mes) > 32){ 
    Serial.println("*****ERROR*****");
    Serial.print("Message is too large. Your message size is: ");
    Serial.println(sizeof(sent_mes));
  }

  transmit_tx1(); // This transmits message

  Serial.println("***************************************************");
  Serial.println(" ");
  Serial.print("Transmitted Message: ");
  Serial.println(sent_mes);

  if (Serial2.available() > 0){
    receive_rx2(); // this receives messages
  }

  ExecStop = millis();

  if (key == true){
    Serial.print("Total Time (in ms): ");
    Serial.println(TravelTime); 
    Serial.print("Distance Calculated (in ft): ");
    Serial.println(Distance);
    Serial.println(" ");
    key = false;
  }
  
  Serial.println("***************************************************");
  clear_rcvd();
  delay(3000);

}

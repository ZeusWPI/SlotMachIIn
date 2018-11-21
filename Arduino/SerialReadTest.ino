void setup() {
  Serial.begin(9600);
}

bool openDoor(){
 return true;
 //TODO: control motor to open door
 //Return true if it was able to open the door
  }
bool closeDoor(){
 //TODO: control motor to close door
 //Return true if it was able to close the door
 return false;
  }

void loop() {
  if (Serial.available() > 0) {
   String tekst = Serial.readString();
   tekst.trim();
   //Check Serial Input
    //If input is "open", open the door.
   if (tekst == "open"){   
    if(openDoor()){
      Serial.println("open");
    } else {
      Serial.println("error");
    }
    //If input is "close", close the door.
   } else if(tekst == "close"){
    if(closeDoor()){
     Serial.println("close");
    } else {
     Serial.println("error");
    }
  }
 }
}

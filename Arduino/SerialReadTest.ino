void setup() {
  Serial.begin(9600);
}

bool openDoor(){
 return true;
  }
bool closeDoor(){
 return false;
  }

void loop() {
  if (Serial.available() > 0) {
   String tekst = Serial.readString();
   tekst.trim();
   if (tekst == "open"){   
    if(openDoor()){
      Serial.println("Gelukt!");
    } else {
      Serial.println("Niet gelukt!");
    }
   } else if(tekst == "close"){
    if(closeDoor()){
     Serial.println("Gelukt!");
    } else {
     Serial.println("Niet gelukt!");
    }
  }
 }
}

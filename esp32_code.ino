```cpp
int donorCounter = 1;

void setup() {

  Serial.begin(115200);

  pinMode(2, OUTPUT);
}

void loop() {

  if (Serial.available() > 0) {

    String command = Serial.readStringUntil('\n');

    command.trim();

    if (command == "SEND_KEY") {

      digitalWrite(2, HIGH);

      String donorID = "DONOR_";

      if (donorCounter < 10)
        donorID += "00";
      else if (donorCounter < 100)
        donorID += "0";

      donorID += String(donorCounter++);

      Serial.println(donorID);

      delay(300);

      digitalWrite(2, LOW);
    }
  }
}
```

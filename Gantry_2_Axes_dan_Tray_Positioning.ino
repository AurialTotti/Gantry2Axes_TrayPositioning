#include <AccelStepper.h> //Library motor stepper

//Define pin
#define echoPin1 50 //pin echo ultrasonik 1
#define trigPin1 51 //pin trigger ultrasonik 1
#define echoPin2 52 //pin echo ultrasonik 2
#define trigPin2 53 //pin trigger ultrasonik 2
#define Pompa1 22   //Pompa
#define Pompa2 23

#define mode1 3    //Pilihan mode
#define mode2 4
#define mode3 5
#define mode4 6
#define mode5 7
#define Lamp1 26
#define Lamp2 27
#define Lamp3 28
#define Lamp4 29

#define VRX_PIN A0  //Joystick
#define VRY_PIN A1
#define SW_PIN 2

#define stepPin1 8  //Motor Stepper Y1
#define dirPin1 9
#define LS1 42
#define LS1_2 43
AccelStepper stepper1 = AccelStepper(1, stepPin1, dirPin1);
#define stepPin2 10  //Motor Stepper Y2
#define dirPin2 11
#define LS2 44
#define LS2_2 45
AccelStepper stepper2 = AccelStepper(1, stepPin2, dirPin2);
#define stepPin3 12  //Motor Stepper Z
#define dirPin3 13
#define LS3 46
#define LS3_2 47
AccelStepper stepper3 = AccelStepper(1, stepPin3, dirPin3);
#define En1 14
#define En2 15

//Variabel
long durasi1; //Durasi ultrasonik 1
long durasi2; //Durasi ultrasonik 2
int jarak1;   //Jarak benda 1
int jarak2;   //Jarak benda 2

int SW = 1;   //Push Button joystick

int step1 = -1; //Posisi Motor Stepper Y1
int step2 = -1; //Posisi Motor Stepper Y2
int step3 = -1; //Posisi Motor Stepper Z
int kecepatanY = 800;
int kecepatanZ = 800;
int menu = 0;

int pilihan = 1;  //Pilihan mode

long startUltra = 0;  //Multitask
long durasiUltra = 500;

void setup() {
  pinMode(mode1, INPUT_PULLUP); //PULLUP = anti floating
  pinMode(mode2, INPUT_PULLUP);
  pinMode(mode3, INPUT_PULLUP);
  pinMode(mode4, INPUT_PULLUP);
  pinMode(mode5, INPUT_PULLUP);
  pinMode(Lamp1, OUTPUT);
  pinMode(Lamp2, OUTPUT);
  pinMode(Lamp3, OUTPUT);
  pinMode(Lamp4, OUTPUT);
  pinMode(En1, OUTPUT);
  pinMode(En2, OUTPUT);
  
  pinMode(SW_PIN, INPUT_PULLUP);  //Joystick
   
  pinMode(Pompa1, OUTPUT);  //Pompa
  pinMode(Pompa2, OUTPUT);
  
  pinMode(echoPin1, INPUT); //Ultrasonik
  pinMode(trigPin1, OUTPUT); 
  pinMode(echoPin2, INPUT);
  pinMode(trigPin2, OUTPUT);
 
  stepper1.setMaxSpeed(1000); //Motor Stepper 1000step/s
  stepper2.setMaxSpeed(1000);
  stepper3.setMaxSpeed(1000);
  pinMode(LS1, INPUT_PULLUP);
  pinMode(LS2, INPUT_PULLUP);
  pinMode(LS3, INPUT_PULLUP);
  pinMode(LS1_2, INPUT_PULLUP);
  pinMode(LS3_2, INPUT_PULLUP);
  pinMode(LS2_2, INPUT_PULLUP);
  homing2();  //ke posisi awal
  homing1();
   
  Serial.begin(9600);
}

void loop() {
  while(pilihan == 1){
    digitalWrite(En1, HIGH);
    digitalWrite(En2, HIGH);
    digitalWrite(Lamp1, LOW);
    digitalWrite(Lamp2, HIGH);
    digitalWrite(Lamp3, HIGH);
    digitalWrite(Lamp4, HIGH);
    atur_tray();
    pilih();
  } while(pilihan == 2){
    digitalWrite(En1, LOW);
    digitalWrite(En2, LOW);
    digitalWrite(Lamp2, LOW);
    digitalWrite(Lamp1, HIGH);
    digitalWrite(Lamp3, HIGH);
    digitalWrite(Lamp4, HIGH);
    pasang_tray();
  } while(pilihan == 3){
    digitalWrite(En1, HIGH);
    digitalWrite(En2, HIGH);
    digitalWrite(Lamp3, LOW);
    digitalWrite(Lamp1, HIGH);
    digitalWrite(Lamp2, HIGH);
    digitalWrite(Lamp4, HIGH);
    panen();
    pilih();
  } while(pilihan == 4){
    Serial.println("menu 4");
    digitalWrite(Lamp4, LOW);
    digitalWrite(Lamp1, HIGH);
    digitalWrite(Lamp2, HIGH);
    digitalWrite(Lamp3, HIGH);
    manual();
    pilih();
  } while(pilihan == 5){
    digitalWrite(En1, HIGH);
    digitalWrite(En2, HIGH);
    digitalWrite(Lamp1, HIGH);
    digitalWrite(Lamp2, HIGH);
    digitalWrite(Lamp3, HIGH);
    digitalWrite(Lamp4, HIGH);
    digitalWrite(Pompa1, HIGH);
    digitalWrite(Pompa2, HIGH);
    pilih();
  }
}

//Mode pasang tray
void pasang_tray(){
  while(menu == 0){
    Serial.println("menu 0");
    kecepatanY = 1000; kecepatanZ = 1000;
    homing2();
    homing1();
    menu = 1;
    delay(500);
    ngeprint();
  }
  while(menu == 1){ //persiapan ambil tray
    Serial.println("menu 1");
    step3 = -7500; kecepatanZ = -1000; //300mm
    motorZ();
    ngeprint();
    menu = 2;
    delay(500);
  }
  while(menu == 2){ //motor Y maju ke conveyor
    Serial.println("menu 2");
    step1 = -3750; step2 = -3750;     //150mm
    kecepatanY = -1000;
    motorY();
    ngeprint();
    menu = 3;
    delay(500);
  }
  while(menu == 3){ //motor Z ambil tray
    Serial.println("menu 3");
    step3 = -3750; kecepatanZ = 1000;  //150mm
    motorZ();
    ngeprint();
    menu = 4;
    delay(500);
  }
  while(menu == 4){ //motor Y ke kolam
    Serial.println("menu 4");
    step1 = -11500; step2 = -11500;   //460mm
    kecepatanY = -1000;
    motorY();
    ngeprint();
    if(millis()-startUltra > durasiUltra){
      ultrasonik1();
      startUltra = millis();
    }
    if(jarak1 <= 250){
      digitalWrite(Pompa1, LOW);  //nyala
    } else if(jarak1 > 250){
      digitalWrite(Pompa1, HIGH);
      menu = 5;
    }
    delay(500);
  }
  while(menu == 5){ //motor Z naruh tray
    Serial.println("menu 5");
    step3 = -9000; kecepatanZ = -1000; //360mm
    motorZ();
    ngeprint();
    if(millis()-startUltra > durasiUltra){
      ultrasonik1();
      startUltra = millis();
    }
    if(jarak1 <= 250){
      digitalWrite(Pompa1, LOW);  //nyala
    } else if(jarak1 > 250){
      digitalWrite(Pompa1, HIGH);
      menu = 6;
    }
    delay(500);
  }
  while(menu == 6){ //homing lagi
    kecepatanY = 1000; kecepatanZ = 1000;
    homing2();
    homing1();
    ngeprint();
    menu = 7;
    delay(500);
  }
  if(menu == 7){
    pilihan = 1;
  }
}
//Akhir mode pasang tray

void atur_tray(){
  if(millis()-startUltra > durasiUltra){
    ultrasonik1();
    ultrasonik2();
    startUltra = millis();
  }

  if(jarak1 <= 245){
    digitalWrite(Pompa1, LOW);
  } else if(jarak1 > 255){
    digitalWrite(Pompa1, HIGH);
  }
  if(jarak2 <= 95){
    digitalWrite(Pompa2, LOW);
  } else if(jarak2 > 105){
    digitalWrite(Pompa2, HIGH);
  }
}

//Sensor ultrasonik
void ultrasonik1(){
  digitalWrite(trigPin1, LOW);     //reset trigger
  delayMicroseconds(2);
  digitalWrite(trigPin1, HIGH);    //mengirimkan pulsa
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  
  durasi1 = pulseIn(echoPin1, HIGH);//baca durasi pulsa
  jarak1 = (durasi1 * 0.343 / 2) + 5;     //hitung jarak dengan mengkalikan durasi dengan 0.343mm/us atau 0.0343cm/us kemudian dibagi 2 (Sensor baru membaca mulai dari 5mm dari speaker)
  
  Serial.print("Jarak 1: ");
  Serial.print(jarak1);
  Serial.println(" mm");
}
  
void ultrasonik2(){
  digitalWrite(trigPin2, LOW);     //reset trigger
  delayMicroseconds(2);
  digitalWrite(trigPin2, HIGH);    //mengirimkan pulsa
  delayMicroseconds(10);
  digitalWrite(trigPin2, LOW);
  
  durasi2 = pulseIn(echoPin2, HIGH);//baca durasi pulsa
  jarak2 = (durasi2 * 0.343 / 2) + 5;     //hitung jarak dengan mengkalikan durasi dengan 0.343mm/us atau 0.0343cm/us kemudian dibagi 2
  
  Serial.print("Jarak 2: ");
  Serial.print(jarak2);
  Serial.println(" mm");
}
//Akhir sensor ultrasonik

//Mode panen
  void panen(){
    digitalWrite(Pompa2, HIGH);
    digitalWrite(Pompa1, LOW);
  }
//Akhir mode panen

//Pilih mode
  void pilih(){
    delay(100);
    if(digitalRead(mode1) == LOW){
      pilihan = 1;
    } else if(digitalRead(mode2) == LOW){
      menu = 0;
      pilihan = 2;     
    } else if(digitalRead(mode3) == LOW){
      pilihan = 3;      
    } else if(digitalRead(mode4) == LOW){
      pilihan = 4;      
    } else if(digitalRead(mode5) == LOW){
      pilihan = 5;     
    }
  }
//Akhir pilih mode

//MotorStepper
  //Void Homing
    void homing1(){
      while(digitalRead(LS1) == HIGH && digitalRead(LS2) == HIGH){ 
        Motor1();
        Motor2();
      }
      //Preventif Motor
      while(digitalRead(LS1) == HIGH){ 
        Motor1();
      }
      while(digitalRead(LS2) == HIGH){ 
        Motor2();
      }
      while(digitalRead(LS1) == HIGH){ 
        Motor1();
      }
      step1 = 0;
      step2 = 0;
      stepper1.setCurrentPosition(0);
      stepper2.setCurrentPosition(0);
    }
    
    //Preventif Motor 1
    void Motor1(){
      stepper1.moveTo(step1);  // Set the position to move to
      step1++;  // Decrease by 1 for next move if needed
      stepper1.setSpeed(kecepatanY);
      stepper1.runSpeed();
    }
    //Preventif Motor 2
    void Motor2(){
      stepper2.moveTo(step2);  // Set the position to move to
      step2++;  // Decrease by 1 for next move if needed
      stepper2.setSpeed(kecepatanY);
      stepper2.runSpeed();
    }
    
    void homing2(){
      while(digitalRead(LS3) == HIGH) {  
        stepper3.moveTo(step3);  // Set the position to move to
        step3++;  // Decrease by 1 for next move if needed
        stepper3.setSpeed(kecepatanZ);
        stepper3.runSpeed();
      }
      step3 = 0;
      stepper3.setCurrentPosition(0);
    }
  //Akhir Void Homing

  //Void gerak motor Y
    void motorY(){
      while(stepper1.currentPosition() != step1 && stepper1.currentPosition() != step2){
        stepper1.setSpeed(kecepatanY);
        stepper1.runSpeed();
        stepper2.setSpeed(kecepatanY);
        stepper2.runSpeed();
      }
    }
  //Akhir Void gerak motor Y
  
  //Void gerak motor Z
    void motorZ(){
      while(stepper3.currentPosition() != step3){
        stepper3.setSpeed(kecepatanZ);
        stepper3.runSpeed();
      }
    }
  //Akhir Void gerak motor Z
  
  //Print ke Serial
    void ngeprint(){
      Serial.print("Step 1 : ");
      Serial.print(step1/25*-1);   //1mm = 25 step
      Serial.print("mm, Step 2 : ");
      Serial.print(step2/25*-1);
      Serial.print("mm, Step 3 : ");
      Serial.print(step3/25*-1);
      Serial.println("mm");
    }
  //Akhir print ke serial
//Akhir MotorStepper

//Mode manual
void manual(){
  digitalWrite(En1, HIGH);
  digitalWrite(En2, HIGH);
  
  if(SW > 3){
    SW = 1;
  } else if(!digitalRead(SW_PIN)){
    SW++;
    delay(200);
  } else if(SW == 1){   //low speed
    kecepatanY = 600;
    kecepatanZ = 600;
  } else if(SW == 2){   //medium speed
    kecepatanY = 800;
    kecepatanZ = 800;
  } else if (SW == 3){  //high speed
    kecepatanY = 1000;
    kecepatanZ = 1000;
  }

  if(digitalRead(LS1) == LOW || digitalRead(LS2) == LOW){
    step1 = 0;
    step2 = 0;
  }
  if(digitalRead(LS3) == LOW){
    step3 = 0;
  }

  while(analogRead(VRX_PIN) < 400 && digitalRead(LS1) == HIGH && digitalRead(LS2) == HIGH){
    digitalWrite(En1, LOW);
    stepper1.moveTo(step1);  // Set the position to move to
    step1++;  // Decrease by 1 for next move if needed
    stepper1.setSpeed(kecepatanY);
    stepper1.runSpeed();
    stepper2.moveTo(step2);  // Set the position to move to
    step2++;  // Decrease by 1 for next move if needed
    stepper2.setSpeed(kecepatanY);
    stepper2.runSpeed();
  }
  while(analogRead(VRX_PIN) > 600 && digitalRead(LS1_2) == HIGH && digitalRead(LS2_2) == HIGH){
    digitalWrite(En1, LOW);
    stepper1.moveTo(step1);  // Set the position to move to
    step1--;  // Decrease by 1 for next move if needed
    stepper1.setSpeed(kecepatanY*-1);
    stepper1.runSpeed();
    stepper2.moveTo(step2);  // Set the position to move to
    step2--;  // Decrease by 1 for next move if needed
    stepper2.setSpeed(kecepatanY*-1);
    stepper2.runSpeed();
  }

  //move Z
  while(analogRead(VRY_PIN) < 400 && digitalRead(LS3) == HIGH){
    digitalWrite(En2, LOW);
    stepper3.moveTo(step3);  // Set the position to move to
    step3++;  // Decrease by 1 for next move if needed
    stepper3.setSpeed(kecepatanZ);
    stepper3.runSpeed();
  }
  while(analogRead(VRY_PIN) > 600 && digitalRead(LS3_2) == HIGH){
    digitalWrite(En2, LOW);
    stepper3.moveTo(step3);  // Set the position to move to
    step3--;  // Decrease by 1 for next move if needed
    stepper3.setSpeed(kecepatanZ*-1);
    stepper3.runSpeed();
  }

  atur_tray();
  ngeprint_manual();
}

void ngeprint_manual(){
  if(kecepatanY <= 600 && kecepatanZ <= 600){
    Serial.print("Step 1 : ");
    Serial.print(step1/25*-1/2.93);   //1mm = 25 step
    Serial.print("mm, Step 2 : ");
    Serial.print(step2/25*-1/2.93);
    Serial.print("mm, Step 3 : ");
    Serial.print(step3/25*-1/5);
    Serial.println("mm");
  }
  if(kecepatanY > 600 && kecepatanZ > 600){
    Serial.print("Step 1 : ");
    Serial.print(step1/25*-1/2);   //1mm = 25 step
    Serial.print("mm, Step 2 : ");
    Serial.print(step2/25*-1/2);
    Serial.print("mm, Step 3 : ");
    Serial.print(step3/25*-1/4);
    Serial.println("mm");
  }
}
//Akhir mode manual

#include <Wire.h>
#include <LiquidCrystal_I2C.h> //library lcd
#include <Adafruit_ADS1X15.h> //library eksternal adc

// alamat lcd 0x27
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_ADS1115 ads;  //tipe ads 16 bit

#define buzzer 13 //pin buzzer

int initADC;

void setup() {
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);
  lcd.begin();
  lcd.setCursor(0,0);
  lcd.print("  ALAT DETEKSI  ");
  lcd.setCursor(0,1);
  lcd.print("   KOLESTEROL   ");
  delay(500);
  lcd.setCursor(0,1);
  lcd.print("   ASAM  URAT   ");
  ads.begin(0x48);   //alamat i2c ads
  delay(1000);
  initADC = 8 + ads.readADC_SingleEnded(0); //membaca adc pin 0 untuk inisiasi nilai toleransi start
}

void loop() {
  if (ads.readADC_SingleEnded(1)>=1000) { //mendeteksi apakah strip terpasang
    lcd.setCursor(0, 0);
    lcd.print("  Ready To Use  ");
    lcd.setCursor(0, 1);
    lcd.print("Masukan Strip !!");
  }
  else if (ads.readADC_SingleEnded(1)<=50) {  //jika dimasukan strip guladarah maka menampilkan tidak didukung
    lcd.setCursor(0, 0);
    lcd.print("      Strip     ");
    lcd.setCursor(0, 1);
    lcd.print(" Tidak Didukung ");
  }
  else if (ads.readADC_SingleEnded(1)<=190) { //mendeteksi strip kolesterol
    chol();
  }
  else if (ads.readADC_SingleEnded(1)<=380) { //mendeteksi strip asam urat
    uric();
  }
  Serial.print("ADC tipe Strip : "); Serial.println(ads.readADC_SingleEnded(1));
}

void chol(){  //pengambilan data untuk kolesterol
  int cholADC[26]={0};
  unsigned long cholADD=0;
  int cholAVG;
  float hasilChol;
  if (ads.readADC_SingleEnded(0) >= initADC) {
    beep();
    lcd.clear();
    for(int i=26 ; i>=1 ; i--){         //sampling 26 kali selama 26 detik
      cholADC[i]= ads.readADC_SingleEnded(0);
      cholADD += cholADC[i];
      lcd.setCursor(0, 0);
      lcd.print(" MEMPROSES DATA ");
      lcd.setCursor(7, 1);
      lcd.print("  ");
      lcd.setCursor(7, 1);
      lcd.print(i);
      delay(975);
    }
    cholAVG=cholADD/26;
    hasilChol=(cholAVG-1925.3)/0.8711;//y = 0,8711x + 1925,3
    tampilHasil("KOLESTEROL", hasilChol, "mg/dL"); //memanggil fungsi display
    if(hasilChol>=200.0){ //jika sama atau lebih dari 200, buzzer nyala dua kali
      beepWarning();
      lcd.setCursor(0,0);
      lcd.print("===KOLESTEROL===");
      lcd.setCursor(0,1);
      lcd.print(" KADAR TINGGI ! ");
    }
    else if (hasilChol<200.0) { //jika kurang dari 200 buzzer nyala sekali
      beep();
    }
  }
  else{
    lcd.setCursor(0,0);
    lcd.print("===KOLESTEROL===");
    lcd.setCursor(0,1);
    lcd.print(" Masukan Sample ");
  }
}

void uric(){ //pengambilan data asam urat
  int uricADC[15]={0};
  unsigned long uricADD=0;
  int uricAVG;
  float hasilUric;
  if (ads.readADC_SingleEnded(0) >= initADC) {
    beep();
    lcd.clear();
    for(int i=15 ; i>=1 ; i--){              //sampling 15 kali selama 15 detik
      uricADC[i]= ads.readADC_SingleEnded(0);
      uricADD += uricADC[i];
      lcd.setCursor(0, 0);
      lcd.print(" MEMPROSES DATA ");
      lcd.setCursor(7, 1);
      lcd.print("  ");
      lcd.setCursor(7, 1);
      lcd.print(i);
      delay(975);
    }
    uricAVG=uricADD/15;
    hasilUric = (uricAVG-1764.7)/32.538;
    tampilHasil("ASAM  URAT", hasilUric, "mg/dL"); //memanggil fungsi display
    if(hasilUric>=7.0){ //jika sama atau lebih dari 200, buzzer nyala dua kali
      beepWarning();
      lcd.setCursor(0,0);
      lcd.print("===ASAM__URAT===");
      lcd.setCursor(0,1);
      lcd.print(" KADAR TINGGI ! ");
    }
    else if (hasilUric<7.0) { //jika kurang dari 200 buzzer nyala sekali
      beep();
    }
  }
  else{
    lcd.setCursor(0,0);
    lcd.print("===ASAM__URAT===");
    lcd.setCursor(0,1);
    lcd.print(" Masukan Sample ");
  }
}

void tampilHasil(const char* mode, float hasil, const char* satuan){ //fungsi untuk display hasil
  lcd.clear();
  while ( hasil<10000 && ads.readADC_SingleEnded(1)<=1000) { //hold tampilan hasil selama strip tidak dganti
    lcd.setCursor(3, 0);
    lcd.print(mode);
    lcd.setCursor(1, 1);
    lcd.print(hasil,2);
    lcd.setCursor(11, 1);
    lcd.print(satuan);
  }
}

void beep(){ //untuk menyalakan buzzer
  digitalWrite(buzzer, HIGH);
  delay(60);
  digitalWrite(buzzer, LOW);
}

void beepWarning(){
  digitalWrite(buzzer, HIGH);
  delay(60);
  digitalWrite(buzzer, LOW);
  delay(100);
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
}
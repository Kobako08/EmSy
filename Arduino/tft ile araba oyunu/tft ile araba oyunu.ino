/*
 ST7735 ve arduino esplora kullanılmıştır. Araç görünümlü bir objenin ekranda akışı ve farklı bir obje ile çarptığında puan yazılması amaçlanmıştır.
  
  
 created 10 November 2015
 by Şemsidin YILMAZ
 */

#include <Esplora.h>// Esplora hazır fonksiyonlarını kullanmak için
#include <Adafruit_GFX.h>// Çekirdek grafik kütüphanemizi dahil edelim
#include <Adafruit_ST7735.h>// Esplora LCD'sinin sürücüsünü dahil edelim
#include <SPI.h>// Esplora LCD'sini SPI haberleşme kullanarak daha hızlı
// kullanmak için SPI.h dosyasını dahil edelim

// Esplora LCD'sinin pin bağlantılarını ayarlıyoruz
#define sclk 15 // Arduino'nun 15. pini: SCK clock pini
#define mosi 16 // Arduino'nun 16. pini: Master Output Slave Input (MOSI) data pini
#define cs   7
#define dc   0
#define rst  1

// RGB565CONVERT fonksiyonu 32 bit rgb renk kodunu 16 bit rgb renk koduna cevirir
// Photoshop, Adobe Fireworks gibi tasarım programları 32bit rgb renk kodları ile çalışır.
// Parlak kırmızı renk kodu bilgisayarda #FF3300 iken, bunu 16 bit renk koduna çevirmek
// için RGB565CONVERT fonksiyonundan geçirmeliyiz: RGB565CONVERT(0xFF, 0x33, 0x00)

#define RGB565CONVERT(red, green, blue)\
  (uint16_t)((( red   >> 3 )<< 11 )| \
             (( green >> 2 )<< 5  )| \
             ( blue  >> 3 ))
             
             //projede kullandığımız gerekli değişkenlerimizi oluşturuyoruz
int serit = 0;
float mutlakX ;
float mutlakY ;
int rakip1 = 0;
bool kaza = false;
int rakip2 = 0;
int rakip1s = 0;
long skor = 0;
int rakip2s = 0;
long randSerit; 
long randNumber;

//renkleri define ediyoruz
#define  siyah    0// siyah
#define  gri      RGB565CONVERT(0xCC, 0xCA, 0xCF)//gri
#define  koyu_gri RGB565CONVERT(127, 127, 135)//koyu gri
#define  yesil    RGB565CONVERT(0x66, 0xcc, 0x00)// parlak yesil
#define  kirmizi  RGB565CONVERT(0xFF, 0x33, 0x00)// parlak kırmızı
#define  sari     RGB565CONVERT(0xFF, 0xFF, 0x00)// sari

//SPI donanım pinlerini kullanarak LCD ile haberleşelim.
// İşte bu LCD'yi kullanmanın en hızlı yolu!

Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, rst);

void setup() {
  tft.initR(INITR_BLACKTAB);//tft özellikleri tanımlanıyor
  
  tft.setRotation(1);// LCD'yi yatay kullan
  tft.fillScreen(siyah);//ilk ekranı siyaha boyuyoruz

  tft.setCursor(1, 1);   // imleci X:1, Y:1 koordinatlarına getir
  tft.setTextSize(2);     // yazı boyutunu 2 yap
  tft.println("Baslamak icinasagi yon    tusunu       kullaniniz.");   // Önceden ayarladığımız koordinatlara başlamak için gerekli yazıyı yaz
  while (Esplora.readButton(SWITCH_DOWN) == true) {// alt tuşa basana kadar bekle
    
  }
  tft.fillScreen(siyah);//tuşa bastıktan sonra ekranı siyaha boya
  tft.fillRoundRect(20, 0, 5, 130, 0, gri);//sol yol bitim şeridi çiziliyor
  tft.fillRoundRect(135, 0, 5, 130, 0, gri);//sağ yol bitim şeridi çiziliyor
  randomSeed(analogRead(0));//rasgele sayı seçimi için random sistemi çalıştırılıyor
}

void loop() {
  int tilt = Esplora.readAccelerometer(X_AXIS);//arduinonun accelerometre bilgisi okunup tilt değişkenine atılıyor
  int aracx = map(tilt, -150, 150, 110, 35);//accelemetreden okuduğumuz değeri  gerekli olan değer aralığına geçiriyoruz haritalandırma yöntemiyle

  if (rakip2 == 280)// rakip iki yolun sonuna geldi ise
  {
    rakip2s = random(2);//sağ yada sol şeritte olduğunu rast gele seç
    rakip2 = 100;//yolun başına koy aracı
  }
  if (rakip1 == 180)//rakip1 yolun sonuna geldi ise
  {
    rakip1 = 0;//rakip 1 yolun başına al
    rakip1s = random(2);//hangi şeritte olucağını rast gele seç
  }
  if (serit == 45)//akan şerit yolun sonuna geldi ise
  {
    serit = 0;//şeridi başa al
  }
  serit = serit + 9;//şeride +9 kaydırarak akışı sağla
  skor = skor + 7;//her ilerlemeden sonra skor hanesine 7 puan ekle

//şerit çizgilerini çiz akış için şerit bilgilerinin y sine şerit değişkenindeki sayıyı ekle
  tft.fillRoundRect(73, 0 + serit - 40, 10, 30, 5, gri);
  tft.fillRoundRect(73, 0 + serit, 10, 30, 5, gri);
  tft.fillRoundRect(73, 40 + serit, 10, 30, 5, gri);
  tft.fillRoundRect(73, 80 + serit, 10, 30, 5, gri);
  tft.fillRoundRect(73, 120 + serit, 10, 30, 5, gri);

//arabamızı çiz x değeri yerine accelemetreden okunan ve haritalandırma metodu ile istediğimiz değerler aralığına indirilen değeri yaz
  tft.fillRoundRect(aracx, 80, 20, 40, 5, yesil);
  tft.fillRoundRect(aracx + 3, 80 + 5, 14, 10, 0, koyu_gri);
  tft.fillRoundRect(aracx + 3, 80 + 31, 14, 7, 0, koyu_gri);
  tft.fillRoundRect(aracx + 5, 80 + 16, 10, 14, 0, kirmizi);
  tft.fillRoundRect(aracx + 1, 80 + 16, 3, 6, 0, koyu_gri);
  tft.fillRoundRect(aracx + 1, 80 + 24, 3, 6, 0, koyu_gri);
  tft.fillRoundRect(aracx + 16, 80 + 16, 3, 6, 0, koyu_gri);
  tft.fillRoundRect(aracx + 16, 80 + 24, 3, 6, 0, koyu_gri);
  tft.fillRect(aracx + 4, 80, 4, 2, sari);
  tft.fillRect(aracx + 12, 80, 4, 2, sari);
  
  rakip1 = rakip1 + 5;//rakip1i hareket ettirmek için değerini yükselt
  rakip2 = rakip2 + 5;//rakip2 yi hareket ettirmek için değerini yükselt
  if (rakip1s == 1)//rakip1 için randum seçilen şerit sol şeritse
  {
    //aracı çiz aracın ysinin yerine rakip1 değişkeninin içindeki değerimizi ekle
    tft.fillRoundRect(45, -40 + rakip1, 20, 40, 5, gri);
    tft.fillRoundRect(45 + 3, -40 + rakip1 + 5, 14, 10, 0, koyu_gri);
    tft.fillRoundRect(45 + 3, -40 + rakip1 + 31, 14, 7, 0, koyu_gri);
    tft.fillRoundRect(45 + 5, -40 + rakip1 + 16, 10, 14, 0, kirmizi);
    tft.fillRoundRect(45 + 1, -40 + rakip1 + 16, 3, 6, 0, koyu_gri);
    tft.fillRoundRect(45 + 1, -40 + rakip1 + 24, 3, 6, 0, koyu_gri);
    tft.fillRoundRect(45 + 16, -40 + rakip1 + 16, 3, 6, 0, koyu_gri);
    tft.fillRoundRect(45 + 16, -40 + rakip1 + 24, 3, 6, 0, koyu_gri);
    tft.fillRect(45 + 4, -40 + rakip1, 4, 2, sari);
    tft.fillRect(45 + 12, -40 + rakip1, 4, 2, sari);
    //kaza olup olmadığını sorgula
    mutlakX = abs((aracx + (20 / 2)) - (45 + (20 / 2)));
    mutlakY = abs((80 + (40 / 2)) - (-40 + rakip1 + (40 / 2)));
    if ((20 > mutlakX) && (40 > mutlakY))//eğer yüksekliği mutlak y sinden büyük iken genişliği mutlak xinden büyük ise
    {
      kaza = true;//kaza durumunu aktif et
    }
  }
  else//rakip1 için randum seçilen şerit sol şerit değil ise
  {
    //aracı çiz
    tft.fillRoundRect(90, -40 + rakip1, 20, 40, 5, gri);
    tft.fillRoundRect(90 + 3, -40 + rakip1 + 5, 14, 10, 0, koyu_gri);
    tft.fillRoundRect(90 + 3, -40 + rakip1 + 31, 14, 7, 0, koyu_gri);
    tft.fillRoundRect(90 + 5, -40 + rakip1 + 16, 10, 14, 0, kirmizi);
    tft.fillRoundRect(90 + 1, -40 + rakip1 + 16, 3, 6, 0, koyu_gri);
    tft.fillRoundRect(90 + 1, -40 + rakip1 + 24, 3, 6, 0, koyu_gri);
    tft.fillRoundRect(90 + 16, -40 + rakip1 + 16, 3, 6, 0, koyu_gri);
    tft.fillRoundRect(90 + 16, -40 + rakip1 + 24, 3, 6, 0, koyu_gri);
    tft.fillRect(90 + 4, -40 + rakip1, 4, 2, sari);
    tft.fillRect(90 + 12, -40 + rakip1, 4, 2, sari);
    //kaza durumunu sorgula
    mutlakX = abs((aracx + (20 / 2)) - (90 + (20 / 2)));
    mutlakY = abs((80 + (40 / 2)) - (-40 + rakip1 + (40 / 2)));
    if ((20 > mutlakX) && (40 > mutlakY))//eğer yüksekliği mutlak y sinden büyük iken genişliği mutlak xinden büyük ise
    {
      kaza = true;//kazayı aktir et
    }
  }
  if (rakip2s == 1)
  {
    tft.fillRoundRect(45, -150 + rakip2, 20, 40, 5, gri);
    tft.fillRoundRect(45 + 3, -150 + rakip2 + 5, 14, 10, 0, koyu_gri);
    tft.fillRoundRect(45 + 3, -150 + rakip2 + 31, 14, 7, 0, koyu_gri);
    tft.fillRoundRect(45 + 5, -150 + rakip2 + 16, 10, 14, 0, yesil);
    tft.fillRoundRect(45 + 1, -150 + rakip2 + 16, 3, 6, 0, koyu_gri);
    tft.fillRoundRect(45 + 1, -150 + rakip2 + 24, 3, 6, 0, koyu_gri);
    tft.fillRoundRect(45 + 16, -150 + rakip2 + 16, 3, 6, 0, koyu_gri);
    tft.fillRoundRect(45 + 16, -150 + rakip2 + 24, 3, 6, 0, koyu_gri);
    
    tft.fillRect(45 + 4, -150 + rakip2, 4, 2, sari);
    tft.fillRect(45 + 12, -150 + rakip2, 4, 2, sari);
    mutlakX = abs((aracx + (20 / 2)) - (45 + (20 / 2)));
    mutlakY = abs((80 + (40 / 2)) - (-150 + rakip2 + (40 / 2)));
    if ((20 > mutlakX) && (40 > mutlakY))//eğer yüksekliği mutlak y sinden büyük iken genişliği mutlak xinden büyük ise
    {
      kaza = true;
    }
  }
  else
  {
    tft.fillRoundRect(90, -150 + rakip2, 20, 40, 5, gri);
    tft.fillRoundRect(90 + 3, -150 + rakip2 + 5, 14, 10, 0, koyu_gri);
    tft.fillRoundRect(90 + 3, -150 + rakip2 + 31, 14, 7, 0, koyu_gri);
    tft.fillRoundRect(90 + 5, -150 + rakip2 + 16, 10, 14, 0, yesil);
    tft.fillRoundRect(90 + 1, -150 + rakip2 + 16, 3, 6, 0, koyu_gri);
    tft.fillRoundRect(90 + 1, -150 + rakip2 + 24, 3, 6, 0, koyu_gri);
    tft.fillRoundRect(90 + 16, -150 + rakip2 + 16, 3, 6, 0, koyu_gri);
    tft.fillRoundRect(90 + 16, -150 + rakip2 + 24, 3, 6, 0, koyu_gri);
    
    tft.fillRect(90 + 4, -150 + rakip2, 4, 2, sari);
    tft.fillRect(90 + 12, -150 + rakip2, 4, 2, sari);
    mutlakX = abs((aracx + 10) - (90 + 10));
    mutlakY = abs((80 + 20) - (-150 + rakip2 + 20));
    if ((20 > mutlakX) && (40 > mutlakY))//eğer yüksekliği mutlak y sinden büyük iken genişliği mutlak xinden büyük ise
    {
      kaza = true;
    }
  }
  if (kaza == true)//herhangi bir araç ile kaza yapılmış ise
  {
    tft.setCursor(1, 5);   // imleci X:1, Y:5 koordinatlarına getiz
    tft.println("Kaza yaptiniz");
    tft.setCursor(55, 35);   // imleci X:30, Y:30 koordinatlarına getiz
    tft.println("SKOR");
    int b = 1;//b değişkenini 1 yap
    int skorYeri = 0;//skor yerini sufurla
    while ((skor / b) > 9) {//skorun kaç basamaklı olduğunu anlamak için while döngüsü

      b = b * 10;

      skorYeri++;

    }
    skorYeri = skorYeri * 6;//basamak sayısına göresayıyı yazıcağımız yeri kaydırıyoruz ki ekranın ortasında kalsın yazı
    tft.setCursor(72 - skorYeri, 60); // imleci Xi ortalıyoruz, Y:60 koordinatlarına getiz
    tft.println(skor);//skor bilgisini yazıyoruz
    while (Esplora.readButton(SWITCH_DOWN) == true) ;//eğer aşağı yön tuşuna basılır ise
    tft.fillScreen(siyah);//ekranı siyaha boyuyoruz
    rakip2 = 0;//rakip2 bilgilerini 0lıyoruz
    rakip1 = 0;//rakip1 bilgilerini sıfırlıyoruz
    tft.fillRoundRect(20, 0, 5, 130, 0, gri);//sol şeriti çiziyoruz
    tft.fillRoundRect(135, 0, 5, 130, 0, gri);//sağ şeridi çiziyoruz
    kaza = false;//kaza durumunu kapatıyoruz
    skor=0;//skoru sıfırlıyoruz
  }
  delay(100);//100milisaniye bekletiyoruz
  
  tft.fillRoundRect(26, 0, 108, 130, 0, siyah);// LCD'yi siyah renkle doldur
}

//#define KT_TEST





/*
   Không chỉnh sửa gì nhé mọi người

//////////////// Quy trình kiểm tra Vietduino Uno///////////////////////////////////////////////////////
B1: Kiểm tra nguồn (5V, 3.3V), chập chân (CP2102), bảo vệ USB (đo áp chỗ cầu chì), hở chân LK         //
B2: Hàn rào-đế-giắc DC                                                                                //
B3: Gắn IC đã Burn BootLoader - nạp CT test                                                           //
B4: Rửa - Vệ sinh                                                                                     //
B5: Kiểm tra lại nút reset (sau khi vệ sinh bằng axetol)                                              //
////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////// Quy trình kiểm tra Vietduino Mega2560//////////////////////////////////////////////////
B1: Kiểm tra nguồn (5V, 3.3V), chập chân (CP2102), bảo vệ USB (đo áp chỗ cầu chì), hở chân LK         //
B2: Burn BootLoader - nạp CT test                                                                     //
B3: Hàn rào-đế-giắc DC                                                                                //
B4: Rửa - Vệ sinh                                                                                     //
B5: Kiểm tra lại nút reset (sau khi vệ sinh bằng axetol)                                              //
////////////////////////////////////////////////////////////////////////////////////////////////////////




























































































































































*/

/*
  Note:
  - 17/11/2019: Không code trong void setup và loop nữa, toàn bộ chuyển qua CREATE_FUNCTION
  - 26/2/2019: thêm vị trí chân Mega bị lỗi VD: 3.6 là chân số 89 (Mega chia làm 4 mặt, chân 1 được tính ở mặt một, số thập phân là vị trí mặt đó) => mặt 4, ở giữa.
*/
#include "Vietduino_NewFunction.h"

#define DBL(x)      Serial.println(x)

#define ID_KXN            String(1.0)

//#define ID_STORE        String(2.0)
//#define ID_ONLINE       String(3.0)

#if defined(ID_KXN)
#define QC_ID ID_KXN
#elif defined(ID_STORE)
#define QC_ID ID_STORE
#elif defined(ID_ONLINE)
#define QC_ID ID_ONLINE
#endif


#if   defined(ARDUINO_AVR_ADK)
#define BOARD "Mega Adk"
#elif defined(ARDUINO_AVR_BT)    // Bluetooth
#define BOARD "Bt"
#elif defined(ARDUINO_AVR_DUEMILANOVE)
#define BOARD "Duemilanove"
#elif defined(ARDUINO_AVR_ESPLORA)
#define BOARD "Esplora"
#elif defined(ARDUINO_AVR_ETHERNET)
#define BOARD "Ethernet"
#elif defined(ARDUINO_AVR_FIO)
#define BOARD "Fio"
#elif defined(ARDUINO_AVR_GEMMA)
#define BOARD "Gemma"
#elif defined(ARDUINO_AVR_LEONARDO)
#define BOARD "Leonardo"
#elif defined(ARDUINO_AVR_LILYPAD)
#define BOARD "Lilypad"
#elif defined(ARDUINO_AVR_LILYPAD_USB)
#define BOARD "Lilypad Usb"
#elif defined(ARDUINO_AVR_MEGA)
#define BOARD "Mega"
#elif defined(ARDUINO_AVR_MEGA2560)
#define BOARD "Mega 2560"
#elif defined(ARDUINO_AVR_MICRO)
#define BOARD "Micro"
#elif defined(ARDUINO_AVR_MINI)
#define BOARD "Mini"
#elif defined(ARDUINO_AVR_NANO)
#define BOARD "Nano"
#elif defined(ARDUINO_AVR_NG)
#define BOARD "NG"
#elif defined(ARDUINO_AVR_PRO)
#define BOARD "Pro"
#elif defined(ARDUINO_AVR_ROBOT_CONTROL)
#define BOARD "Robot Ctrl"
#elif defined(ARDUINO_AVR_ROBOT_MOTOR)
#define BOARD "Robot Motor"
#elif defined(ARDUINO_AVR_UNO)
#define BOARD "Uno"
#elif defined(ARDUINO_AVR_YUN)
#define BOARD "Yun"

// These boards must be installed separately:
#elif defined(ARDUINO_SAM_DUE)
#define BOARD "Due"
#elif defined(ARDUINO_SAMD_ZERO)
#define BOARD "Zero"
#elif defined(ARDUINO_ARC32_TOOLS)
#define BOARD "101"
#else
#error "Unknown board"
#endif


int Start_pin = 2;
int Stop_pin = NUM_DIGITAL_PINS - 1;

unsigned char default_pin_value = LOW;
bool b_complete_bit = false;
bool b_tested_low_bit = false;

typedef struct pin_info {
  unsigned char lastValue;
};

pin_info my_pin_array[NUM_DIGITAL_PINS];

#if NUM_DIGITAL_PINS > 68 // Mega có 70 chân
byte viTriLoi[70] {
  //             0          1         2         3         4         5         6         7         8         9
  2,         3,        6,        7,        1,        5,        15,       16,       17,       18,  //     0
  23,        24,       25,       26,       64,       63,       13,       12,       46,       45,    //   1
  44,        43,       78,       77,       76,       75,       74,       73,       72,       71,    //   2
  60,        59,       58,       57,       56,       55,       54,       53,       50,       70,    //   3
  52,        51,       42,       41,       40,       39,       38,       37,       36,       35,    //   4
  22,        21,       20,       19,       97,       96,       95,       94,       93,       92,    //   5
  91,        90,       89,       88,       87,       86,       85,       84,       83,       82,    //   6

};

String ErrcodeToStr(int _errCode_) {
  String tem_str_4 = String((float)viTriLoi[_errCode_] / 25, 1);

  return tem_str_4;
}
#endif

//String ErrcodeToStr(int _errCode_){
//  String tem_str_4 = String((float)viTriLoi[_errCode_]/25,1);
//
//  return tem_str_4;
//}

void setAllPinPullUp() {
  for (int cf = Start_pin; cf <= Stop_pin; cf++) {
    pinMode(cf, INPUT_PULLUP);
    my_pin_array[cf].lastValue = HIGH;
  }
}

CREATE_FUNCTION(ShowPinState) {
  static int cf;
  cf = 0;
  Serial.println(F("==========="));
  for (cf = Start_pin; cf <= Stop_pin; cf++) {
    if (my_pin_array[cf].lastValue == !default_pin_value) {
      continue;
    } else {
      int temValPin = digitalRead(cf);
      if (my_pin_array[cf].lastValue == default_pin_value) {
        Serial.print("Pin ");
        Serial.print(default_pin_value ? "HIGH: " : "LOW: ");
        if (Stop_pin > 50 && cf >= 54) {
          Serial.print("A" + String(cf - 54));
        } else if (Stop_pin < 50 && cf >= 14) {
          Serial.print("A" + String(cf - 14));
        } else Serial.print(cf);


#if NUM_DIGITAL_PINS > 68 // Mega có 70 chân
        Serial.print("\t");
        Serial.print(ErrcodeToStr(cf));
#endif

        Serial.println();
        M_DELAY(0);
      }
    }
  }
  M_DELAY(1000);
  ShowPinState.disable();

  END_CREATE_FUNCTION
}

CREATE_FUNCTION(blink_led_ok) {
  pinMode(13, OUTPUT);

  while (1) {
    int tempState = digitalRead(13);
    if (!b_tested_low_bit) digitalWrite(13, tempState ^ 1);
    else Serial.println(F("Vietduino Test wire done, all pins are OK!"));

    if (tempState == 0) {
      #ifndef KT_TEST
        Serial.println("Hshop tests Arduino lan 3 OK!!");
      #endif
    }
    M_DELAY(100);
  }
  END_CREATE_FUNCTION
}

#pragma region CREATE_FUNCTION(TestLow)
/*
  @ Kịch bản: Khi các chân đều LOW thì chương trình kết thúc. Nếu còn bất kỳ chân IO nào đang ở mức HIGH, chương trình sẽ show ra Serial các IO đó.
*/
CREATE_FUNCTION(TestLow) {
  Serial.println("Test I/O Low begin:");
  //Set default value of pin before the test begin!
  default_pin_value = HIGH;

  blink_led_ok.disable();
  setAllPinPullUp();
  M_DELAY(100);
  b_complete_bit = false;
  while (!b_complete_bit) {
    M_DELAY(0);

    static int cf;
    cf = 0;
    static bool flag_check_again = false;
    flag_check_again = false;

    for (cf = Start_pin; cf <= Stop_pin; cf++) {
      //      if(cf != 13){
      if (my_pin_array[cf].lastValue == !default_pin_value) {
        continue;
      } else {
        int temValPin = digitalRead(cf);
        b_complete_bit = true;
        if (temValPin == default_pin_value) {
          flag_check_again = true;
        } else if (temValPin == !default_pin_value) {
          my_pin_array[cf].lastValue = !default_pin_value;
        } else;
      }
      //      }
    }
    if (flag_check_again) {
      b_complete_bit = false;
    }

    if (!ShowPinState.available())ShowPinState.enable();
  }

  if (b_complete_bit)
    Serial.println(F("Vietduino Test wire done, all pins are OK!"));

  TestLow.disable();
  b_tested_low_bit = true;
  blink_led_ok.enable();
  END_CREATE_FUNCTION
}

#pragma endregion CREATE_FUNCTION(TestLow)

CREATE_FUNCTION(CheckSeria_command) {
  if (Serial.available()) {
    char kk = Serial.read();
    if (b_complete_bit) {
      if (!TestLow.available() && (!b_tested_low_bit)) {
        TestLow.enable();
      }
    }
  }

  END_CREATE_FUNCTION
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("There are " + String(NUM_DIGITAL_PINS) + " pin need check");
  Serial.println(F("Welcome to Vietduino, test begin!"));
  Serial.println("Version firmWare test: " + String(BOARD));//QC_ID);

  Serial.println(F("Author: Mr.Ngoc - 0938 022 500"));
  TestLow.disable();
  ShowPinState.disable();
  blink_led_ok.disable();
  // testChap_setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  VIETDUINO_UPDATE;
}

#pragma region testChap funtion
CREATE_FUNCTION(testChap_setup_2) {
  for (int cf = Start_pin; cf <= Stop_pin; cf++) {
    if ((cf % 2) == 0) {
      pinMode(cf, INPUT_PULLUP);
    } else {
      pinMode(cf, OUTPUT);
      digitalWrite(cf, 0);
    }
  }

  delay(10);
  for (int cf = Start_pin; cf <= Stop_pin; cf++) {
    //    if(BOARD == "Nano"){
    if (BOARD != "Uno" && BOARD != "Mega 2560") {
      if (cf == 13) {
        continue;
      }
    }
    if ((cf % 2) == 0) {
      if (digitalRead(cf) == 0) {
        //        Serial.println("May be pin " + String (cf) + " error");
        Serial.print("May be pin ");
        if (Stop_pin > 50 && cf >= 54) {
          Serial.print("A" + String(cf - 54));
        } else if (Stop_pin < 50 && cf >= 14) {
          Serial.print("A" + String(cf - 14));
        } else Serial.print(cf);

        Serial.print(" error ");

#if NUM_DIGITAL_PINS > 68 // Mega có 70 chân
        Serial.print(ErrcodeToStr(cf));
#endif

        Serial.println();
        b_complete_bit = true;
      } else;
    } else;
  }

  delay(10);

  for (int cf = Start_pin; cf <= Stop_pin; cf++) {
    if ((cf % 2) != 0) {
      pinMode(cf, INPUT_PULLUP);
    } else {
      pinMode(cf, OUTPUT);
      digitalWrite(cf, 0);
    }
  }

  delay(10);
  for (int cf = Start_pin; cf <= Stop_pin; cf++) {
    //    if(BOARD == "Nano"){
    if (BOARD != "Uno" && BOARD != "Mega 2560") {
      if (cf == 13) {
        continue;
      }
    }
    if ((cf % 2) != 0) {
      if (digitalRead(cf) == 0) {
        //        Serial.println("May be pin " + String (cf) + " error");

        Serial.print("May be pin ");
        if (Stop_pin > 50 && cf >= 54) {
          Serial.print("A" + String(cf - 54));
        } else if (Stop_pin < 50 && cf >= 14) {
          Serial.print("A" + String(cf - 14));
        } else Serial.print(cf);

        Serial.print(" error ");

#if NUM_DIGITAL_PINS > 68 // Mega có 70 chân
        Serial.print(ErrcodeToStr(cf));
#endif

        Serial.println();

        b_complete_bit = true;
      } else;
    } else;
  }

  for (int cf = Start_pin; cf <= Stop_pin; cf++) {
    pinMode(cf, INPUT);
    my_pin_array[cf].lastValue = HIGH;
  }

  if (!b_complete_bit) {
    Serial.println("Congratulation, Vietduino is working very good!");
    blink_led_ok.enable();
  } else {
    Serial.println("Some pin has proplem please check hardware again!");
  }
  testChap_setup_2.disable();
  END_CREATE_FUNCTION
}

#pragma endregion testChap funtion

#pragma region testChap_setup old
// void testChap_setup(){
//   for(int cf = Start_pin; cf <= Stop_pin; cf++){
//     if((cf%2) == 0){
//       pinMode(cf, INPUT_PULLUP);
//     }else{
//       pinMode(cf, OUTPUT);
//       digitalWrite(cf,0);
//     }
//   }

//   delay(10);
//   for(int cf = Start_pin; cf <= Stop_pin; cf++){
//     if((cf%2) == 0){
//       if(digitalRead(cf) == 0){
// //        Serial.println("May be pin " + String (cf) + " error");
//         Serial.print("May be pin ");
//         if(Stop_pin > 50 && cf >= 54){
//           Serial.print("A" + String(cf-54));
//         }else if(Stop_pin < 50 && cf >= 14){
//           Serial.print("A" + String(cf-14));
//         }else Serial.print(cf);

//         Serial.print(" error ");

//         #if NUM_DIGITAL_PINS > 68 // Mega có 70 chân
//           Serial.print(ErrcodeToStr(cf));
//         #endif

//         Serial.println();
//         b_complete_bit = true;
//       }else;
//     }else;
//   }

//   delay(10);

//   for(int cf = Start_pin; cf <= Stop_pin; cf++){
//     if((cf%2) != 0){
//       pinMode(cf, INPUT_PULLUP);
//     }else{
//       pinMode(cf, OUTPUT);
//       digitalWrite(cf,0);
//     }
//   }

//   delay(10);
//   for(int cf = Start_pin; cf <= Stop_pin; cf++){
//     if((cf%2) != 0){
//       if(digitalRead(cf) == 0){
// //        Serial.println("May be pin " + String (cf) + " error");

//         Serial.print("May be pin ");
//         if(Stop_pin > 50 && cf >= 54){
//           Serial.print("A" + String(cf-54));
//         }else if(Stop_pin < 50 && cf >= 14){
//           Serial.print("A" + String(cf-14));
//         }else Serial.print(cf);

//         Serial.print(" error ");

//         #if NUM_DIGITAL_PINS > 68 // Mega có 70 chân
//           Serial.print(ErrcodeToStr(cf));
//         #endif

//         Serial.println();

//         b_complete_bit = true;
//       }else;
//     }else;
//   }

//   for(int cf = Start_pin; cf <= Stop_pin; cf++){
//     pinMode(cf, INPUT);
//     my_pin_array[cf].lastValue = HIGH;
//   }

//   if(!b_complete_bit){
//     Serial.println("Congratulation, Vietduino is working very good!");
//     blink_led_ok.enable();
//   }else{
//     Serial.println("Some pin has proplem please check hardware again!");
//   }


//   testChap_setup_2.disable();
// }
#pragma endregion testChap_setup old

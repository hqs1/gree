/*      本程序基于blinker物联网平台，利用红外发射管、DHT11数字温湿度传感器制作
             硬件清单：ESP8266开发板 、 红外发射管 、DHT11
             硬件连接：红外发射管接ESP8266 D1
                        DHT11   接ESP8266 D2
              WiFi、密码、key需要更换的在程序下修改
*/
#define BLINKER_PRINT Serial //串口调试组件
#define BLINKER_MQTT         //通讯方式
#define BLINKER_ALIGENIE_OUTLET
//#define BLINKER_ESP_SMARTCONFIG
//#define BLINKER_DEBUG_ALL
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ir_Gree.h>
#include <Blinker.h>
#include <modules/dht/DHT.h>

char auth[] = "ea4d6a9782ff"; //APP key
//
// char ssid[] = "MI";   //WiFi 名字
// char pswd[] = "13450375499";             //密码

char ssid[] = "Xiaomi_WB";        //WiFi 名字
char pswd[] = "13450375499"; //密码

//char ssid[] = "Mi6";   //WiFi 名字
//char pswd[] = "2277375994";             //密码

int nowtemp = 26; //用于暂存温度数据
int num_mode = 1; //用于暂存模式数据
int nowfan = 1;   //用于暂存风速数据

// 新建组件对象
BlinkerNumber NUM1("num-temp"); //绑定数字组件
BlinkerNumber NUM2("num-h");    //绑定数字组件
BlinkerNumber NUM3("settemp");
BlinkerButton Gree_power("btn_pwr");
BlinkerButton Gree_temp_add("btn_tp+");
BlinkerButton Gree_temp_sub("btn_tp-");
BlinkerButton Gree_setmode("btn_mode");
BlinkerButton Gree_fan("btn_fan");

//#define DHTPIN D1     //dht11传感器端口
#define IR_LED D1     //红外发射管端口
//#define DHTTYPE DHT22 // DHT 22
IRGreeAC Gree(IR_LED);

//DHT dht(DHTPIN, DHTTYPE); //格式 （传感器端口，传感器型号）

void setup()
{
  // 初始化串口
  Serial.begin(115200);
  Gree.begin();
  // pinMode(D2, OUTPUT);
  Gree_power.attach(Gree_power_callback);
  Gree_temp_add.attach(Gree_temp_add_callback);
  Gree_temp_sub.attach(Gree_temp_sub_callback);
  Gree_setmode.attach(Gree_setmode_callback);
  Gree_fan.attach(Gree_fan_callback);

  //初始化空调状态
  Gree.stateReset();

  Blinker.attachHeartbeat(heartbeat);

  // 初始化blinker
  Blinker.begin(auth,ssid,pswd); //格式 （APP key ， WiFi名字 ，密码）
 // Blinker.begin();
//  dht.begin();
}

void loop()
{
  Blinker.run(); //物联网基本组件（重要不可注释）

  // 如果未绑定的组件被触发，则会执行其中内容
}

void heartbeat() //心跳包入口
{
//  float h = dht.readHumidity();
//  float t = dht.readTemperature();

  if (Gree.getPower())
  {

    Gree_power.icon("fal fa-power-off");
    Gree_power.color("#FF0000");
    Gree_power.text("", "开");
    Gree_power.print("on");
  }
  else if (Gree.getPower())
  {

    Gree_power.icon("fal fa-power-off");
    Gree_power.color("#FFFFFF");
    Gree_power.text("", "关");
    Gree_power.print("off");
  }

  switch (num_mode)
  {
    case 1:

      Gree_setmode.icon("far fa-recycle");
      Gree_setmode.color("#7FFFD4");
      Gree_setmode.text("", "自动");

      break;
    case 2:

      Gree_setmode.icon("fas fa-snowflake");
      Gree_setmode.color("#1E90FF");
      Gree_setmode.text("", "制冷");
      break;
    case 3:

      Gree_setmode.icon("fas fa-tint");
      Gree_setmode.color("#7CFC00");
      Gree_setmode.text("", "除湿");
      break;
    case 4:

      Gree_setmode.icon("fab fa-first-order-alt");
      Gree_setmode.color("#E0FFFF");
      Gree_setmode.text("", "送风");
      break;
    case 5:

      Gree_setmode.icon("fas fa-sun");
      Gree_setmode.color("#FF0000");
      Gree_setmode.text("", "制热");
      break;
  }
  Gree_setmode.print();

  switch (nowfan)
  {
    case 0:
      Gree_fan.text("自动");
      break;
    case 1:
      Gree_fan.text("风速1");
      break;
    case 2:
      Gree_fan.text("风速2");
      break;
    case 3:
      Gree_fan.text("风速3");
      break;
    case 4:
      Gree_fan.text("超强");
      break;
  
  }
  Gree_fan.print();

//  NUM1.print(t);
//  NUM2.print(h);
  NUM3.print(nowtemp);
}

/****************** 按钮事件 *********************/

//空调电源开关
void Gree_power_callback(const String &state)
{
  BLINKER_LOG2("get button state: ", state);

  if (state == BLINKER_CMD_ON)
  {
    Gree.on();
    Gree.send();
    Gree_power.icon("fal fa-power-off");
    Gree_power.color("#FF0000");
    Gree_power.text("", "开");
    Gree_power.print("on");
  }
  else if (state == BLINKER_CMD_OFF)
  {
    Gree.off();
    Gree.send();
    Gree_power.icon("fal fa-power-off");
    Gree_power.color("#FFFFFF");
    Gree_power.text("", "关");
    Gree_power.print("off");
  }
}

//http://www.w3school.com.cn/tags/html_ref_colornames.asp
//https://fontawesome.com/

//空调模式按钮
void Gree_setmode_callback(const String &state)
{
  if (state == BLINKER_CMD_BUTTON_TAP)
  {
    num_mode++;
    if (num_mode >= 6)
    {
      num_mode = 1;
    }

    switch (num_mode)
    {
      case 1:
        Gree.setMode(GREE_AUTO);
        Gree_setmode.icon("fal fa-recycle");
        Gree_setmode.color("#008000");
        Gree_setmode.text("", "自动");
        BLINKER_LOG2("auto ", num_mode);
        break;

      case 2:
        Gree.setMode(GREE_COOL);
        Gree_setmode.icon("fal fa-snowflake");
        Gree_setmode.color("#1E90FF");
        Gree_setmode.text("", "制冷");
        break;

      case 3:
        Gree.setMode(GREE_DRY);
        Gree_setmode.icon("fal fa-tint-slash");
        Gree_setmode.color("#87CEFA");
        Gree_setmode.text("", "除湿");
        break;

      case 4:
        Gree.setMode(GREE_FAN);
        Gree_setmode.icon("fab fa-first-order-alt");
        Gree_setmode.color("#2E8B57");
        Gree_setmode.text("", "送风");
        break;

      case 5:
        Gree.setMode(GREE_HEAT);
        Gree_setmode.icon("fas fa-sun");
        Gree_setmode.color("#FF0000");
        Gree_setmode.text("", "制热");
        break;
    }
    Gree_setmode.print();
    Gree.send();
  }
}

//空调温度+
void Gree_temp_add_callback(const String &state)
{
  if (state == "temp+")
  {
    BLINKER_LOG1("temp+");
    nowtemp++;
    if (nowtemp >= 31)
    {
      nowtemp = 30;
    }
    Gree_temp_add.icon("far fa-plus");
    NUM3.print(nowtemp);
    Gree.setTemp(nowtemp);
    Gree.send();
  }
}

//空调温度-
void Gree_temp_sub_callback(const String &state)
{

  if (state == "temp-")
  {
    BLINKER_LOG1("temp-");
    nowtemp--;
    if (nowtemp <= 15)
    {
      nowtemp = 16;
    }
    Gree_temp_sub.icon("far fa-minus");
    NUM3.print(nowtemp);
    Gree.setTemp(nowtemp);
    Gree.send();
  }
}

//空调风速
void Gree_fan_callback(const String &state)
{

  if (state == BLINKER_CMD_BUTTON_TAP)
  {
    nowfan++;
    if (nowfan >= 6)
    {
      nowfan = 1;
    }
    switch (nowfan)
    {
      case 0:
        Gree_fan.text("自动");
        Gree.setFan(nowfan);
        Gree.setXFan(false);
        break;

      case 1:
        Gree_fan.text("风速1");
        Gree.setFan(nowfan);
        Gree.setXFan(false);
        break;

      case 2:
        Gree_fan.text("风速2");
        Gree.setFan(nowfan);
        Gree.setXFan(false);
        break;

      case 3:
        Gree_fan.text("风速3");
        Gree.setFan(nowfan);
        Gree.setXFan(false);
        break;

     case 4:
        Gree_fan.text("超强");
        Gree.setXFan(true);
    }
    Gree_fan.print();

    Gree.send();
  }
}

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <epd.h>
#include <Wire.h>

const String endpoint_metoffice = "http://datapoint.metoffice.gov.uk/public/data/val/wxfcs/all/json/352793?res=3hourly&key=/* replace by your KEY */ ";
const String newsapiOrg = "http://newsapi.org/v2/top-headlines?sources=the-guardian-uk&apiKey=/* replace by your KEY */";
String news_payload;

const int wake_up = 14;
const int reset = 12;

char buff[96] = {};
String g_hours;
String g_minutes;
int g_minutes_int;
String g_week_day;

String g_news0;
String g_news1;
String g_news2;
String g_news3;
String g_date;

String g_weather0_main;
String g_main_temp;
String g_main_temp_min;
String g_main_temp_max;
String g_main_humidity;
String g_main_pressure;

String today_weatherType;
String tomorrow_weatherType;
String after_weatherType;

String today_feelsLike;
String tomorrow_feelsLike;
String after_feelsLike;

String today_humi;
String tomorrow_humi;
String after_humi;

String today_pop;
String tomorrow_pop;
String after_pop;


String after2_weatherType;
String after2_feelsLike ;
String after2_humi ;
String after2_pop ;

const char *ssid     = "replace by your ssid";
const char *password = "replace by your wifi password";
String g_ssid = ssid;
const long utcOffsetInSeconds = 3600;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);


void setup() {
  epd_init(wake_up, reset);
  epd_wakeup(wake_up);
  epd_set_memory(MEM_NAND);
  //----------epd setup
}

void get_time_from_ntp() {
  timeClient.update();
  Serial.print(timeClient.getHours() - 1);
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(daysOfTheWeek[timeClient.getDay()]);
  g_hours = timeClient.getHours() - 1;
  g_minutes = timeClient.getMinutes();
  g_minutes_int = timeClient.getMinutes();
  g_week_day = daysOfTheWeek[timeClient.getDay()];
}

void htte_get_news() {
  HTTPClient http;
  http.begin(newsapiOrg); //Specify the URL
  int httpCode = http.GET();  //Make the request
  if (httpCode > 0) { //Check for the returning code
    news_payload = http.getString();
    Serial.println("Http get news api, ok.");
  }
  else {
    Serial.println("Error on news HTTPs request");
  }
  http.end(); //Free the resources
}

void print_news() {
  const size_t capacity = JSON_ARRAY_SIZE(10) + 10 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3) + 10 * JSON_OBJECT_SIZE(8) + 11000;
  DynamicJsonBuffer jsonBuffer(capacity);
  String json = news_payload;
  JsonObject& root = jsonBuffer.parseObject(json);
  const char* status = root["status"]; // "ok"
  int totalResults = root["totalResults"]; // 10
  JsonArray& articles = root["articles"];
  JsonObject& articles0 = articles[0];
  const char* articles0_title = articles0["title"]; // "Mother of three-year-old is first person convicted of FGM in UK"
  const char* articles0_publishedAt = articles0["publishedAt"]; // "2019-02-02T14:36:19Z"
  JsonObject& articles1 = articles[1];
  const char* articles1_title = articles1["title"]; // "Senior HMRC official sacked after sexual misconduct claims"
  JsonObject& articles2 = articles[2];
  const char* articles2_title = articles2["title"]; // "UK weather: heavy snow closes schools and disrupts travel"
  JsonObject& articles3 = articles[3];
  const char* articles3_title = articles3["title"]; // "Man who upskirted woman at tube station fined £2,000"
  Serial.println(articles0_title);
  Serial.println(articles1_title);
  Serial.println(articles2_title);
  Serial.println(articles3_title);
  g_news0 = articles0_title;
  g_news1 = articles1_title;
  g_news2 = articles2_title;
  g_news3 = articles3_title;
  g_date = articles0_publishedAt;
}

void connect_wifi_set_up_time() {

  Serial.begin(115200);
  WiFi.begin(ssid, password);
  display_offline() ;
  while ( WiFi.status() != WL_CONNECTED ) {
    delay ( 500 );
    Serial.print ( "." );
  }
  timeClient.begin();
}
void display_offline() {
  epd_init(wake_up, reset);
  epd_set_color(BLACK, WHITE);
  epd_clear();
  epd_set_en_font(ASCII64);
  epd_disp_string("Currently Offline.", 20, 20);
  epd_disp_string("Please wait.", 20, 90);
  epd_disp_string("Connecting to Wi-Fi.....", 20, 160);

  ("SSID: " + g_ssid).toCharArray(buff, 25);
  char *ssid_display[]  = {buff};
  epd_disp_string(ssid_display[0], 20, 230);
  epd_udpate();
}

void get_print_metoffice() {
  String json; //payload of metoffice http request
  HTTPClient http;
  http.begin(endpoint_metoffice); //Specify the URL
  int httpCode = http.GET();  //Make the request
  if (httpCode > 0) { //Check for the returning code
    json  = http.getString();
    Serial.println("Http get metoffice weather api, ok.");
  }
  else {
    Serial.println("Error on metoffice weather HTTP request");

  }
  http.end(); //Free the resources

  const size_t capacity = JSON_ARRAY_SIZE(5) + JSON_ARRAY_SIZE(6) + 4 * JSON_ARRAY_SIZE(8) + JSON_ARRAY_SIZE(10) + 2 * JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(2) + 16 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(8) + 38 * JSON_OBJECT_SIZE(11) + 2950;
  DynamicJsonBuffer jsonBuffer(capacity + 10000);
  JsonObject& root = jsonBuffer.parseObject(json);
  JsonArray& SiteRep_Wx_Param = root["SiteRep"]["Wx"]["Param"];
  JsonObject& SiteRep_Wx_Param0 = SiteRep_Wx_Param[0];
  JsonObject& SiteRep_Wx_Param1 = SiteRep_Wx_Param[1];
  JsonObject& SiteRep_Wx_Param2 = SiteRep_Wx_Param[2];
  JsonObject& SiteRep_Wx_Param3 = SiteRep_Wx_Param[3];
  JsonObject& SiteRep_DV = root["SiteRep"]["DV"];
  JsonObject& SiteRep_DV_Location = SiteRep_DV["Location"];
  JsonArray& SiteRep_DV_Location_Period = SiteRep_DV_Location["Period"];
  JsonObject& SiteRep_DV_Location_Period0 = SiteRep_DV_Location_Period[0];
  JsonArray& SiteRep_DV_Location_Period0_Rep = SiteRep_DV_Location_Period0["Rep"];
  JsonObject& SiteRep_DV_Location_Period0_Rep0 = SiteRep_DV_Location_Period0_Rep[0];
  const char* SiteRep_DV_Location_Period0_Rep0_F = SiteRep_DV_Location_Period0_Rep0["F"]; // "-7" feels like
  const char* SiteRep_DV_Location_Period0_Rep0_H = SiteRep_DV_Location_Period0_Rep0["H"]; // "76" relative humidi
  const char* SiteRep_DV_Location_Period0_Rep0_Pp = SiteRep_DV_Location_Period0_Rep0["Pp"]; // "1" prob of rain
  const char* SiteRep_DV_Location_Period0_Rep0_T = SiteRep_DV_Location_Period0_Rep0["T"]; // "-5" real temp
  const char* SiteRep_DV_Location_Period0_Rep0_W = SiteRep_DV_Location_Period0_Rep0["W"]; // "2" weather type

  today_weatherType = SiteRep_DV_Location_Period0_Rep0_W;
  today_feelsLike = SiteRep_DV_Location_Period0_Rep0_F;
  today_humi = SiteRep_DV_Location_Period0_Rep0_H;
  today_pop = SiteRep_DV_Location_Period0_Rep0_Pp;
  Serial.println(today_weatherType);

  JsonObject& SiteRep_DV_Location_Period1 = SiteRep_DV_Location_Period[1];

  JsonArray& SiteRep_DV_Location_Period1_Rep = SiteRep_DV_Location_Period1["Rep"];

  JsonObject& SiteRep_DV_Location_Period1_Rep4 = SiteRep_DV_Location_Period1_Rep[4];
  const char* SiteRep_DV_Location_Period1_Rep4_F = SiteRep_DV_Location_Period1_Rep4["F"]; // "1"
  const char* SiteRep_DV_Location_Period1_Rep4_H = SiteRep_DV_Location_Period1_Rep4["H"]; // "89"
  const char* SiteRep_DV_Location_Period1_Rep4_Pp = SiteRep_DV_Location_Period1_Rep4["Pp"]; // "1"
  const char* SiteRep_DV_Location_Period1_Rep4_T = SiteRep_DV_Location_Period1_Rep4["T"]; // "6"
  const char* SiteRep_DV_Location_Period1_Rep4_W = SiteRep_DV_Location_Period1_Rep4["W"]; // "2"
  tomorrow_weatherType = SiteRep_DV_Location_Period1_Rep4_W;
  tomorrow_feelsLike = SiteRep_DV_Location_Period1_Rep4_F;
  tomorrow_humi = SiteRep_DV_Location_Period1_Rep4_H;
  tomorrow_pop = SiteRep_DV_Location_Period1_Rep4_Pp;


  JsonObject& SiteRep_DV_Location_Period2 = SiteRep_DV_Location_Period[2];
  JsonArray& SiteRep_DV_Location_Period2_Rep = SiteRep_DV_Location_Period2["Rep"];
  JsonObject& SiteRep_DV_Location_Period2_Rep4 = SiteRep_DV_Location_Period2_Rep[4];
  const char* SiteRep_DV_Location_Period2_Rep4_F = SiteRep_DV_Location_Period2_Rep4["F"]; // "-3"
  const char* SiteRep_DV_Location_Period2_Rep4_H = SiteRep_DV_Location_Period2_Rep4["H"]; // "95"
  const char* SiteRep_DV_Location_Period2_Rep4_Pp = SiteRep_DV_Location_Period2_Rep4["Pp"]; // "15"
  const char* SiteRep_DV_Location_Period2_Rep4_T = SiteRep_DV_Location_Period2_Rep4["T"]; // "0"
  const char* SiteRep_DV_Location_Period2_Rep4_W = SiteRep_DV_Location_Period2_Rep4["W"]; // "5"
  after_weatherType = SiteRep_DV_Location_Period2_Rep4_W;
  after_feelsLike = SiteRep_DV_Location_Period2_Rep4_F;
  after_humi = SiteRep_DV_Location_Period2_Rep4_H;
  after_pop = SiteRep_DV_Location_Period2_Rep4_Pp;


  JsonObject& SiteRep_DV_Location_Period3 = SiteRep_DV_Location_Period[3];
  JsonArray& SiteRep_DV_Location_Period3_Rep = SiteRep_DV_Location_Period3["Rep"];
  JsonObject& SiteRep_DV_Location_Period3_Rep4 = SiteRep_DV_Location_Period3_Rep[4];
  const char* SiteRep_DV_Location_Period3_Rep4_F = SiteRep_DV_Location_Period3_Rep4["F"]; // "-3"
  const char* SiteRep_DV_Location_Period3_Rep4_H = SiteRep_DV_Location_Period3_Rep4["H"]; // "95"
  const char* SiteRep_DV_Location_Period3_Rep4_Pp = SiteRep_DV_Location_Period3_Rep4["Pp"]; // "15"
  const char* SiteRep_DV_Location_Period3_Rep4_T = SiteRep_DV_Location_Period3_Rep4["T"]; // "0"
  const char* SiteRep_DV_Location_Period3_Rep4_W = SiteRep_DV_Location_Period3_Rep4["W"]; // "5"
  after2_weatherType = SiteRep_DV_Location_Period3_Rep4_W;
  after2_feelsLike = SiteRep_DV_Location_Period3_Rep4_F;
  after2_humi = SiteRep_DV_Location_Period3_Rep4_H;
  after2_pop = SiteRep_DV_Location_Period3_Rep4_Pp;

  Serial.println("metoffice");
}

void display_epd() {
  epd_init(wake_up, reset);
  epd_set_color(BLACK, WHITE);
  epd_clear();
  char *display_buffer[]  = {buff};

  epd_set_en_font(ASCII64);
  (g_week_day).toCharArray(buff, 10);

  epd_disp_string(display_buffer[0], 520, 4);
  
  (g_date).toCharArray(buff, 11);

  epd_disp_string(display_buffer[0], 6, 5);

  epd_set_en_font(ASCII32);
  ("@" + g_hours + ":" + g_minutes).toCharArray(buff, 8);
  epd_disp_string(display_buffer[0], 700, 90);

  //g_date
  for (int i = 0; i <= 800; i += 100)
  {
    epd_draw_line(0, 75, i, 75);
  }

  epd_set_en_font(ASCII48);
  epd_disp_string("Headlines from The Guardian: ", 6, 85);
  epd_set_en_font(ASCII32);

  (g_news0).toCharArray(buff, 64);
  epd_disp_string(display_buffer[0], 6, 145);

  (g_news1).toCharArray(buff, 64);
  epd_disp_string(display_buffer[0], 6, 185);

  (g_news2).toCharArray(buff, 64);
  epd_disp_string(display_buffer[0], 6, 225);

  (g_news3).toCharArray(buff, 64);
  epd_disp_string(display_buffer[0], 6, 265);

  epd_set_en_font(ASCII48);

  for (int i = 0; i <= 800; i += 100)
  {
    epd_draw_line(0, 315, i, 315);
  }


  (today_weatherType + ".JPG").toCharArray(buff, 12);
//  char *today_weather_display[]  = {buff};
  epd_disp_bitmap(display_buffer[0], 15, 360);

  (tomorrow_weatherType + ".JPG").toCharArray(buff, 12);
  epd_disp_bitmap(display_buffer[0], 210, 360);

  (after_weatherType + ".JPG").toCharArray(buff, 12);
  epd_disp_bitmap(display_buffer[0], 409, 360);


  (after2_weatherType + ".JPG").toCharArray(buff, 12);
  epd_disp_bitmap(display_buffer[0], 608, 360);


  epd_set_en_font(ASCII32);

  epd_disp_string("Today", 15, 327);
  (today_feelsLike + " C").toCharArray(buff, 6);
//  char *today_feelsLike_display[]  = {buff};
  epd_disp_string(display_buffer[0], 15, 450);

  (today_humi + " %" ).toCharArray(buff, 6);
  epd_disp_string(display_buffer[0], 15, 500);

  (today_pop + " % Rain").toCharArray(buff, 10);
  epd_disp_string(display_buffer[0], 15, 550);

  epd_disp_string("Tomorrow", 210, 327);
  (tomorrow_feelsLike + " C").toCharArray(buff, 6);
  epd_disp_string(display_buffer[0], 210, 450);

  (tomorrow_humi + " %" ).toCharArray(buff, 6);
//  char *tomorrow_humi_display[]  = {buff};
  epd_disp_string(display_buffer[0], 210, 500);

  (tomorrow_pop + " % Rain").toCharArray(buff, 10);
//  char *tomorrow_pop_display[]  = {buff};
  epd_disp_string(display_buffer[0], 210, 550);


  epd_disp_string("Day After", 409, 327);
  (after_feelsLike + " C").toCharArray(buff, 6);
  epd_disp_string(display_buffer[0], 409, 450);

  (after_humi + " %" ).toCharArray(buff, 6);
  epd_disp_string(display_buffer[0], 409, 500);

  (after_pop + " % Rain").toCharArray(buff, 10  );
  char *after_pop_display[]  = {buff};
  epd_disp_string(after_pop_display[0], 409, 550);

epd_disp_string("Two Days After", 608, 327);
  (after2_feelsLike + " C").toCharArray(buff, 6);
  epd_disp_string(display_buffer[0], 608, 450);

  (after2_humi + " %" ).toCharArray(buff, 6);
  epd_disp_string(display_buffer[0], 608, 500);

  (after2_pop + " % Rain").toCharArray(buff, 10  );
  epd_disp_string(display_buffer[0], 608, 550);

  epd_udpate();
}

void loop() {
  connect_wifi_set_up_time();

  while (WiFi.status() == WL_CONNECTED) {
    get_time_from_ntp();
    get_print_metoffice();
    htte_get_news();
    print_news();
    display_epd();
    delay(12000);
  }
}

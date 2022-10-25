#include <WiFi.h>

const char* ssid = "ssid";
const char* password = "password";

void setup() {
  Serial.begin(115200);

  delay(10);

  // wifi_scan();
}

void wifi_scan() {

  // WiFi.disconnect();
  WiFi.disconnect(false, true);

  delay(100);
  Serial.print("\nScan start ... ");
  int n = WiFi.scanNetworks();
  Serial.print(n);
  Serial.println(" network(s) found");

  for (int i = 0; i < n; ++i) {

    // Print SSID and RSSI for each network found
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(WiFi.SSID(i));
    Serial.print(" (");
    Serial.print(WiFi.RSSI(i));
    Serial.print(")");
    Serial.printf("\nBSSID: %s\n", WiFi.BSSIDstr(i).c_str());
    Serial.print(" Channel:");
    Serial.print(WiFi.channel(i));

    Serial.println("");
    delay(10);
  }
  Serial.println();
}

#define ARRAYSIZE 13
// #define ARRAYSIZE 2  // test only first two power modes
#define CONNECT_ATTEMTS 10

wifi_power_t powerModes[13] = { WIFI_POWER_19_5dBm, WIFI_POWER_19dBm, WIFI_POWER_18_5dBm, WIFI_POWER_17dBm, WIFI_POWER_15dBm, WIFI_POWER_13dBm, WIFI_POWER_11dBm, WIFI_POWER_8_5dBm, WIFI_POWER_7dBm, WIFI_POWER_7dBm, WIFI_POWER_5dBm, WIFI_POWER_2dBm, WIFI_POWER_MINUS_1dBm };
int successCount[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int failureCount[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


void loop() {
  // reset success and failure count
  for (int i = 0; i < ARRAYSIZE; i++) {
    successCount[i] = 0;
    failureCount[i] = 0;
  }

  // set station mode
  WiFi.mode(WIFI_STA);

  // over all power modes
  for (int powerCnt = 0; powerCnt < ARRAYSIZE; powerCnt++) {

    // set new power mode
    Serial.printf("\nSet new powerMode %d\n", powerCnt);

    Serial.printf("\setTxPower:%i\n", powerModes[powerCnt]);

    WiFi.setTxPower(powerModes[powerCnt]);
    Serial.printf("\getTxPower:%i\n", WiFi.getTxPower());

    delay(2000);

    for (int connectAttempt = 0; connectAttempt < CONNECT_ATTEMTS; connectAttempt++) {
      Serial.printf("\nConnectAttempt %d\n", connectAttempt);

      // disconnect
      WiFi.disconnect(false, true);

      // wait 2 seconds
      delay(2000);

      // try to connect using waitForConnectResult()
      // WiFi.begin(ssid, password);
      // if (WiFi.waitForConnectResult() == WL_CONNECTED) {
      //   Serial.println("\tWiFi connected");
      //   Serial.printf("\tIP address: ");
      //   Serial.println(WiFi.localIP());
      //   successCount[powerCnt]++;
      // } else {
      //   Serial.println("\tWiFi Failed");
      //   failureCount[powerCnt]++;
      // }



      // perhaps better to test work with manual timeout by using millis?
      // as waitForConnectResult() has diffent timeout in different esp32 versions
      unsigned long begints;
      unsigned long elapsedts;
      unsigned long timeout = 6000;  // timeout after 6 seconds

      begints = millis();
      WiFi.begin(ssid, password);

      while (WiFi.status() != WL_CONNECTED) {
        elapsedts = millis() - begints;
        if (elapsedts > timeout) {  // if elsapsed > timeout
          break;
        }
        delay(500);
        Serial.print(".");
      }

      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\tWiFi connected");
        Serial.printf("\tIP address: ");
        Serial.println(WiFi.localIP());
        successCount[powerCnt]++;
      } else {
        Serial.println("\tWiFi Failed");
        failureCount[powerCnt]++;
      }
    }
  }

  Serial.printf("Max Attempts: %i\n", CONNECT_ATTEMTS);
  Serial.println("RESULTS:");

  // print out success and failurecount by powermode
  Serial.print("mode:\t\t");

  for (int i = 0; i < ARRAYSIZE; i++) {
    Serial.printf("%i\t", powerModes[i]);
  }
  Serial.print("\n");

  Serial.print("success:\t");
  for (int i = 0; i < ARRAYSIZE; i++) {
    Serial.printf("%i\t", successCount[i]);
    successCount[i] = 0;
  }
  Serial.print("\n");

  Serial.print("failure:\t");
  for (int i = 0; i < ARRAYSIZE; i++) {
    Serial.printf("%i\t", failureCount[i]);
    failureCount[i] = 0;
  }
  Serial.print("\n");
  Serial.print("\n");
  delay(5000);
}
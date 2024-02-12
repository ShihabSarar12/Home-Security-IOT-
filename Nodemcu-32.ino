#include <WiFi.h>

const char* ssid     = "Wifi_Name";
const char* password = "Wifi_Password";

WiFiServer server(80);
const int outputPin = 2;

void setup() {
    Serial.begin(115200);
    pinMode(outputPin, OUTPUT);

    delay(10);

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();
}

void loop() {
    WiFiClient client = server.available();

    if (client) {
        Serial.println("New Client.");
        String currentLine = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                Serial.write(c);
                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();

                        client.println("<!DOCTYPE html>");
                        client.println("<html>");
                        client.println("<head>");
                        client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<style>");
                        client.println("body { display: flex; justify-content: center; align-items: center; gap: 1rem; height: 100vh; margin: 0; }");
                        client.println(".button { padding: 15px 30px; font-size: 18px; text-align: center; text-decoration: none; display: inline-block; border-radius: 8px; color: #fff; }");
                        client.println("</style>");
                        client.println("</head>");
                        client.println("<body>");

                        // the content of the HTML body:
                        client.println("<a style=\"background-color: #34db5e\" href=\"/H\" class=\"button\">Unlock</a><br>");
                        client.println("<a style=\"background-color: #e84138\" href=\"/L\" class=\"button\">Lock</a><br>");

                        client.println("</body>");
                        client.println("</html>");

                        client.println();
                        break;
                    } else {
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c;
                }

                if (currentLine.endsWith("GET /H")) {
                    digitalWrite(outputPin, HIGH);
                }
                if (currentLine.endsWith("GET /L")) {
                    digitalWrite(outputPin, LOW);
                }
            }
        }
        client.stop();
        Serial.println("Client Disconnected.");  
    }
}

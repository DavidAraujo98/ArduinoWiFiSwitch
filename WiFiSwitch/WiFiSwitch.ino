char ssid[] = "NOS_Internet_F97C";
char pass[] = "99531543";

int wifi_led = D2;
int trigger = D1;
String header;

WiFiServer server(80);

unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 10000;

void turnSw(WiFiClient cl)
{
  if (digitalRead(trigger) != 1)
  {
    digitalWrite(trigger, HIGH);
    cl.println("<p style=\"color: green\">Desligado</p>");
  }
  else
  {
    digitalWrite(trigger, LOW);
    cl.println("<p style=\"color: red\">Ligado</p>");
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(wifi_led, OUTPUT);
  pinMode(trigger, OUTPUT);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
  digitalWrite(wifi_led, HIGH);
  digitalWrite(trigger, LOW);
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop()
{
  WiFiClient client = server.available();
  if (client)
  {
    Serial.println("New Client");
    boolean ch = true;
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    {
      currentTime = millis();
      if (client.available())
      {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n')
        {
          if (ch)
          {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connnection: close");
            client.println();
            if (header.indexOf("GET /on") >= 0)
            {
              turnSw(client);
            }
            else
            {
              client.println("<!DOCTYPE html><html>");
              client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
              client.println("<link rel=\"icon\" href=\"data:,\">");
              client.println("<script>");
              client.println("function turnOn() {");
              client.println("var request = new XMLHttpRequest();");
              client.println("request.onreadystatechange = function(){");
              client.println("if(this.readyState == 4 && this.status ==200){");
              client.println("if(this.responseText != null){");
              client.println("document.getElementById(\"state\").innerHTML = this.responseText;");
              client.println("}}}");
              client.println("request.open(\"GET\",\"on\",true);");
              client.println("request.send(null); }");
              client.println("</script></head><body>");
              client.println("<h1 style=\" text - align: center; font - family: verdana; color: rgb(82, 82, 82) \">Controlo Painel Solar</h1>");
              if (digitalRead(trigger) != 1)
              {
                client.println("<h2 id=\"state\" style=\"text-align: center; font-family: verdana;\"><p style=\"color: red\">Ligado</p></h2>");
              }
              else
              {
                client.println("<h2 id=\"state\" style=\"text-align: center; font-family: verdana;\"><p style=\"color: green\">Desligado</p></h2>");
              }
              client.println("<div style=\" height : 50px; position: relative;\">");
              client.println("<div style=\"margin: 0; position: absolute; top: 50%; left: 50%; -ms-transform: translate(-50%, -50%); transform: translate(-50%, -50%);\">");
              client.println("<button id=\"OnSw\" type=\"button\" onclick=\"turnOn()\" >ON/OFF</button>");
              client.println("</div></div>");
              client.println("</body></html>");
              client.println();
            }
            break;
          }
          else
          {
            ch = true;
          }
        }
        else if (c != '\r')
        {
          ch = false;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>

// #define DEBUG

#define IDLE -1
#define MOVE 1

#define BACKWARD 0
#define FORWARD 1
#define LEFT 2
#define RIGHT 3
//============================================================
//                          SERVER_INFO
//============================================================
const char* ssid = "NodeMCU-Spider";
const char* password = "13424RG1";

String server_info = "";
String server_spider_info = "";

IPAddress apIP(192, 168, 0, 5);

ESP8266WebServer server(80);

int commandID = IDLE;
//============================================================
//                          PROTOTYPES
//============================================================
void handleRoot();
void infoPage();

void sendCommandMove();
void sendCommand_UP();
void sendCommand_DOWN();
void sendCommand_RIGHT();
void sendCommand_LEFT();
//============================================================
//                          MAIN_FUNC
//============================================================
void setup() {
// set ports
// i2c bridge
  Wire.begin(D1, D2);
  
#ifdef DEBUG
  Serial.begin(115200);
  Serial.println();
  Serial.println("Configuring access point...");
#endif
  
  // create AP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);

#ifdef DEBUG
  Serial.print("AP IP address: ");
  Serial.println(WiFi.localIP());
#endif

  // set work logic
  server.on("/", handleRoot);
  server.on("/info_page", infoPage);
  server.on("/move", sendCommandMove);
  
  server.on("/up", sendCommand_UP);
  server.on("/down", sendCommand_DOWN);
  server.on("/right", sendCommand_RIGHT);
  server.on("/left", sendCommand_LEFT);
  
  server.begin();

#ifdef DEBUG
  Serial.print("HTTP server started;\n");
#endif

  server_info += "<!DOCTYPE html><html><head>  <title>Robot-Spider</title> <style>   *{      font-family: \"Lucida Console\", Monaco, monospace;     margin: 0;      padding: 0;   }     a.btn {     color: #fff; /* цвет текста */      text-decoration: none; /* убирать подчёркивание у ссылок */     user-select: none; /* убирать выделение текста */     background: rgb(212,75,56); /* фон кнопки */      padding: .7em 1.5em; /* отступ от текста */     outline: none; /* убирать контур в Mozilla */     font-size: 120%;    }     a.btn:hover { background: rgb(232,95,76); } /* при наведении курсора мышки */   a.btn:active { background: rgb(152,15,0); } /* при нажатии */       a.button30 {      position: relative;     z-index: 1;     display: inline-block;      width: 1.3em;     height: 1.3em;      line-height: 1.3em;     vertical-align: middle;     text-align: center;     text-decoration: none;      text-shadow: 1px 1px rgba(255,255,255,.3);      font-size: 300%;      font-weight: 900;     color: #000;      border-radius: 100%;      background: rgb(144,142,105) linear-gradient(rgb(185,181,151), rgb(144,142,105));     box-shadow:      inset 0 -2px 1px rgba(0,0,0,.5),      inset 0 2px 1px rgba(255,255,255,.9),       0 4px 4px rgba(0,0,0,.9);    }   a.button30:after {      content: \"\";      position: absolute;     z-index: -1;      top: 12%;     left: 12%;      right: 12%;     bottom: 12%;      border-radius: 100%;      background: rgb(242,203,20) linear-gradient(rgb(242,203,20), rgb(255,182,7));     box-shadow:      inset 0 2px 1px rgba(0,0,0,.5),       inset 0 -2px 1px rgba(255,255,255,.3);   }       a.title{      margin-left: 42%;     font-size: 50px;      color: white;   }       ul{     list-style-type: none;    }       li{     margin-top: clamp(25px, 2.5%, 100px);   }       .header{      background: #cb2d41;      height: 60px;   }       html,   body,   .wrapper {      height: 100%;   }       .content {      box-sizing: border-box;     min-height: 100%;     padding-bottom: 90px;   }       .footer{      height: 80px;     margin-top: -80px;      background: #cb2d41;    }       .block{     background: #d1cdcd;            margin-left: 10%;     margin-top: 20px;           padding: 10px;      padding-bottom: 20px;           height: 80%;      width: 80%;   }       .control{     margin: 5%;     margin-left: 40%;     height: 10%;    }       .btn{     display: inline-block;    }     </style></head><body> <div class = \"wrapper\">   <div class = \"header\">      <a class = \"title\">Robot-Spider</a>   </div>        <div class = \"content\">     <div class = \"block\">       <a class = \"title\" style = \"color: black;\">Commands</a>       <ul class=\"list3a\">         <li><a href=\"/\" class=\"btn\">Main Page</a></li>          <li><a href=\"/info_page\" class=\"btn\">Info Page</a></li>         <li><a href=\"/move\" class=\"btn\">Move</a></li>       </ul>               <div class = \"control\">         <!-- LEFT -->         <div class = \"btn\">           <a href=\"/left\" class=\"button30\">L</a>          </div>          <!-- UP/DOWN -->          <div class = \"btn\">           <a href=\"/up\" class=\"button30\" style = \"margin-top: -100px; margin-left: 0px;\">U</a>            <a href=\"/down\" class=\"button30\" style = \"margin-top: 100px; margin-left: -70px;\">D</a>         </div>          <!-- RIGHT -->          <div class = \"btn\">           <a href=\"/right\" class=\"button30\">R</a>         </div>        </div>      </div>    </div>        <div class=\"footer\">          </div>  </div></body></html>";
  server_spider_info += "<!DOCTYPE html><html><head> <title>Robot-Spider</title> <style>   *{      font-family: \"Lucida Console\", Monaco, monospace;     margin: 0;      padding: 0;   }     a.btn {     color: #fff; /* цвет текста */      text-decoration: none; /* убирать подчёркивание у ссылок */     user-select: none; /* убирать выделение текста */     background: rgb(212,75,56); /* фон кнопки */      padding: .7em 1.5em; /* отступ от текста */     outline: none; /* убирать контур в Mozilla */     font-size: 120%;    }     a.btn:hover { background: rgb(232,95,76); } /* при наведении курсора мышки */   a.btn:active { background: rgb(152,15,0); } /* при нажатии */         a.title{      margin-left: 42%;     font-size: 50px;      color: white;   }       ul{     list-style-type: none;    }       li{     margin-top: clamp(25px, 2.5%, 100px);   }       .header{      background: #cb2d41;      height: 60px;   }       html,   body,   .wrapper {      height: 100%;   }       .content {      box-sizing: border-box;     min-height: 100%;     padding-bottom: 90px;   }       .footer{      height: 80px;     margin-top: -80px;      background: #cb2d41;    }       .block{     background: #d1cdcd;            margin-left: 10%;     margin-top: 20px;           padding: 10px;      padding-bottom: 20px;           height: 80%;      width: 80%;   }       .control{     margin: 5%;     margin-left: 40%;     height: 10%;    }       .btn{     display: inline-block;    }       .leftimg {      float:left; /* Выравнивание по левому краю */     margin: 7px 7px 7px 0; /* Отступы вокруг картинки */    }        .rightimg  {     float: right; /* Выравнивание по правому краю  */       margin: 7px 0 7px 7px; /* Отступы вокруг картинки */    }     </style></head><body> <div class = \"wrapper\">   <div class = \"header\">      <a class = \"title\">Robot-Spider</a>   </div>        <div class = \"content\">     <div class = \"block\">       <a class = \"title\" style = \"color: black;\">Project Info</a>       <ul class=\"list3a\">         <li><a href=\"/\" class=\"btn\">Main Page</a></li>        </ul>     <p> </p>        <h2 align = \"center\"> Общая информация о проекте</h2>       <p> </p>        <p><img src=\"spooder_walk.gif\" alt=\"\" width=\"300\" height=\"169\" class=\"rightimg\">         Данный проект разрабатывается группой 1941 первого института СПб ГУАП в рамках летней         практики основ программирования. В рамках этого проекта нами была выбрана тема разработки         \"Робота-паука\" на основе платы контроля Arduino Mega 2560. и сервоприводах. На разработку нам было          дано 4 недели, так же наш ВУЗ предоставил нам все необходимые материалы для сборки робота.</p>             <p> </p>      <p> </p>            <h2 align = \"center\">Цели проекта</h2>        <p><img src=\"Паук1.jpg\" alt=\"\" width=\"282\" height=\"216\" class=\"leftimg\">        <ul>          <li>Изучить механику передвижения гексаподов, разработать схему взаимодействия с Arduino.         <li>Разработка прототипа робота-паука, способного перемещаться во все стороны (с учетом           будущего размещения на спине робота турели).</li>         <li>Разработка программы взаимодействия платы Arduino  с компьютером/телефоном посредством          модулей Bluetoooth/Wi-Fi, а также модули ответственные за автономную работу робота-паука.</li>          <li>Реализовать прототип турели с движением по горизонтали и вертикали.</li>          <li>Соединить воедино робота-паука с турелью и провести финальные тесты.</li>       </ul>       </p>      </div>      </div>  </div>      <div class=\"footer\">        </div></body></html>";
}

// the loop function runs over and over again forever
void loop() {
  server.handleClient();

  Wire.beginTransmission(8);
  if(commandID == MOVE)
     Wire.write("command=1");
  
  if(commandID == BACKWARD)
     Wire.write("command=0");
  if(commandID == FORWARD)
     Wire.write("command=1");
  if(commandID == LEFT)
     Wire.write("command=2");
  if(commandID == RIGHT)
     Wire.write("command=3");

  commandID = IDLE;

  Wire.endTransmission();
   delay(500);
}
//============================================================
//                          OTHER
//============================================================
void handleRoot() {
  server.send(200, "text/html", server_info);
}

// information page about project
void infoPage(){
  server.send(200, "text/html", server_spider_info);
}

//============================================================
//                          MOVE_COMMANDS
//============================================================
void sendCommandMove(){
  commandID = MOVE;

  server.send(200, "text/html", server_info);
  
#ifdef DEBUG
  Serial.print("COMMAND:: MOVE;\n");
#endif
}

void sendCommand_UP(){
  commandID = FORWARD;

  server.send(200, "text/html", server_info);
  }

void sendCommand_DOWN(){
  commandID = BACKWARD;

  server.send(200, "text/html", server_info);
  }

void sendCommand_RIGHT(){
  commandID = RIGHT;

  server.send(200, "text/html", server_info);
  }

void sendCommand_LEFT(){
  commandID = LEFT;

  server.send(200, "text/html", server_info);
  }

#include "logger.h"

#include <ESP8266WebServer.h>
#include <FS.h>

Logger logger;

namespace
{

ESP8266WebServer server(80);

File logFile;

const char* web_page = ""
"<html>\n"
"  <head>\n"
"  </head>\n"
"  <body>\n"
"    <script></script>"
"    <h3>{message}</h3>\n"
"    <p><a href=\"/log\">Download Debug Log</a></p>\n"
"    <p><a href=\"/trunc\">Truncate Debug Log</a></p>\n"
"    <p><a href=\"/restart\">Restart Board</a></p>\n"
"  </body>\n"
"</html>\n";

const char* redirect_script = ""
"<script>\n"
"   setTimeout(function(){\n"
"            window.location.href = '/';\n"
"         }, 3000);\n"
"</script>";

} // namespace

void Logger::begin()
{
    Serial.println("Initializing SPIFFS");

    // Create the log file
    if (!SPIFFS.begin())
    {
        Serial.println("Error initializing SPIFFS");
        return;
    }

   Serial.println("Checking for file system...");
   FSInfo info;
   if (!SPIFFS.info(info))
   {
       Serial.println("Formatting file system");
       if (!SPIFFS.format())
       {
           Serial.println("Error formatting SPIFFS");
           return;
       }
   }
   else
   {
       Serial.println("File system already formatted.");
   }

    Serial.println("Creating log file");
    logFile = SPIFFS.open("/debug.log", "a");
    if (!logFile)
    {
        Serial.println("Failed to create log file");
    }

    Serial.println("Starting web server");

    server.on("/", HTTP_GET, []() {
            String html = web_page;
            if (server.hasArg("message"))
            {
                html.replace("{message}", server.arg("message"));
                html.replace("<script></script>", redirect_script);
            }
            else
            {
                html.replace("{message}", "");
            }
            server.send(200, "text/html", html);
    });
    server.on("/log", HTTP_GET, []() {
            logFile.flush();
            File log = SPIFFS.open("/debug.log", "r");
            if (!log)
            {
                server.send(404, "text/plain", "FileNotFound");
                return;
            }

            server.streamFile(log, "plain/text");

            log.close();
        });

    server.on("/trunc", HTTP_GET, []() {
            if (!logFile.truncate(0))
            {
                server.send(500, "text/plain", "Failed to truncate log file");
            }
            else
            {
                server.sendHeader("Location", String("/?message=Log+file+truncated..."), true);
                server.send ( 302, "text/plain", "");
                //server.send(200, "text/plain", "Successuflly truncated log file");
            }
    });

    server.on("/restart", HTTP_GET, []() {
            server.send(200, "text/plain", "Restarting...");
            delay(500);
            ESP.restart();
    });

    server.begin();
}

void Logger::on_loop()
{
    server.handleClient();
}

void Logger::log(const String& log_message)
{
    Serial.println(log_message.c_str());

    // Write to the log file.
    if (logFile)
    {
        logFile.write((log_message + "\n").c_str());
    }
}

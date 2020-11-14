#include "ArduinoJson.h"
#include "ArduinoLog.h"

#if defined(ARDUINO_ARCH_ESP32)
#include <ESPmDNS.h>
#elif defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266mDNS.h>
// MDNSResponder::hMDNSService hMDNSService;
#endif

#include "hasp_conf.h"

#include "hasp_config.h"
#include "hasp_debug.h"

uint8_t mdnsEnabled = true;

void mdnsSetup()
{
    // mdnsSetConfig(settings);
    mdnsStart();
    Log.verbose(TAG_MDNS, F("Setup Complete"));
}

void mdnsStart()
{
#if HASP_USE_MDNS > 0
    if(mdnsEnabled) {

#if HASP_USE_MQTT > 0
        String hasp2Node = mqttGetNodename();
#else
        String hasp2Node = "unknown";
#endif
        // Setup mDNS service discovery if enabled
        /*if(debugTelnetEnabled) {
        }
        return;
        char buffer[128];
        snprintf_P(buffer, sizeof(buffer), PSTR("%u.%u.%u"), HASP_VERSION_MAJOR, HASP_VERSION_MINOR,
                   HASP_VERSION_REVISION);
        MDNS.addServiceTxt(hasp2Node, "tcp", "app_version", buffer); */
        if(MDNS.begin(hasp2Node.c_str())) {
            MDNS.addService(F("http"), F("tcp"), 80);
            MDNS.addService(F("telnet"), F("tcp"), 23);
            MDNS.addServiceTxt(hasp2Node, F("tcp"), F("app_name"), F("HASP-lvgl"));
            /*
                        addService("arduino", "tcp", port);
                        addServiceTxt("arduino", "tcp", "tcp_check", "no");
                        addServiceTxt("arduino", "tcp", "ssh_upload", "no");
                        addServiceTxt("arduino", "tcp", "board", ARDUINO_BOARD);
                        addServiceTxt("arduino", "tcp", "auth_upload", (auth) ? "yes" : "no");*/
            Log.notice(TAG_MDNS, F("Responder started"));
        } else {
            Log.error(TAG_MDNS, F("Responder failed to start %s"), hasp2Node.c_str());
        };
    }
#endif
}

void mdnsLoop()
{
#if defined(ARDUINO_ARCH_ESP8266)
    if(mdnsEnabled) {
        MDNS.update();
    }
#endif
}

void mdnsStop()
{
#if HASP_USE_MDNS > 0
    MDNS.end();
#endif
}

bool mdnsGetConfig(const JsonObject & settings)
{
    bool changed = false;

    if(mdnsEnabled != settings[FPSTR(F_CONFIG_ENABLE)].as<bool>()) changed = true;
    settings[FPSTR(F_CONFIG_ENABLE)] = mdnsEnabled;

    if(changed) configOutput(settings);
    return changed;
}

/** Set MDNS Configuration.
 *
 * Read the settings from json and sets the application variables.
 *
 * @note: data pixel should be formated to uint32_t RGBA. Imagemagick requirements.
 *
 * @param[in] settings    JsonObject with the config settings.
 **/
bool mdnsSetConfig(const JsonObject & settings)
{
    configOutput(settings);
    bool changed = false;

    changed |= configSet(mdnsEnabled, settings[FPSTR(F_CONFIG_ENABLE)], F("mdnsEnabled"));

    return changed;
}
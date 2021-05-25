#include <FreematicsPlus.h>

FreematicsESP32 sys;

const char * cert = \
"-----BEGIN CERTIFICATE-----\n"\
"MIIB4TCCAYigAwIBAgIQAopd7dzGHqUYJXLAxjVegTAKBggqhkjOPQQDAjBgMS0w\n"\
"KwYDVQQDDCRCaXRkZWZlbmRlciBQZXJzb25hbCBDQS5hdmZyZWUwMDAwMDAxDDAK\n"\
"BgNVBAsMA0lEUzEUMBIGA1UECgwLQml0ZGVmZW5kZXIxCzAJBgNVBAYTAlVTMB4X\n"\
"DTIwMDgyMTA5NDAwMFoXDTIwMDgyOTA5NDAwMFowbjELMAkGA1UEBhMCQVQxDzAN\n"\
"BgNVBAgMBlZpZW5uYTEPMA0GA1UEBwwGVmllbm5hMREwDwYDVQQKDAhTZWN1cml0\n"\
"eTEUMBIGA1UECwwLRGV2ZWxvcG1lbnQxFDASBgNVBAMMC2V4YW1wbGUuY29tMFkw\n"\
"EwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEU6t4i4rQM6F+hHPZExnUDE+DXH+mCzz9\n"\
"NgaQZCtDa67YZhmdNVb0ycVjDtEnym/yEaDLmkDO7FXPGz1NqKNLK6MWMBQwEgYD\n"\
"VR0RAQH/BAgwBocEoSPD4jAKBggqhkjOPQQDAgNHADBEAiA6obVEISPZpuIYyRzi\n"\
"GNWFM+ryAAfNP/7o7PYtHOpuvQIgIcscXIOihOkJHDAfQCVBDwilZS6dElY24R9/\n"\
"x0JdiQw=\n"\
"-----END CERTIFICATE-----\n";
char buf[50];
void setup(){
    Serial.begin(115200);
    // use following for Freematics ONE+
    sys.begin();
    // use following for Freematics Esprit or other ESP32 dev board
    //sys.xbBegin(115200, 16, 17);
    Serial.println("List available certificates");
    sys.xbWrite("AT+CCERTLIST");
    char recBytes[100];
    sys.xbRead(recBytes, 100);
    Serial.println(recBytes);

    Serial.println();
    Serial.println("Add our new certificate");
    sprintf(buf,"AT+CCERTDOWN=\”example_com.pem\”,%u",strlen(cert));
    sys.xbWrite(buf);

    Serial.println("List available certificates");
    sys.xbWrite("AT+CCERTLIST");
    
    sys.xbRead(recBytes, 100);
    Serial.println(recBytes);
}

void loop(){


}

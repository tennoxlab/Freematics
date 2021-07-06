#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <SPIFFS.h>

#include <string>
using namespace std;
// #include <fstream>
//     using std::ofstream;
// #include <iostream>
//     using std::cout;
//     using std::endl;

class CStorage;

class CStorage {
public:
    virtual bool init() { return true; }
    virtual void uninit() {}
    virtual void log(uint16_t pid, int value)
    {
        char buf[24];
        byte len = sprintf(buf, "%X%c%d", pid, m_delimiter, value);
        dispatch(buf, len);
    }
    virtual void log(uint16_t pid, uint32_t value)
    {
        char buf[24];
        byte len = sprintf(buf, "%X%c%u", pid, m_delimiter, value);
        dispatch(buf, len);
    }
    virtual void log(uint16_t pid, float value)
    {
        char buf[24];
        byte len = sprintf(buf, "%X%c%f", pid, m_delimiter, value);
        dispatch(buf, len);
    }
    virtual void log(uint16_t pid, float value[])
    {
        char buf[48];
        byte len = sprintf(buf, "%X%c%.2f;%.2f;%.2f", pid, m_delimiter, value[0], value[1], value[2]);
        dispatch(buf, len);
    }
    virtual void timestamp(uint32_t ts)
    {
        log(0, ts);
    }
    virtual void purge() { m_samples = 0; }
    virtual uint16_t samples() { return m_samples; }
    virtual void dispatch(const char* buf, byte len)
    {
        // output data via serial
        Serial.write((uint8_t*)buf, len);
        Serial.write(' ');
        m_samples++;
    }

protected:
    byte checksum(const char* data, int len)
    {
        byte sum = 0;
        for (int i = 0; i < len; i++) sum += data[i];
        return sum;
    }
    virtual void header(const char* devid) {}
    virtual void tailer() {}
    uint16_t m_samples = 0;
    char m_delimiter = ':';
};

class CStorageRAM: public CStorage {
public:
    bool init(unsigned int cacheSize)
    {
      if (m_cacheSize != cacheSize) {
        uninit();
        m_cache = new char[m_cacheSize = cacheSize];
      }
      return true;
    }
    void uninit()
    {
        if (m_cache) {
            delete m_cache;
            m_cache = 0;
            m_cacheSize = 0;
        }
    }
    void purge() { m_cacheBytes = 0; m_samples = 0; }
    unsigned int length() { return m_cacheBytes; }
    char* buffer() { return m_cache; }
    void dispatch(const char* buf, byte len)
    {
        // reserve some space for checksum
        int remain = m_cacheSize - m_cacheBytes - len - 3;
        if (remain < 0) {
          // m_cache full
          return;
        }
        // store data in m_cache
        memcpy(m_cache + m_cacheBytes, buf, len);
        m_cacheBytes += len;
        m_cache[m_cacheBytes++] = ',';
        m_samples++;
    }

    void header(const char* devid)
    {
        m_cacheBytes = sprintf(m_cache, "%s#", devid);
    }
    void tailer()
    {
        //if (m_cache[m_cacheBytes - 1] == ',') m_cacheBytes--;
        m_cacheBytes += sprintf(m_cache + m_cacheBytes, "*%X", (unsigned int)checksum(m_cache, m_cacheBytes));
    }
    void untailer()
    {
        char *p = strrchr(m_cache, '*');
        if (p) {
            *p = ',';
            m_cacheBytes = p + 1 - m_cache;
        }
    }
    void print(){
        //this is only for debugging purposes
        m_cache[m_cacheBytes] = '\0';
        Serial.println(m_cache);
    }
protected:
    unsigned int m_cacheSize = 0;
    unsigned int m_cacheBytes = 0;
    char* m_cache = 0;
};

class FileLogger : public CStorage {
public:
    FileLogger() { m_delimiter = ','; }
    virtual void dispatch(const char* buf, byte len)
    {
        if (m_id == 0) return;

        if (m_file.write((uint8_t*)buf, len) != len) {
            // try again
            if (m_file.write((uint8_t*)buf, len) != len) {
                Serial.println("Error writing. End file logging.");
                end();
                return;
            }
        }
        m_file.write('\n');
        m_size += (len + 1);
    }
    virtual uint32_t size()
    {
        return m_size;
    }
    virtual void end()
    {
        m_file.close();
        m_id = 0;
        m_size = 0;
    }
    virtual void flush()
    {
        m_file.flush();
    }

    virtual int openForRead(){
        if(m_file)
            m_file.close(); //close only the file without resetting m_size and m_id
        return 0;
    }

    virtual void readFiletoBuf(CStorageRAM *store){
        char buf[50];
        int lencounter = 0;
        while(m_file_read.available()){
            buf[lencounter] = m_file_read.read();
            if(buf[lencounter] == '\n'){
                store->dispatch(buf,lencounter);
                //check if that's the timestamp, if yes then stop this process.
                if((buf[0] == '0') && (buf[1] == m_delimiter)){
                    break;
                }
                lencounter = 0;

            }
            else{
                lencounter++;
            }
        }
        m_lockedForRead = 0;
    }

    virtual boolean isFileAvailable(){
        if(m_file_read){
            if(m_file_read.available()){
                return true;
            }
            else{
                //delete the file, since we have no use of it now.
                Serial.println("Removing file");
                m_file_read.close();
                char buf[32];
                sprintf(buf, "/DATA/%u.CSV", m_id_read);
                SPIFFS.remove(buf);
                Serial.println("Removing file Done");
                return false;
            }
        }
        return false;
    }
protected:
    int getFileID(File& root)
    {
        if (root) {
            File file;
            int id = 0;
            while(file = root.openNextFile()) {
                Serial.println(file.name());
                if (!strncmp(file.name(), "/DATA/", 6)) {
                    unsigned int n = atoi(file.name() + 6);
                    if (n > id) id = n;
                }
            }
            return id + 1;
        } else {
            return 1;
        }
    }
    uint32_t m_dataTime = 0;
    uint32_t m_dataCount = 0;
    uint32_t m_size = 0;
    uint32_t m_id = 0;
    File m_file;

    uint32_t m_id_read = 0;
    File m_file_read;
    int      m_lockedForRead = 0;
};

class SDLogger : public FileLogger {
public:
    bool init()
    {
        SPI.begin();
        if (SD.begin(PIN_SD_CS, SPI, SPI_FREQ)) {
            unsigned int total = SD.totalBytes() >> 20;
            unsigned int used = SD.usedBytes() >> 20;
            Serial.print("SD:");
            Serial.print(total);
            Serial.print(" MB total, ");
            Serial.print(used);
            Serial.println(" MB used");
            return true;
        } else {
            Serial.println("NO SD CARD");
            return false;
        }
    }
    uint32_t begin()
    {
        File root = SD.open("/DATA");
        m_id = getFileID(root);
        SD.mkdir("/DATA");
        char path[24];
        sprintf(path, "/DATA/%u.CSV", m_id);
        Serial.print("File: ");
        Serial.println(path);
        m_file = SD.open(path, FILE_WRITE);
        if (!m_file) {
            Serial.println("File error");
            m_id = 0;
        }
        m_dataCount = 0;
        return m_id;
    }
    void flush()
    {
        char path[24];
        sprintf(path, "/DATA/%u.CSV", m_id);
        m_file.close();
        m_file = SD.open(path, FILE_APPEND);
        if (!m_file) {
            Serial.println("File error");
        }
    }

    int openForRead(){
        Serial.print("Opening for read ");
        Serial.println((m_id_read+1));
        int result = 0;
        char path[24];
        m_id_read = m_id;

        if(m_file){
            //close current file and open new file for writing. So we dont miss any data
            m_file.close(); //close only the file without resetting m_size and m_id
            sprintf(path, "/DATA/%u.CSV", ++m_id);
            m_file = SD.open(path, FILE_APPEND);
            if (!m_file) {
                Serial.println("File error");
                result = -1;
            }
        }
        sprintf(path, "/DATA/%u.CSV", m_id_read);
        m_file_read = SD.open(path,FILE_READ);
        if (!m_file_read) {
            Serial.println("File error");
            result = -1;
        }else{
            m_lockedForRead = 1;
        }
        m_size = 0;

        return result;
    }


    void logln(string msg)
    {
        log("[Log] " + msg + "\n");
    }
    void log(string msg)
    {
        // Serial.print("[Log] ");
        Serial.print(msg.c_str());

        // print to file
        File log_file;
        char path[24];
        sprintf(path, "/DATA/%u.LOG", m_id);
        log_file = SD.open(path, FILE_APPEND);

        if (!log_file) {
            Serial.println();
            Serial.print("[Log] file error:");
            Serial.println(path);
        } else {
            log_file.println(msg.c_str());
        }
        log_file.close();
    }
};

class SPIFFSLogger : public FileLogger {
public:
    bool init()
    {
        bool mounted = SPIFFS.begin();
        if (!mounted) {
            Serial.println("Formatting SPIFFS...");
            mounted = SPIFFS.begin(true);
        }
        if (mounted) {
            Serial.print("SPIFFS:");
            Serial.print(SPIFFS.totalBytes());
            Serial.print(" bytes total, ");
            Serial.print(SPIFFS.usedBytes());
            Serial.println(" bytes used");
        } else {
            Serial.println("No SPIFFS");
        }
        return mounted;
    }
    uint32_t begin()
    {
        File root = SPIFFS.open("/");
        m_id = getFileID(root);
        char path[24];
        sprintf(path, "/DATA/%u.CSV", m_id);
        Serial.print("File: ");
        Serial.println(path);
        m_file = SPIFFS.open(path, FILE_WRITE);
        if (!m_file) {
            Serial.println("File error");
            m_id = 0;
        }
        m_dataCount = 0;
        return m_id;
    }

    int openForRead(){
        Serial.print("Opening for read ");
        Serial.println((m_id_read+1));
        int result = 0;
        char path[24];
        m_id_read = m_id;

        if(m_file){
            //close current file and open new file for writing. So we dont miss any data
            m_file.close(); //close only the file without resetting m_size and m_id
            sprintf(path, "/DATA/%u.CSV", ++m_id);
            m_file = SPIFFS.open(path, FILE_APPEND);
            if (!m_file) {
                Serial.println("File error");
                result = -1;
            }
        }
        sprintf(path, "/DATA/%u.CSV", m_id_read);
        m_file_read = SPIFFS.open(path,FILE_READ);
        if (!m_file_read) {
            Serial.println("File error");
            result = -1;
        }else{
            m_lockedForRead = 1;
        }
        m_size = 0;

        return result;
    }



private:
    void purge()
    {
        // remove oldest file when unused space is insufficient
        File root = SPIFFS.open("/");
        File file;
        int idx = 0;
        while(file = root.openNextFile()) {
            if (!strncmp(file.name(), "/DATA/", 6)) {
                unsigned int n = atoi(file.name() + 6);
                if (n != 0 && (idx == 0 || n < idx)) idx = n;
            }
        }
        if (idx) {
            m_file.close();
            char path[32];
            sprintf(path, "/DATA/%u.CSV", idx);
            SPIFFS.remove(path);
            Serial.print(path);
            Serial.println(" removed");
            sprintf(path, "/DATA/%u.CSV", m_id);
            m_file = SPIFFS.open(path, FILE_APPEND);
            if (!m_file) m_id = 0;
        }
    }
};

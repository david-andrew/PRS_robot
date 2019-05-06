#include <EEPROM.h>

#define DEFAULT_LASER_ALIGNMENT_OFFSET -52      //number of steps offset from slot positions to the laser axis location
#define LASER_ALIGNMENT_ADDRESS 0
#define DEFAULT_GLUE_ALIGNMENT_OFFSET 4436      //number of steps offset from slot positions to the glue needle location
#define GLUE_ALIGNMENT_ADDRESS 4
#define DEFAULT_PRESS_ALIGNMENT_OFFSET 13510    //number of steps offset from slot positions to the press arm location
#define PRESS_ALIGNMENT_ADDRESS 8

#define EEPROM_TOLERANCE 1000           //if the value in EEPROM memory deviates more than this much, use default instead

int32_t LASER_ALIGNMENT_OFFSET;// = -52;      //number of steps offset from slot positions to the laser axis location
int32_t GLUE_ALIGNMENT_OFFSET;// = 4436;      //number of steps offset from slot positions to the glue needle location
int32_t PRESS_ALIGNMENT_OFFSET;// = 13510;    //number of steps offset from slot positions to the press arm location

void setup()
{
    Serial.begin(115200);

//    //store the defaults to memory
//    EEPROM.put(LASER_ALIGNMENT_ADDRESS, (int32_t) DEFAULT_LASER_ALIGNMENT_OFFSET);
//    EEPROM.put(GLUE_ALIGNMENT_ADDRESS,  (int32_t) DEFAULT_GLUE_ALIGNMENT_OFFSET);
//    EEPROM.put(PRESS_ALIGNMENT_ADDRESS, (int32_t) DEFAULT_PRESS_ALIGNMENT_OFFSET);
//    
    //read from memory
    EEPROM.get(LASER_ALIGNMENT_ADDRESS, LASER_ALIGNMENT_OFFSET);
    if (abs(LASER_ALIGNMENT_OFFSET - DEFAULT_LASER_ALIGNMENT_OFFSET) > EEPROM_TOLERANCE)
    {
      Serial.println("ERROR: EEPROM stored value for LASER alignment appears to be incorrect.");
      Serial.println("    Found: " + String(LASER_ALIGNMENT_OFFSET) + ".");
      Serial.println("    Using default value: " + String(DEFAULT_LASER_ALIGNMENT_OFFSET) + ".");
    }

    EEPROM.get(GLUE_ALIGNMENT_ADDRESS, GLUE_ALIGNMENT_OFFSET);
    if (abs(GLUE_ALIGNMENT_OFFSET - DEFAULT_GLUE_ALIGNMENT_OFFSET) > EEPROM_TOLERANCE)
    {
      Serial.println("ERROR: EEPROM stored value for GLUE alignment appears to be incorrect.");
      Serial.println("    Found: " + String(GLUE_ALIGNMENT_OFFSET) + ".");
      Serial.println("    Using default value: " + String(DEFAULT_GLUE_ALIGNMENT_OFFSET) + ".");
    }

    EEPROM.get(PRESS_ALIGNMENT_ADDRESS, PRESS_ALIGNMENT_OFFSET);
    if (abs(PRESS_ALIGNMENT_OFFSET - DEFAULT_PRESS_ALIGNMENT_OFFSET) > EEPROM_TOLERANCE)
    {
      Serial.println("ERROR: EEPROM stored value for PRESS alignment appears to be incorrect.");
      Serial.println("    Found: " + String(PRESS_ALIGNMENT_OFFSET) + ".");
      Serial.println("    Using default value: " + String(DEFAULT_PRESS_ALIGNMENT_OFFSET) + ".");
    }


    Serial.println("\n");
    Serial.println(String(LASER_ALIGNMENT_OFFSET) + " " + String(GLUE_ALIGNMENT_OFFSET) + " " + String(PRESS_ALIGNMENT_OFFSET));
}

void loop()
{
}

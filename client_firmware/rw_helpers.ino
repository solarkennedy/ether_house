#define DEBUG_EEPROM 0

#include <util/crc16.h>

// Read Write Helpers for eeprom management

#define STATE_ADDR       0
#define STATE_SIZE       1
#define TARGET_IP_ADDR   (STATE_ADDR + STATE_SIZE)
#define TARGET_IP_SIZE   4
#define CHECKSUM_ADDR    (TARGET_IP_ADDR + TARGET_IP_SIZE)
#define CHECKSUM_SIZE    2
#define EEPROM_DATA_SIZE CHECKSUM_ADDR
#define EEPROM_SIZE      (CHECKSUM_ADDR + CHECKSUM_SIZE)

uint16_t crc_calc(const uint8_t *data, int len)
{
   uint16_t crc = 0;
   int index;

   for (index = 0; index < len; index++)
      crc = _crc16_update(crc, data[index]);

   return crc;
}

uint8_t crc_write(uint8_t *data, int len)
{
  uint16_t crc;

  crc = crc_calc(data, len);
#if DEBUG_EEPROM
  {
    char buf[5];

    Serial.print(F("Recalc CRC: "));
    sprintf(buf, "%04x", (unsigned int)crc);
    Serial.println(buf);
  }
#endif
  data[len] = crc & 0xff;
  data[len + 1] = (crc >> 8) & 0xff;
}

#if DEBUG_EEPROM
void dump_cur_state(void)
{
  char buf[9];

  sprintf(buf, BYTETOBINARYPATTERN, BYTETOBINARY(state));
  Serial.print(F("State: "));
  Serial.print(buf);
  Serial.println();
  Serial.print(F("IP: "));
  ether.printIp(target_ip);
  Serial.println();
}

void dump_buf(const uint8_t *data, int len)
{
  int i;
  char buf[4];

  for (i = 0; i < len; i++) {
    sprintf(buf, "%02x ", data[i]);
    Serial.print(buf);
  }
  Serial.println();
}
#endif

void saveStateToEeprom() {
  int i;
  uint8_t eeprom_cur[EEPROM_SIZE];
  uint8_t eeprom_new[EEPROM_SIZE];

  eeprom_new[STATE_ADDR] = state;
  memcpy(&eeprom_new[TARGET_IP_ADDR], target_ip, TARGET_IP_SIZE);
  crc_write(eeprom_new, EEPROM_DATA_SIZE);

#if DEBUG_EEPROM
  Serial.println(F("Saving to EEPROM:"));
  dump_cur_state();
  Serial.println("EEPROM new data:");
  dump_buf(eeprom_new, EEPROM_SIZE);
#endif

  cli();
  for (i = 0; i < EEPROM_SIZE; i++)
    eeprom_cur[i] = EEPROM.read(i);
  sei();

#if DEBUG_EEPROM
  Serial.println("EEPROM current data:");
  dump_buf(eeprom_cur, EEPROM_SIZE);
  Serial.print(F("Writing: < "));
  for (i = 0; i < EEPROM_SIZE; i++) {
    if (eeprom_new[i] != eeprom_cur[i]) {
      Serial.print(i);
      Serial.print(F(" "));
    }
  }
  Serial.println(F(">"));
#endif

  cli();
  for (i = 0; i < EEPROM_SIZE; i++)
    if (eeprom_new[i] != eeprom_cur[i])
      EEPROM.write(i, eeprom_new[i]);
  sei();
}

void readStateFromEeprom() {
  int i;
  uint8_t eeprom[EEPROM_SIZE];

  cli();
  for (i = 0; i < EEPROM_SIZE; i++)
    eeprom[i] = EEPROM.read(i);
  sei();

#if DEBUG_EEPROM
  Serial.println("EEPROM raw data:");
  dump_buf(eeprom, EEPROM_SIZE);
#endif

  if (crc_calc(eeprom, EEPROM_SIZE)) {
#if DEBUG_EEPROM
    Serial.println(F("EEPROM read: CRC mismatch, loading defaults"));
#endif
    state = 0;
    memcpy(target_ip, allOnes, sizeof(target_ip));
  } else {
#if DEBUG_EEPROM
    Serial.println(F("EEPROM read: CRC OK..."));
#endif
    state = eeprom[STATE_ADDR];
    memcpy(target_ip, &eeprom[TARGET_IP_ADDR], sizeof(target_ip));
  }

#if DEBUG_EEPROM
    Serial.println(F("EEPROM load results:"));
    dump_cur_state();
#endif

  sync_leds();
}

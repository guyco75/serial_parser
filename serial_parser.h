#ifndef SERIAL_PARSER_H
#define SERIAL_PARSER_H

char rs_str[SERIAL_OUT_BUF_SIZE];
#define serial_out(_s, _f...) \
  do { \
    snprintf((_s), sizeof(_s), _f); \
    Serial.println(_s); \
  } while (0);

struct serial_parser {
  char rx_cmd_str[SERIAL_CMD_BUF_SIZE];
  char *p, *a, *b;
  bool in_sync;

  inline char* get_next_token() {
    a = b;
    while (*b != ',' && *b != '\0')
      b++;

    if (*b == ',') {
      *b = '\0';
      b = b+1;
    }
    return a;
  }

  bool get_next_token_int(int32_t *val) {
    char *s, *p;
    s = get_next_token();
    *val = strtol(s, &p, 10);
    return (p != s && *p == '\0');
  }

  inline bool verify_ending() {
    return (a == p);
  }

  //returns true if a command is available
  bool process_serial() {
    while (Serial.available()) {
      char c = Serial.read();

      if (c == '$') {
        p = rx_cmd_str;
        in_sync = true;
        continue;
      }

      if (in_sync) {
        if (c == '#') {
          *p = '\0';
          a = b = rx_cmd_str;
          in_sync = false;
          return true;
        } else {
          if (p-rx_cmd_str < sizeof(rx_cmd_str)-1) {
            *p = c;
            p++;
          } else {
            in_sync = false;
          }
        }
      }
    }
    return false;
  }

  bool simulate_serial_in(const char* str) {
    if (strlen(str) < sizeof(rx_cmd_str)) {
      snprintf(rx_cmd_str, sizeof(rx_cmd_str), "%s", str);
      a = b = rx_cmd_str;
      in_sync = false;
      return true;
    } else {
      rx_cmd_str[0] = '\0';
      return false;
    }
  }
};

serial_parser ser_parser;

#endif


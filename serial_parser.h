#ifndef SERIAL_PARSER_H
#define SERIAL_PARSER_H

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

  long int get_next_token_int() {
    return strtol(get_next_token(), NULL, 10);
  }

  inline boolean verify_ending() {
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
};

#endif


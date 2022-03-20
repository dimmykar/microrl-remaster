#include "esp8266_misc.h"

/*
ESP8266 platform specific implementation routines.
*/
#define _ESP8266_DEMO_VER    "1.0"

#define _LOGIN        "admin"
#define _PASSWORD     "1234"


// definition commands word
#define _CMD_HELP     "help"
#define _CMD_CLEAR    "clear"
#define _CMD_LOGIN    "login"

//*****************************************************************************
void print(microrl_t* mrl, const char* str) {
    MICRORL_UNUSED(mrl);

    Serial.print(str);
}

//*****************************************************************************
void print_help(microrl_t* mrl) {
    print(mrl, "clean              - clear screen\n\r");
    print(mrl, "login YOUR_LOGIN   - admin in this example\n\r");
    print(mrl, "if login is correct, you will be asked to enter password.\n\r");
}

//*****************************************************************************
// execute callback for microrl library
// do what you want here, but don't write to argv!!! read only!!
int execute(microrl_t* mrl, int argc, const char* const *argv) {
    static int pass_word = 0;
    int i = 0;
    // just iterate through argv word and compare it with your commands
    while (i < argc) {
        if (strcmp (argv[i], _CMD_HELP) == 0) {
            print(mrl, "microrl v");
            print(mrl, MICRORL_LIB_VER);
            print(mrl, "\r\nESP8266 DEMO v");
            print(mrl, _ESP8266_DEMO_VER);
            print(mrl, "\n\r");
            print_help(mrl);        // print help
        } else if (strcmp (argv[i], _CMD_CLEAR) == 0) {
            print(mrl, "\033[2J");    // ESC seq for clear entire screen
            print(mrl, "\033[H");     // ESC seq for move cursor at left-top corner
        } else if (strcmp (argv[i], _CMD_LOGIN) == 0) {
            if (++i < argc) {
                if (strcmp (argv[i], _LOGIN) == 0) {
                    print(mrl, "Enter your password:\r\n");
                    microrl_set_echo(prl, MICRORL_ECHO_ONCE);
                    pass_word = 1;
                    return 0;
                } else {
                    print(mrl, "Wrong login name. try again.\r\n");
                    return 1;
                }
            } else {
                print(mrl, "Enter your login after command login.\r\n");
                return 0;
            }
        } else if (pass_word == 1) {
            if (strcmp(argv[i], _PASSWORD) == 0) {
                print(mrl, "Grate You Log In!!!\r\n");
                pass_word = 0;
                return 0;
            } else {
                print(mrl, "Wrong password, try log in again.\r\n");
                pass_word = 0;
                return 1;
            }
        } else {
            print(mrl, "Command: '");
            print(mrl, (char*)argv[i]);
            print(mrl, "' Not found.\n\r");
        }
        i++;
    }
    return 0;
}

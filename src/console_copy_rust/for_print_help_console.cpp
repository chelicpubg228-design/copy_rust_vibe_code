#include "../system_rust/copy_rust.h"


void kop_rast::print_help_console_cop_rust() {
        print(R"HELP(
Type a command and press Enter.

------------------------------------------------------------
  GENERAL
------------------------------------------------------------
  help              - show this help
  cls / clear       - clear the screen
  exit              - close console, return to game

------------------------------------------------------------
  INFO
------------------------------------------------------------
  stat              - HP / food / water / energy

  status            - HP / food / water / energy - table, min and max status  
------------------------------------------------------------
  COMMANDS
------------------------------------------------------------
  tp                - teleport (asks x and y)
  user_change       - edit your stats:
                        h - hp
                        d - damage of stone axe
                        e - energy
                        w - water
                        f - food
  setting           - open settings (name, reset)


------------------------------------------------------------
  NOTES
------------------------------------------------------------
  * Console bypasses normal game rules.
  * If HP >= 250 when saving -> config deleted.
  * Commands are case-sensitive.
  * Unknown command -> "error syntaxis".
)HELP");
}
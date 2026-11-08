#include "copy_rust.h"

const std::string kop_rast::help_text = R"HELP(

   MAIN MENU (type number + Enter):
    1  - info              show all resources and stats
    2  - exit              save and close the game
    3  - sleep             +75 energy (10 sec)
    4  - help              this message
    5  - craft             open craft menu
    6  - don armor         put on leather armor
    7  - undon armor       take off leather armor
    8  - statistic user    HP / food / water / energy
    9  - statistic axe     axes and their durability
    10 - free mode         walk, explore, gather, hunt
    11 - settings          name / reset / help
    12 - console           cheat console (tp, stat, etc.)
    13 - min resources     bar chart of resources
    14 - status table      bar chart of HP/food/water/energy
    15 - mini game         guess the number (1..100)

  Extra commands (type in main menu):
    help   - this help
    clear  - clear screen
    cls    - clear screen

------------------------------------------------------------
  FREE MODE (option 10)
------------------------------------------------------------
  w - x+     s - x-     d - y+     a - y-
  e - inspect area (resources and animals are found
      ONLY by pressing e)
  ` - open console (backtick, not tilde)
  q - back to main menu (saves progress)

  When asked "y or n":
    y - accept (gather / fight)
    n - decline

------------------------------------------------------------
  CHARACTER
------------------------------------------------------------
  HP 0     -> death
  Food 0   -> death
  Water 0  -> death
  Energy -100 -> death

  HP regen works when food >= 70 and water >= 70.
  Regen: +10 HP per menu tick (costs 10 food + 10 water).
  Sleep: +75 energy (10 sec wait).

------------------------------------------------------------
  RESOURCES
------------------------------------------------------------
  tree   - wood       (axe)
  stown  - stone      (pick or axe)
  metal  - iron       (pick)
  sera   - sulfur     (pick)
  berry  - berries    (hands)
  tkan   - cloth      (hands)
  bone, meet, kosha   (from animals)
  nitki  - threads    (craft from cloth)

------------------------------------------------------------
  CRAFTING (option 5)
------------------------------------------------------------
  1. Stone axe        = 5 wood + 1 stone
  2. Iron axe         = 5 wood + 5 iron
  3. Leather helmet   = 3 cloth + 3 threads + 3 leather
  4. Leather bib      = 5 cloth + 6 threads + 10 leather
  5. Leather leggings = 4 cloth + 5 threads + 4 leather
  6. Leather boots    = 3 cloth + 3 threads + 5 leather
  7. Threads          = 5 cloth

  NOTE: only options 1 and 2 are wired up in the current build.
  Options 3-7 are planned but not yet connected to the menu.

------------------------------------------------------------
  ARMOR (leather)
------------------------------------------------------------
  Helmet +10 max HP    Bib    +20 max HP
  Leggings +15 max HP  Boots  +5 max HP
  Each piece has its own durability and breaks.

------------------------------------------------------------
  COMBAT
------------------------------------------------------------
  Meet animal: 1 - run away   2 - fight
  Weapon:  1 stone axe (15)   2 iron axe (25)
           3 iron pick (35)    4 torch (10)

  Mouse cannot be killed - it only costs HP.
  Rewards: bones, meat, leather, cloth.

------------------------------------------------------------
  CONSOLE (option 12)
------------------------------------------------------------
  help              - console help
  stat              - HP / food / water / energy
  status            - HP / food / water / energy (bars)
  tp                - teleport (asks x and y)
  user_change       - edit stats (hp / damage / energy /
                      water / food)
  setting           - open settings
  cls / clear       - clear screen
  exit              - close console, return to game

------------------------------------------------------------
  SETTINGS (option 11)
------------------------------------------------------------
  h - show settings help
  n - change player name
  r - remove save file (config) and start from scratch
  q - back to main menu

------------------------------------------------------------
  TIPS
------------------------------------------------------------
  1. Watch food and water - they drop every action.
  2. Sleep often - energy runs out fast.
  3. In free mode press 'e' often - that's how you
     find everything.
  4. First craft: stone axe (5 wood + 1 stone).
  5. Berries restore HP, food, water, energy.
  6. Save iron for iron axe.
  7. The game autosaves on exit (option 2) and in
     free mode (q).

------------------------------------------------------------
  CHEATS
------------------------------------------------------------
  If HP >= 250 when the config is loaded, the config
  is deleted and progress resets.
  Console allows editing stats and teleporting.

------------------------------------------------------------
              Good luck surviving!
------------------------------------------------------------
)HELP";

const std::string kop_rast::help_text_for_set = R"HELP(
  +==========================================================+
|                      SETTINGS HELP                       |
+==========================================================+

  Type a letter and press Enter.

  h - show this help
  n - change your player name
  r - remove the save file (config) and start from scratch
  q - leave settings and go back to the main menu

------------------------------------------------------------
  WARNING
------------------------------------------------------------
  r cannot be undone: all resources, tools, armor,
  coordinates and progress are lost. The game will ask
  you to confirm with "y" before deleting anything.
  Any answer other than "y" cancels the action.

  Note: your player name is kept after a reset.
  Use "n" to change it explicitly.
)HELP";
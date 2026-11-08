#ifndef NOMINMAX
#define NOMINMAX
#endif
#pragma once
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iso646.h>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>
#include <conio.h>
#include <filesystem>
#include <windows.h>
#include <initializer_list>
#include <algorithm>

class kop_rast {
public:
  static const std::string help_text;
  static const std::string help_text_for_set;
  bool for_exit_rust = false;
  void console_rust();
  void print_help_console_cop_rust();
  void prov_alive();
  void load_json();
  void ubdate_json();
  void resurs_mine(int &resours, int &hp_resours, long long x_resurs,
                   long long y_result, int time_to_find, int reset_hp,
                   std::string resours_name, int instr,
                   void (kop_rast::*func_update)());
  void locate_sera();
  void rybka_tree_kamen_topor_big();
  void rybka_tree_kamen_topor_normal();
  void rybka_tree_kamen_topor_small();
  void stone_process();
  void time(int time_cin);
  void min_res();
  void hp_minys();
  void ate_berry();
  void sleep();
  void setting_user();
  template <typename T> void print(T &&print_str) {
    std::cout << print_str << std::endl;
  }
  void locate_tkan();
  void hunting();
  void craft_boots_leather();
  void craft_leggings_leather();
  void locate_iron();
  void max_min_pers();
  void craft_bib_leather();
  void craft_helmet_leather();
  void vstr_wolf();
  void vstr_bear();
  void prov_coord();
  void vstr_mouse();
  void mouse_coordinats();

  void craft_threads();
  void vstr_olen();
  void fight(long long y_animal, int &hp_animal, long long x_animal,
             int damage_animal, int reset_hp_animal, std::string name_animal);
  void random_ubdate_();
  void craft_axe_stone();
  void craft_axe_iron();
  void craft();
  void locate_berry();
  void hp_minus_for_fight(int ddmage_for_def_fight, int &hp_bron_for_def_fight);
  void reset();
  void regeneration();
  void prov_instryment_stone_axe_hp();
  void prov_pick_iron_hp();
  void prov_iron_axe_hp();
  void prov_instryments();
  void update_coordinats_sulfrum();
  void update_coordinats_iron();
  void update_coordinats_stone();
  void update_coordinats_big_tree();
  void update_coordinats_normal_tree();
  void update_coordinats_small_tree();
  void update_coordinats_berry();
  void update_coordinats_tkan();
  void loc_shanc();
  bool loc_shanc_auto(long long x_plus, long long y_plus, int shanc_what,
                      int big_whanc_what, long long &x_what, long long &y_what,
                      std::string name_object,
                      char char_for_auto

  );
  void update_coordinats_wolf();
  void update_coordinats_mouse();
  void update_coordinats_bear();
  void update_coordinats_olen();
  void update_coordinats();

  void menu();
  void ygaday_numbers();
  void free_mode();
  void user_change_v();
  void print_menu();
  void update_random_plays();
  void exit_rust();
  void tp_rust_console();
  void print_statisc_user();
  void print_statistic_axe();
  void print_info();
  void craft_bron();
  void leather_minus_for_def_fuction_fight_ok_avtomatisation_for_bear();
  void leather_minus_for_def_fuction_fight_ok_avtomatisation_for_wolf();
  void leather_minus_for_def_fuction_fight_ok_avtomatisation_for_mouse();
  void leather_minus_for_def_fuction_fight_ok_avtomatisation_for_olen();
  void ignore();
  void continue_enter();
  void auto_minus_leathing(int damage_helmet, int damage_bib,
                           int damage_leggings, int damage_boots);
  void proc_for_fight_all_animals(
      std::string name_animal_fight, int damage_animal, int fight_bone,
      int fight_meet, int fight_tkan, int fight_kosha, int &hp_animal_fight,
      int reset_hp_animal_for_fight, int leathing_bib_minus_sample,
      int leathing_helmet_minus_sample, int leathing_boots_minus_sample,
      int leathing_leggints_minus_sample

  );

  void donning_armor();
  void undonning_armor();
  void prov_bron();
  //! рандомные числа для добычи дерева
  int tree_rand_kamen_topor{0};
  int tree_rand_iron_topor{0};
  int tree_rand_MVK_topor{0};
  int tree_chainsaw{0};
  // рандомное время находки дерева
  int tree_time{0};
  std::string jjson;
  // шанс встретить большое дерево
  int tree__big_shans{0};
  int shans_berry{0};
  // шанс втретить среднее дерево

  int tree_normal_shans{0};
  int shans_tkan = 0;
  // время находки камня
  // шанс втретить волка
  int shans_wolf{0};
  // шанс втретить мышь
  int shans_mouse{0};
  // шанс втретить медведя
  int shans_bear{0};
  // шанс втретить оленя
  int shanc_olen{0};
  int shans_n_stoe{0};
  // шанс втретить маленькое дерево
  int tree_small_shans{0};
  //! координаты начало ======
  long long y{0}, x{0};
  // координаты большого дерева

  long long y_big_big_tree{0};
  long long y_normal_big_tree{0};
  long long y_small_big_tree{0};
  long long x_big_big_tree{0};
  long long x_normal_big_tree{0};
  long long x_small_big_tree{0};

  // координаты среднего дерева

  long long y_big_normal_tree{0};
  long long y_normal_normal_tree{0};
  long long y_small_normal_tree{0};
  long long x_big_normal_tree{0};
  long long x_normal_normal_tree{0};
  long long x_small_normal_tree{0};

  // координаты маленького дерева

  long long y_big_small_tree{0};
  long long y_normal_small_tree{0};
  long long y_small_small_tree{0};
  long long x_big_small_tree{0};
  long long x_normal_small_tree{0};
  long long x_small_small_tree{0};

  // камень

  long long y_big_stone{0};
  long long y_normal_stone{0};
  long long y_small_stone{0};
  long long x_big_stone{0};
  long long x_normal_stone{0};
  long long x_small_stone{0};

  // сера

  long long y_big_sulfur{0};
  long long y_normal_sulfur{0};
  long long y_small_sulfur{0};
  long long x_big_sulfur{0};
  long long x_normal_sulfur{0};
  long long x_small_sulfur{0};

  // железо

  long long y_big_iron{0};
  long long y_normal_iron{0};
  long long y_small_iron{0};
  long long x_big_iron{0};
  long long x_normal_iron{0};
  long long x_small_iron{0};

  // волк

  long long y_big_wolf{0};
  long long y_normal_wolf{0};
  long long y_small_wolf{0};
  long long x_big_wolf{0};
  long long x_normal_wolf{0};
  long long x_small_wolf{0};

  // медведь

  long long y_big_bear{0};
  long long y_normal_bear{0};
  long long y_small_bear{0};
  long long x_big_bear{0};
  long long x_normal_bear{0};
  long long x_small_bear{0};

  // олень

  long long y_big_olen{0};
  long long y_normal_olen{0};
  long long y_small_olen{0};
  long long x_big_olen{0};
  long long x_normal_olen{0};
  long long x_small_olen{0};

  // мышь

  long long y_big_mouse{0};
  long long y_normal_mouse{0};
  long long y_small_mouse{0};
  long long x_big_mouse{0};
  long long x_normal_mouse{0};
  long long x_small_mouse{0};

  // ткань

  long long y_big_tkan{0};
  long long y_normal_tkan{0};
  long long y_small_tkan{0};
  long long x_big_tkan{0};
  long long x_normal_tkan{0};
  long long x_small_tkan{0};

  // ягоды

  long long y_big_berries{0};
  long long y_normal_berries{0};
  long long y_small_berries{0};
  long long x_big_berries{0};
  long long x_normal_berries{0};
  long long x_small_berries{0};

  //! координаты конец =====

  // шанс размера дерево
  int tree_razmer_shans{0};
  // время находки серы
  int time_sera{0};
  // шанс встретить руды
  int stown_shans{0};
  int sera_shans{0};
  int metal_shans{0};
  int time_small_tree{0};
  // шанс встретить бочку
  int bochka_shans{0};
  int hp_iron_exa = 140;
  // время нахождения ткани
  int time_tkan = 0;
  int time_normal_tree{0};
  // рандомное число для мини игры

  int random_num_for_play{0};
  //! рандомные числа для камня
  int stown_stown_topor{0};
  int stown_iron_pick{0};
  int stown_MVK_pick{0};
  bool is_bool_leathing_bib = false;
  bool is_bool_leathing_boots = false;
  bool is_bool_leathing_leggings = false;
  bool is_bool_leathing_helmet = false;
  int stown_byr{0};
  int time_to_iron{0};
  //! жизнь всех существ и предметов
  short hp_bochka = 40;
  int hp_person = 100;
  int hp_tree_big = 90;
  int hp_tree_normal = 70;
  int hp_tree_small = 55;
  int hp_stown = 70;
  int hp_sera = 60;
  int hp_iron = 70;
  int hp_berry = 5;
  int hp_pick = 150;
  short eda = 100;
  int hp_bear = 120;
  int hp_mouse = 10;
  int hp_olen = 80;
  int hp_wolf = 100;
  short water = 100;
  short hp_stown_topor = 100;
  long long wins_r;

  //! важные переменные
  int level_user = 0;
  int stown_topor = 1;
  int tree = 0;
  int tkan = 0;
  int verst_level = 0;
  int metal = 0;
  int hp_tkan = 30;
  int skrap = 0;
  int hp_leathing_bib = 50;
  int hp_leathing_leggings = 20;
  int hp_leating_boots = 10;
  int hp_leathing_helmet = 25;
  int MVK = 0;
  int hp_max = 100;
  std::string name_user;
  std::string cin_console_rust;

  int cin_hunting{0};
  int stown = 0;
  int leather_bib = 0;
  int leather_helmet = 0;
  int leather_boots = 0;
  int leather_leggings = 0;
  int voda_butka = 0;
  int fakel = 1;
  int bone = 0;
  int berry = 0;
  int meet = 0;
  int kosha = 0;
  int nitki = 0;
  int byr = 0;
  // уроны предметов  по существам
  // топоры
  int damage_stone_axe_for_animal = 15;
  int damage_iron_axe_for_animal = 25;
  // кирка
  int damage_iron_pick_for_animal = 35;
  // факел
  int damage_fakel_for_animal = 10;
  // волк
  int damage_wolf = 10;
  // медведь
  int damage_bear = 15;
  // олень
  int damage_olen = 1;
  // мышьы
  int damage_mouse = 5;

  int sera = 0;
  int chainsaw = 0;
  int iron_pick = 0;
  int MVk_pick = 0;
  int iron_topor = 0;
  int MVK_topor = 0;
  std::string cin_menu;
  int energy = 100;
  std::string deception;
};

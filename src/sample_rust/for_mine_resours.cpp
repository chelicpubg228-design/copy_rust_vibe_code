#include "../system_rust/copy_rust.h"

void kop_rast::resurs_mine(int& resours, int& hp_resours,
                           long long x_resurs, long long y_result,
                           int time_to_find, int reset_hp,
                           std::string resours_name, int instr,
                           void (kop_rast::*func_update)()) {
  // 1) проверка: стоим ли мы на ресурсе
  if (x_resurs != x or y_result != y) {
    std::cout << "you didn't find it " << resours_name << std::endl;
    eda -= 5;
    water -= 3;
    energy -= 4;
    return;
  }

  random_ubdate_();
  prov_instryments();

  // 2) instr == 3: собираем руками (ягоды, ткань)
  if (instr == 3) {
    std::cout << "Great! you found " << resours_name << ", wait "
              << time_to_find << " time to get there and get it....."
              << std::endl;
    time(time_to_find);
    resours += 5;
    water -= 3;
    eda -= 1;
    energy -= 2;
    std::cout << "+ 5 " << resours_name << std::endl;
    hp_resours -= 5;
    if (hp_resours <= 0) {
      (this->*func_update)();
      hp_resours = reset_hp;
    }
    return;
  }

  // 3) выбор инструмента
  int choice = 0;
  if (instr == 1) {
    std::cout << "Enter topor (1 stown, 2 iron) -> ";
  } else if (instr == 2) {
    std::cout << "Enter pick (1 stown topor, 2 iron pick) -> ";
  } else {
    return;
  }
  std::cin >> choice;
  std::cin.ignore(10000, '\n');

  // which: 0 = каменный топор, 1 = железный топор, 2 = железная кирка
  int which = -1;
  if (choice == 1) which = 0;
  else if (choice == 2) which = (instr == 1) ? 1 : 2;

  if (which < 0) {
    std::cout << "wrong choice" << std::endl;
    return;
  }

  auto count = [&]() -> int {
    return which == 0 ? stown_topor : (which == 1 ? iron_topor : iron_pick);
  };
  auto power = [&]() -> int {
    return which == 0 ? stown_stown_topor
                      : (which == 1 ? tree_rand_iron_topor : stown_iron_pick);
  };
  auto wear = [&]() {
    if (which == 0) hp_stown_topor -= 1;
    else if (which == 1) hp_iron_exa -= 1;
    else hp_pick -= 1;
  };
  const int step_time = (which == 1) ? 5 : 3;
  const int cost = (which == 1) ? 2 : 1;

  if (count() < 1) {
    std::cout << "tool is none!" << std::endl;
    return;
  }

  std::cout << "Great! you found " << resours_name << ", wait "
            << time_to_find << " time to get there and get it....."
            << std::endl;
  time(time_to_find);

  // 4) добываем, пока ресурс не закончится (а не только половину)
  while (hp_resours > 0) {
    random_ubdate_();
    prov_instryments();

    if (count() < 1) {
      std::cout << "tool broke!" << std::endl;
      hp_resours = reset_hp;   // ресурс восстанавливаем, координаты не трогаем
      return;
    }

    time(step_time);
    eda -= cost;
    energy -= cost;
    water -= cost;

    int p = power();
    resours += p;
    hp_resours -= p;
    wear();

    std::cout << "+ " << p << std::endl;
    std::cout << "hp " << (hp_resours > 0 ? hp_resours : 0) << std::endl;
  }

  // ресурс выработан: новые координаты и восстановление hp
  // (system("cls") убран, чтобы было видно результат)  
  (this->*func_update)();
  hp_resours = reset_hp;
}



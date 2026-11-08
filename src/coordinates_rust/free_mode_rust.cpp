#include "../system_rust/copy_rust.h"
 
void kop_rast::free_mode() {
  std::cout << "w - x +" << std::endl;
  std::cout << "s - x -" << std::endl;
  std::cout << "d - y +" << std::endl;
  std::cout << "a - y -" << std::endl;
  std::cout << "e - inspection" << std::endl;
  std::cout << "q - exit" << std::endl;
  std::cout << "~ - console" << std::endl;
      ubdate_json();
 
  while (true) {
    random_ubdate_();
 
    char c = _getch();
 
    if (c == 'q') {
      ubdate_json();
      system("cls");
      break;
    }
    else if (c == 'e') {
      system("cls");
      std::cout << "x: " << x << std::endl;
      std::cout << "y: " << y << std::endl;
      loc_shanc();
      while (_kbhit()) _getch();
      ubdate_json();          // сохраняем результат осмотра
      continue;               
    }
    else if (c == '`') console_rust();
    else if (c == 'w') x++;
    else if (c == 's') x--;
    else if (c == 'd') y++;
    else if (c == 'a') y--;
    else continue;           
 
    system("cls");
    std::cout << "x: " << x << std::endl;
    std::cout << "y: " << y << std::endl;
  }
}
 
 

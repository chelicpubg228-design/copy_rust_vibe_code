#include "../system_rust/copy_rust.h"


void kop_rast::ygaday_numbers() {
    update_random_plays();

    std::cin.exceptions(std::ios::failbit | std::ios::badbit);
    int cin_for_nums;
    std::cout << "I’ve thought of a number between 1 and 100; try to guess it—you have 10 attempts" << std::endl;
    for (int i = 0; i < 10; i++) {
         try {
        std::cin >> cin_for_nums;
  

        if (cin_for_nums == random_num_for_play) {
            std::cout << "you wins!" << std::endl;
            return;
        }
        else if (cin_for_nums < random_num_for_play) {
            std::cout << "a larger number" << std::endl;
       


        }
        else if (cin_for_nums > random_num_for_play) {
            std::cout << "a smaller number" << std::endl;
            

        }
        }
     catch(const std::ios_base::failure&) {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    continue_enter();
    i--;
    }

 }
 std::cout << "you lose!" << std::endl;
}
 
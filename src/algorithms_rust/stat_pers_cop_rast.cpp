#include "../system_rust/copy_rust.h"


void kop_rast::max_min_pers() {
    std::string for_HP_stat = std::string(hp_person / 5, '#');
    std::string for_food_stat = std::string(eda / 5, '#');
    std::string for_woter_stat = std::string(water / 5, '#');
    std::string for_energy_stat = std::string(energy / 5, '#');
    long long max_stat_pers = std::max<long long>({
        hp_person, eda, water, energy
     });
    long long min_stat_pers = std::min<long long>({
        hp_person, eda, water, energy
     });
    std::cout << "HP:     " << for_HP_stat << " " << hp_person << std::endl;
    std::cout << "food:   " << for_food_stat << " " << eda << std::endl;
    std::cout << "woter:  " << for_woter_stat << " " << water << std::endl;
    std::cout << "energy: " << for_energy_stat << " " << energy << std::endl;
    std::cout << "min:    " << min_stat_pers << std::endl;
    std::cout << "max:    " << max_stat_pers << std::endl;
    
}
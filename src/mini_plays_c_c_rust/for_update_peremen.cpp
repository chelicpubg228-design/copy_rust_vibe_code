#include "../system_rust/copy_rust.h"



void kop_rast::update_random_plays() {
    random_num_for_play = rand() % 100 + 1;
}
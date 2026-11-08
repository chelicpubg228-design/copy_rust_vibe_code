#include "../system_rust/copy_rust.h"
void kop_rast::min_res() {
    // ========== MAX ==========
    long long max_res = std::max({
        tree, stown, sera, metal, tkan, berry,
        meet, bone, kosha, nitki, skrap, voda_butka
    });

    long long max_tems = std::max({
        fakel, stown_topor, iron_topor, iron_pick,
        MVk_pick, chainsaw, MVK_topor, MVK
    });

    long long max_arm = std::max({
        leather_helmet, leather_bib,
        leather_leggings, leather_boots
    });

    // ========== MIN ==========
    long long min_res = std::min({
        tree, stown, sera, metal, tkan, berry,
        meet, bone, kosha, nitki, skrap, voda_butka
    });

    long long min_tems = std::min({
        fakel, stown_topor, iron_topor, iron_pick,
        MVk_pick, chainsaw, MVK_topor, MVK
    });

    long long min_arm = std::min({
        leather_helmet, leather_bib,
        leather_leggings, leather_boots
    });

    // ресурсы
    std::string tree_stat  = std::string(tree / 2, '#');
    std::string stown_stat = std::string(stown / 2, '#');
    std::string sera_stat  = std::string(sera / 2, '#');
    std::string metal_stat = std::string(metal / 2, '#');
    std::string tkan_stat  = std::string(tkan / 2, '#');
    std::string berry_stat = std::string(berry / 2, '#');
    std::string meet_stat  = std::string(meet / 2, '#');
    std::string bone_stat  = std::string(bone / 2, '#');
    std::string kosha_stat = std::string(kosha / 2, '#');
    std::string nitki_stat = std::string(nitki / 2, '#');
    std::string skrap_stat = std::string(skrap / 2, '#');
    std::string voda_stat  = std::string(voda_butka / 2, '#');

    // предметы
    std::string fakel_stat       = std::string(fakel / 2, '#');
    std::string stown_topor_stat = std::string(stown_topor / 2, '#');
    std::string iron_topor_stat  = std::string(iron_topor / 2, '#');
    std::string iron_pick_stat   = std::string(iron_pick / 2, '#');
    std::string MVk_pick_stat    = std::string(MVk_pick / 2, '#');
    std::string chainsaw_stat    = std::string(chainsaw / 2, '#');
    std::string MVK_topor_stat   = std::string(MVK_topor / 2, '#');
    std::string MVK_stat         = std::string(MVK / 2, '#');

    // броня
    std::string helmet_stat   = std::string(leather_helmet / 2, '#');
    std::string bib_stat      = std::string(leather_bib / 2, '#');
    std::string leggings_stat = std::string(leather_leggings / 2, '#');
    std::string boots_stat    = std::string(leather_boots / 2, '#');

    // ========== ВЫВОД ==========
    std::cout << "\n========== RESOURCES ==========\n";
    std::cout << "Tree:    " << tree_stat  << " " << tree       << '\n';
    std::cout << "Stone:   " << stown_stat << " " << stown      << '\n';
    std::cout << "Sulfur:  " << sera_stat  << " " << sera       << '\n';
    std::cout << "Iron:    " << metal_stat << " " << metal      << '\n';
    std::cout << "Fabric:  " << tkan_stat  << " " << tkan       << '\n';
    std::cout << "Berries: " << berry_stat << " " << berry      << '\n';
    std::cout << "Meat:    " << meet_stat  << " " << meet       << '\n';
    std::cout << "Bones:   " << bone_stat  << " " << bone       << '\n';
    std::cout << "Leather: " << kosha_stat << " " << kosha      << '\n';
    std::cout << "Threads: " << nitki_stat << " " << nitki      << '\n';
    std::cout << "Scrap:   " << skrap_stat << " " << skrap      << '\n';
    std::cout << "Water:   " << voda_stat  << " " << voda_butka << '\n';
    std::cout << "min: " << min_res << std::endl;
    std::cout << "max: " << max_res << std::endl;

    std::cout << "\n========== ITEMS ==========\n";
    std::cout << "Torch:          " << fakel_stat       << " " << fakel       << '\n';
    std::cout << "Stone axe:      " << stown_topor_stat << " " << stown_topor << '\n';
    std::cout << "Iron axe:       " << iron_topor_stat  << " " << iron_topor  << '\n';
    std::cout << "Iron pickaxe:   " << iron_pick_stat   << " " << iron_pick   << '\n';
    std::cout << "Stone pickaxe:  " << MVk_pick_stat    << " " << MVk_pick    << '\n';
    std::cout << "Iron chainsaw:  " << chainsaw_stat    << " " << chainsaw    << '\n';
    std::cout << "MVK axe:        " << MVK_topor_stat   << " " << MVK_topor   << '\n';
    std::cout << "MVK pickaxe:    " << MVK_stat         << " " << MVK         << '\n';
    std::cout << "min: " << min_tems << std::endl;
    std::cout << "max: " << max_tems << std::endl;

    std::cout << "\n========== ARMOR ==========\n";
    std::cout << "Leather helmet:   " << helmet_stat   << " " << leather_helmet   << '\n';
    std::cout << "Leather bib:      " << bib_stat      << " " << leather_bib      << '\n';
    std::cout << "Leather leggings: " << leggings_stat << " " << leather_leggings << '\n';
    std::cout << "Leather boots:    " << boots_stat    << " " << leather_boots    << '\n';
    std::cout << "min: " << min_arm << std::endl;
    std::cout << "max: " << max_arm << std::endl;

    std::cout << "===========================\n";
}
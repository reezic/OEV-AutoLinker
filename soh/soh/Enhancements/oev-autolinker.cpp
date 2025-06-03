#include <fstream>
#include <iostream>
#include <filesystem>
#include "soh/OTRGlobals.h"
// #include "randomizer/context.h"
#include "randomizer/entrance.h"
#include "randomizer/randomizer_entrance_tracker.h"
#include "game-interactor/GameInteractor_Hooks.h"

// Hook constants
#define CVAR_OEV_AUTOLINKER_NAME CVAR_ENHANCEMENT("OEVAutoLinker")
#define CVAR_OEV_AUTOLINKER_CONDITION true

struct LinkedEntrance {
    u16 fromIndex;
    u16 toIndex;
};

std::vector<LinkedEntrance> linkedEntrances;

int32_t fileNumber = NULL;

// Struct for Obsidian template mappings
struct OEVEntranceData {
    std::string name;
    std::string group;
    std::string color;
    std::string folder;
};

// Entrance metadata mapping for OEV
const OEVEntranceData oevEntranceData[] = {
    // clang-format off

    // Other
    { "Child Spawn",        "", "color_Spawn",       "Spawn" },
    { "Adult Spawn",        "", "color_Spawn",       "Spawn" },
    { "Minuet of Forest",   "", "color_OcarinaSong", "Ocarina Songs" },
    { "Bolero of Fire",     "", "color_OcarinaSong", "Ocarina Songs" },
    { "Serenade of Water",  "", "color_OcarinaSong", "Ocarina Songs" },
    { "Requiem of Spirit",  "", "color_OcarinaSong", "Ocarina Songs" },
    { "Nocturne of Shadow", "", "color_OcarinaSong", "Ocarina Songs" },
    { "Prelude of Light",   "", "color_OcarinaSong", "Ocarina Songs" },

    // Kokiri Forest
    { "Kokiri Forest Lower Exit",   "Kokiri Forest", "color_KokiriForest", "Kokiri Forest" },
    { "Kokiri Forest Upper Exit",   "Kokiri Forest", "color_KokiriForest", "Kokiri Forest" },
    { "KF Link's House Entry",      "Kokiri Forest", "color_KokiriForest", "Kokiri Forest" },
    { "KF Mido's House Entry",      "Kokiri Forest", "color_KokiriForest", "Kokiri Forest" },
    { "KF Saria's House Entry",     "Kokiri Forest", "color_KokiriForest", "Kokiri Forest" },
    { "KF House of Twins Entry",    "Kokiri Forest", "color_KokiriForest", "Kokiri Forest" },
    { "KF Know-It-All House Entry", "Kokiri Forest", "color_KokiriForest", "Kokiri Forest" },
    { "KF Shop Entry",              "Kokiri Forest", "color_KokiriForest", "Kokiri Forest" },
    { "KF Storms Grotto Entry",     "Kokiri Forest", "color_Grotto",       "Kokiri Forest" },
    { "KF Outside Deku Tree",       "Kokiri Forest", "color_KokiriForest", "Kokiri Forest" },
    { "Link's House",               "",              "color_KokiriForest", "Kokiri Forest" },
    { "Mido's House",               "",              "color_KokiriForest", "Kokiri Forest" },
    { "Saria's House",              "",              "color_KokiriForest", "Kokiri Forest" },
    { "House of Twins",             "",              "color_KokiriForest", "Kokiri Forest" },
    { "Know-It-All House",          "",              "color_KokiriForest", "Kokiri Forest" },
    { "Kokiri Shop",                "",              "color_KokiriForest", "Kokiri Forest" },
    { "KF Storms Grotto",           "",              "color_Grotto",       "Kokiri Forest" },
    { "Deku Tree Entrance",         "Deku Tree",     "color_DekuTree",     "Kokiri Forest" },
    { "Deku Tree Boss Door",        "Deku Tree",     "color_DekuTree",     "Kokiri Forest" },
    { "Gohma",                      "Deku Tree",     "color_DekuTree",     "Kokiri Forest" },
    { "Gohma Blue Warp",            "Deku Tree",     "color_DekuTree",     "Kokiri Forest" },
    { "Deku Tree Blue Warp",        "Kokiri Forest", "color_KokiriForest", "Kokiri Forest" },

    // Lost Woods
    { "Lost Woods Bridge East Exit",    "Lost Woods Bridge", "color_KokiriForest", "Lost Woods" },
    { "Lost Woods Bridge West Exit",    "Lost Woods Bridge", "color_KokiriForest", "Lost Woods" },
    { "Lost Woods South Exit",          "Lost Woods",        "color_KokiriForest", "Lost Woods" },
    { "Lost Woods Tunnel Shortcut",     "Lost Woods",        "color_KokiriForest", "Lost Woods" },
    { "Lost Woods Underwater Shortcut", "Lost Woods",        "color_KokiriForest", "Lost Woods" },
    { "Lost Woods North Exit",          "Lost Woods",        "color_KokiriForest", "Lost Woods" },
    { "LW Tunnel Grotto Entry",         "Lost Woods",        "color_KokiriForest", "Lost Woods" },
    { "LW North Grotto Entry",          "Lost Woods",        "color_Grotto",       "Lost Woods" },
    { "LW Meadow Grotto Entry",         "Lost Woods",        "color_Grotto",       "Lost Woods" },
    { "LW Tunnel Grotto",               "",                  "color_Grotto",       "Lost Woods" },
    { "LW Scrubs Grotto",               "",                  "color_Grotto",       "Lost Woods" },
    { "Deku Theater",                   "",                  "color_Grotto",       "Lost Woods" },

    // Sacred Forest Meadow
    { "Sacred Forest Meadow South Exit",            "Sacred Forest Meadow", "color_KokiriForest", "Sacred Forest Meadow" },
    { "SFM Wolfos Grotto Entry",                    "Sacred Forest Meadow", "color_Grotto",       "Sacred Forest Meadow" },
    { "SFM Fairy Grotto Entry",                     "Sacred Forest Meadow", "color_Grotto",       "Sacred Forest Meadow" },
    { "SFM Storms Grotto Entry",                    "Sacred Forest Meadow", "color_Grotto",       "Sacred Forest Meadow" },
    { "Sacred Forest Meadow Outside Forest Temple", "Sacred Forest Meadow", "color_KokiriForest", "Sacred Forest Meadow" },
    { "SFM Wolfos Grotto",                          "",                     "color_Grotto",       "Sacred Forest Meadow" },
    { "SFM Fairy Grotto",                           "",                     "color_Grotto",       "Sacred Forest Meadow" },
    { "SFM Deku Scrub Grotto",                      "",                     "color_KokiriForest", "Sacred Forest Meadow" },
    { "Forest Temple Entrance",                     "Forest Temple",        "color_ForestTemple", "Sacred Forest Meadow" },
    { "Forest Temple Boss Door",                    "Forest Temple",        "color_ForestTemple", "Sacred Forest Meadow" },
    { "Phantom Ganon",                              "Forest Temple",        "color_ForestTemple", "Sacred Forest Meadow" },
    { "Phantom Ganon Blue Warp",                    "Forest Temple",        "color_ForestTemple", "Sacred Forest Meadow" },
    { "Forest Temple Blue Warp",                    "Sacred Forest Meadow", "color_KokiriForest", "Sacred Forest Meadow" },
    { "SFM Warp Pad",                               "Kokiri Forest",        "color_KokiriForest", "Ocarina Songs" },

    // Kakariko Village
    { "Kakariko Front Gate",            "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Kakariko Southeast Exit",        "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Kakariko Guard Gate Exit",       "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Boss House Entry",           "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Skulltula House Entry",      "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Impa's House Front Entry",   "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Impa's House Back Entry",    "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Windmill Entry",             "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Shooting Gallery Entry",     "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Granny's Potion Shop Entry", "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Bazaar Entry",               "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Potion Shop Front Entry",    "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Potion Shop Back Entry",     "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Open Grotto Entry",          "Kakariko Village",          "color_Grotto",          "Kakariko Village" },
    { "Kak Center Grotto Entry",        "Kakariko Village",          "color_Grotto",          "Kakariko Village" },
    { "Kakariko Outside the Well",      "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Kakariko Village Owl Drop",      "Kakariko Village",          "color_KakarikoVillage", "Kakariko Village" },
    { "Carpenter Boss House",           "",                          "color_KakarikoVillage", "Kakariko Village" },
    { "House of Skulltula",             "",                          "color_KakarikoVillage", "Kakariko Village" },
    { "Impa's House Front",             "",                          "color_KakarikoVillage", "Kakariko Village" },
    { "Impa's House Back",              "",                          "color_KakarikoVillage", "Kakariko Village" },
    { "Windmill",                       "Windmill and Dampes Grave", "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Shooting Gallery",           "",                          "color_KakarikoVillage", "Kakariko Village" },
    { "Granny's Potion Shop",           "",                          "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Bazaar",                     "",                          "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Potion Shop Front",          "Kakariko Potion Shop",      "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Potion Shop Back",           "Kakariko Potion Shop",      "color_KakarikoVillage", "Kakariko Village" },
    { "Kak Open Grotto",                "",                          "color_Grotto",          "Kakariko Village" },
    { "Kak Redead Grotto",              "",                          "color_Grotto",          "Kakariko Village" },
    { "Bottom of the Well Entrance",    "",                          "color_BottomoftheWell", "Kakariko Village" },

    // The Graveyard
    { "Graveyard Entrance",           "Graveyard",                 "color_KakarikoVillage", "Graveyard" },
    { "GY Dampe's Hut Entry",         "Graveyard",                 "color_KakarikoVillage", "Graveyard" },
    { "GY Near-Hut Grave Entry",      "Graveyard",                 "color_Grotto",          "Graveyard" },
    { "GY Near-Tomb Grave Entry",     "Graveyard",                 "color_Grotto",          "Graveyard" },
    { "GY Royal Family's Tomb Entry", "Graveyard",                 "color_Grotto",          "Graveyard" },
    { "GY Near-Ledge Grave Entry",    "Graveyard",                 "color_Grotto",          "Graveyard" },
    { "Graveyard Outside Temple",     "Graveyard",                 "color_KakarikoVillage", "Graveyard" },
    { "Dampe's Hut",                  "",                          "color_KakarikoVillage", "Graveyard" },
    { "Shield Grave",                 "",                          "color_Grotto",          "Graveyard" },
    { "Heart Piece Grave",            "",                          "color_Grotto",          "Graveyard" },
    { "Royal Family's Tomb",          "",                          "color_Grotto",          "Graveyard" },
    { "Dampe's Grave",                "Windmill and Dampes Grave", "color_Grotto",          "Graveyard" },
    { "Shadow Temple Entrance",       "Shadow Temple",             "color_ShadowTemple",    "Graveyard" },
    { "Shadow Temple Boss Door",      "Shadow Temple",             "color_ShadowTemple",    "Graveyard" },
    { "Bongo-Bongo",                  "Shadow Temple",             "color_ShadowTemple",    "Graveyard" },
    { "Bongo-Bongo Blue Warp",        "Shadow Temple",             "color_ShadowTemple",    "Graveyard" },
    { "Shadow Temple Blue Warp",      "Graveyard",                 "color_KakarikoVillage", "Graveyard" },
    { "Graveyard Warp Pad",           "Graveyard",                 "color_KakarikoVillage", "Ocarina Songs" },

    // Death Mountain Trail
    { "DMT Owl Flight",                                "Death Mountain Trail", "color_DeathMountain",  "Death Mountain Trail" },
    { "Death Mountain Trail Middle Exit",              "Death Mountain Trail", "color_DeathMountain",  "Death Mountain Trail" },
    { "Death Mountain Trail Bottom Exit",              "Death Mountain Trail", "color_DeathMountain",  "Death Mountain Trail" },
    { "Death Mountain Trail Top Exit",                 "Death Mountain Trail", "color_DeathMountain",  "Death Mountain Trail" },
    { "DMT Great Fairy Entry",                         "Death Mountain Trail", "color_Grotto",         "Death Mountain Trail" },
    { "DMT Rock Circle Grotto Entry",                  "Death Mountain Trail", "color_Grotto",         "Death Mountain Trail" },
    { "DMT Boulder Grotto Entry",                      "Death Mountain Trail", "color_Grotto",         "Death Mountain Trail" },
    { "Death Mountain Trail Outside Dodongo's Cavern", "Death Mountain Trail", "color_DeathMountain",  "Death Mountain Trail" },
    { "DMT Great Fairy Fountain",                      "",                     "color_Grotto",         "Death Mountain Trail" },
    { "DMT Storms Grotto",                             "",                     "color_Grotto",         "Death Mountain Trail" },
    { "DMT Cow Grotto",                                "",                     "color_Grotto",         "Death Mountain Trail" },
    { "Dodongo's Cavern Entrance",                     "Dodongos Cavern",      "color_DodongosCavern", "Death Mountain Trail" },
    { "Dodongo's Cavern Boss Door",                    "Dodongos Cavern",      "color_DodongosCavern", "Death Mountain Trail" },
    { "King Dodongo",                                  "Dodongos Cavern",      "color_DodongosCavern", "Death Mountain Trail" },
    { "King Dodongo Blue Warp",                        "Dodongos Cavern",      "color_DodongosCavern", "Death Mountain Trail" },
    { "Dodongo's Cavern Blue Warp",                    "Death Mountain Trail", "color_DeathMountain",  "Death Mountain Trail" },

    // Death Mountain Crater
    { "Death Mountain Crater Bridge Exit",    "Death Mountain Crater", "color_DeathMountain", "Death Mountain Crater" },
    { "Death Mountain Crater Upper Exit",     "Death Mountain Crater", "color_DeathMountain", "Death Mountain Crater" },
    { "DMC Great Fairy Entry",                "Death Mountain Crater", "color_Grotto",        "Death Mountain Crater" },
    { "DMC Upper Grotto Entry",               "Death Mountain Crater", "color_Grotto",        "Death Mountain Crater" },
    { "DMC Hammer Grotto Entry",              "Death Mountain Crater", "color_Grotto",        "Death Mountain Crater" },
    { "Death Mountain Crater Outside Temple", "Death Mountain Crater", "color_DeathMountain", "Death Mountain Crater" },
    { "DMC Great Fairy Fountain",             "",                     "color_Grotto",        "Death Mountain Crater" },
    { "DMC Upper Grotto",                     "",                     "color_Grotto",        "Death Mountain Crater" },
    { "DMC Scrubs Grotto",                    "",                     "color_Grotto",        "Death Mountain Crater" },
    { "Fire Temple Entrance",                 "Fire Temple",          "color_FireTemple",    "Death Mountain Crater" },
    { "Fire Temple Boss Door",                "Fire Temple",          "color_FireTemple",    "Death Mountain Crater" },
    { "Volvagia",                             "Fire Temple",            "color_FireTemple",    "Death Mountain Crater" },
    { "Volvagia Blue Warp",                   "Fire Temple",            "color_FireTemple",    "Death Mountain Crater" },
    { "Fire Temple Blue Warp",                "Death Mountain Crater", "color_DeathMountain", "Death Mountain Crater" },
    { "DMC Warp Pad",                         "Death Mountain Crater", "color_DeathMountain", "Ocarina Songs" },

    // Goron City
    { "Goron City Upper Exit",              "Goron City", "color_GoronCity", "Goron City" },
    { "Goron City Darunia's Room Backdoor", "Goron City", "color_GoronCity", "Goron City" },
    { "Goron City Tunnel Shortcut",         "Goron City", "color_GoronCity", "Goron City" },
    { "GC Shop Entry",                      "Goron City", "color_GoronCity", "Goron City" },
    { "GC Lava Grotto Entry",               "Goron City", "color_Grotto",    "Goron City" },
    { "Goron Shop",                         "",           "color_GoronCity", "Goron City" },
    { "GC Scrubs Grotto",                   "",           "color_Grotto",    "Goron City" },

    // Zora's River
    { "Zora's River Lower Exit",          "Zoras River", "color_ZorasRiver", "Zora's River" },
    { "Zora's River Underwater Shortcut", "Zoras River", "color_ZorasRiver", "Zora's River" },
    { "Zora's River Waterfall Exit",      "Zoras River", "color_ZorasRiver", "Zora's River" },
    { "ZR Rock Circle Grotto Entry",      "Zoras River", "color_Grotto",     "Zora's River" },
    { "ZR Raised Boulder Grotto Entry",   "Zoras River", "color_Grotto",     "Zora's River" },
    { "ZR Raised Open Grotto Entry",      "Zoras River", "color_Grotto",     "Zora's River" },
    { "ZR Deku Scrub Grotto",             "",            "color_Grotto",     "Zora's River" },
    { "ZR Fairy Grotto",                  "",            "color_Grotto",     "Zora's River" },
    { "ZR Open Grotto",                   "",            "color_Grotto",     "Zora's River" },

    // Zora's Domain
    { "Zora's Domain Entrance",            "Zoras Domain",   "color_ZorasDomain", "Zora's Domain" },
    { "Zora's Domain Underwater Shortcut", "Zoras Domain",   "color_ZorasDomain", "Zora's Domain" },
    { "Zora's Domain Behind King Zora",    "Zoras Domain",   "color_ZorasDomain", "Zora's Domain" },
    { "ZD Shop Entry",                     "Zoras Domain",   "color_ZorasDomain", "Zora's Domain" },
    { "ZD Island Grotto Entry",            "Zoras Domain",   "color_Grotto",      "Zora's Domain" },
    { "Zora Shop",                         "",               "color_ZorasDomain", "Zora's Domain" },
    { "ZD Fairy Grotto",                   "",               "color_Grotto",      "Zora's Domain" },

    // Zora's Fountain
    { "Zora's Fountain Tunnel Exit",        "Zoras Fountain", "color_ZorasFountain", "Zora's Fountain" },
    { "ZF Great Fairy Entry",               "Zoras Fountain", "color_Grotto",        "Zora's Fountain" },
    { "Zora's Fountain Outside Jabu Jabu",  "Zoras Fountain", "color_ZorasFountain", "Zora's Fountain" },
    { "Zora's Fountain Outside Ice Cavern", "Zoras Fountain", "color_ZorasFountain", "Zora's Fountain" },
    { "ZF Great Fairy Fountain",            "",               "color_Grotto",        "Zora's Fountain" },
    { "Jabu Jabu's Belly Entrance",         "Jabu Jabu",      "color_JabuJabu",      "Zora's Fountain" },
    { "Jabu Jabu's Belly Boss Door",        "Jabu Jabu",      "color_JabuJabu",      "Zora's Fountain" },
    { "Barinade",                           "Jabu Jabu",      "color_JabuJabu",      "Zora's Fountain" },
    { "Barinade Blue Warp",                 "Jabu Jabu",      "color_JabuJabu",      "Zora's Fountain" },
    { "Jabu Jabu's Belly Blue Warp",        "Zoras Fountain", "color_ZorasFountain", "Zora's Fountain" },
    { "Ice Cavern Entrance",                "",               "color_IceCavern",     "Zora's Fountain" },

    // Hyrule Field
    { "Hyrule Field Wooded Exit",            "Hyrule Field", "color_HyruleField", "Hyrule Field" },
    { "Hyrule Field Drawbridge Exit",        "Hyrule Field", "color_HyruleField", "Hyrule Field" },
    { "Hyrule Field Center Exit",            "Hyrule Field", "color_HyruleField", "Hyrule Field" },
    { "Hyrule Field Stairs Exit",            "Hyrule Field", "color_HyruleField", "Hyrule Field" },
    { "Hyrule Field River Exit",             "Hyrule Field", "color_HyruleField", "Hyrule Field" },
    { "Hyrule Field Fence Exit",             "Hyrule Field", "color_HyruleField", "Hyrule Field" },
    { "Hyrule Field Rocky Path",             "Hyrule Field", "color_HyruleField", "Hyrule Field" },
    { "Hyrule Field Owl Drop",               "Hyrule Field", "color_HyruleField", "Hyrule Field" },
    { "HF Near Market Boulder Grotto Entry", "Hyrule Field", "color_Grotto",      "Hyrule Field" },
    { "HF Stone Bridge Tree Grotto Entry",   "Hyrule Field", "color_Grotto",      "Hyrule Field" },
    { "HF Northwest Tree Grotto Entry",      "Hyrule Field", "color_Grotto",      "Hyrule Field" },
    { "HF Northwest Boulder Grotto Entry",   "Hyrule Field", "color_Grotto",      "Hyrule Field" },
    { "HF West Rock Circle Grotto Entry",    "Hyrule Field", "color_Grotto",      "Hyrule Field" },
    { "HF South Open Grotto Entry",          "Hyrule Field", "color_Grotto",      "Hyrule Field" },
    { "HF Fenced Grotto Entry",              "Hyrule Field", "color_Grotto",      "Hyrule Field" },
    { "HF Southeast Boulder Grotto Entry",   "Hyrule Field", "color_Grotto",      "Hyrule Field" },
    { "HF Near Market Boulder Grotto",       "",             "color_Grotto",      "Hyrule Field" },
    { "HF Stone Bridge Tree Grotto",         "",             "color_Grotto",      "Hyrule Field" },
    { "HF Tektite Grotto",                   "",             "color_Grotto",      "Hyrule Field" },
    { "HF Fairy Grotto",                     "",             "color_Grotto",      "Hyrule Field" },
    { "HF Cow Grotto",                       "",             "color_Grotto",      "Hyrule Field" },
    { "HF Open Grotto",                      "",             "color_Grotto",      "Hyrule Field" },
    { "HF Fenced Scrub Grotto",              "",             "color_Grotto",      "Hyrule Field" },
    { "HF Southeast Grotto",                 "",             "color_Grotto",      "Hyrule Field" },

    // Lon Lon Ranch
    { "Lon Lon Ranch Entrance",  "Lon Lon Ranch", "color_LonLonRanch", "Lon Lon Ranch" },
    { "LLR Talon's House Entry", "Lon Lon Ranch", "color_LonLonRanch", "Lon Lon Ranch" },
    { "LLR Stables Entry",       "Lon Lon Ranch", "color_LonLonRanch", "Lon Lon Ranch" },
    { "LLR Tower Entry",         "Lon Lon Ranch", "color_LonLonRanch", "Lon Lon Ranch" },
    { "LLR Grotto Entry",        "Lon Lon Ranch", "color_Grotto",      "Lon Lon Ranch" },
    { "Talon's House",           "",              "color_LonLonRanch", "Lon Lon Ranch" },
    { "LLR Stables",             "",              "color_LonLonRanch", "Lon Lon Ranch" },
    { "LLR Tower",               "",              "color_LonLonRanch", "Lon Lon Ranch" },
    { "LLR Deku Scrub Grotto",   "",              "color_Grotto",      "Lon Lon Ranch" },

    // Lake Hylia
    { "LH Owl Flight",                  "Lake Hylia",   "color_LakeHylia",   "Lake Hylia" },
    { "Lake Hylia North Exit",          "Lake Hylia",   "color_LakeHylia",   "Lake Hylia" },
    { "Lake Hylia Underwater Shortcut", "Lake Hylia",   "color_LakeHylia",   "Lake Hylia" },
    { "LH Lab Entry",                   "Lake Hylia",   "color_LakeHylia",   "Lake Hylia" },
    { "LH Fishing Pond Entry",          "Lake Hylia",   "color_LakeHylia",   "Lake Hylia" },
    { "LH Grave Grotto Entry",          "Lake Hylia",   "color_Grotto",      "Lake Hylia" },
    { "Lake Hylia Outside Temple",      "Lake Hylia",   "color_LakeHylia",   "Lake Hylia" },
    { "Lake Hylia River Exit",          "Lake Hylia",   "color_LakeHylia",   "Lake Hylia" },
    { "LH Lab",                         "",             "color_LakeHylia",   "Lake Hylia" },
    { "Fishing Pond",                   "",             "color_LakeHylia",   "Lake Hylia" },
    { "LH Deku Scrub Grotto",           "",             "color_Grotto",      "Lake Hylia" },
    { "Water Temple Entrance",          "Water Temple", "color_WaterTemple", "Lake Hylia" },
    { "Water Temple Boss Door",         "Water Temple", "color_WaterTemple", "Lake Hylia" },
    { "Morpha",                         "Water Temple", "color_WaterTemple", "Lake Hylia" },
    { "Morpha Blue Warp",               "Water Temple", "color_WaterTemple", "Lake Hylia" },
    { "Water Temple Blue Warp",         "Lake Hylia",   "color_LakeHylia",   "Lake Hylia" },
    { "Lake Hylia Warp Pad",            "Lake Hylia",   "color_LakeHylia",   "Ocarina Songs" },

    // Gerudo Area
    { "Gerudo Valley East Exit",         "Gerudo Valley",   "color_GerudoValley",         "Gerudo Valley" },
    { "Gerudo Valley West Exit",         "Gerudo Valley",   "color_GerudoValley",         "Gerudo Valley" },
    { "Gerudo Valley River Exit",        "Gerudo Valley",   "color_GerudoValley",         "Gerudo Valley" },
    { "GV Carpenters' Tent Entry",       "Gerudo Valley",   "color_GerudoValley",         "Gerudo Valley" },
    { "GV Silver Rock Grotto Entry",     "Gerudo Valley",   "color_Grotto",               "Gerudo Valley" },
    { "GV Behind Tent Grotto Entry",     "Gerudo Valley",   "color_Grotto",               "Gerudo Valley" },
    { "Carpenters' Tent",                "",                "color_GerudoValley",         "Gerudo Valley" },
    { "GV Octorok Grotto",               "",                "color_Grotto",               "Gerudo Valley" },
    { "GV Deku Scrub Grotto",            "",                "color_Grotto",               "Gerudo Valley" },
    { "Gerudo Training Ground Entrance", "",                "color_GerudoTrainingGround", "Gerudo Valley" },
    { "Gerudo Fortress East Exit",       "Gerudo Fortress", "color_GerudoValley",         "Gerudo Valley" },
    { "Gerudo Fortress Gate Exit",       "Gerudo Fortress", "color_GerudoValley",         "Gerudo Valley" },
    { "GF Outside Training Ground",      "Gerudo Fortress", "color_GerudoValley",         "Gerudo Valley" },
    { "GF Storms Grotto Entry",          "Gerudo Fortress", "color_Grotto",               "Gerudo Valley" },
    { "GF Fairy Grotto",                 "",                "color_Grotto",               "Gerudo Valley" },

    // The Wasteland
    { "Haunted Wasteland East Exit", "Haunted Wasteland", "color_HauntedWasteland", "Haunted Wasteland" },
    { "Haunted Wasteland West Exit", "Haunted Wasteland", "color_HauntedWasteland", "Haunted Wasteland" },

    // Desert Colossus
    { "Desert Colossus East Exit",     "Desert Colossus", "color_DesertColossus", "Desert Colossus" },
    { "Colossus Great Fairy Entry",    "Desert Colossus", "color_Grotto",         "Desert Colossus" },
    { "Colossus Grotto Entry",         "Desert Colossus", "color_Grotto",         "Desert Colossus" },
    { "Colossus Outside Temple",       "Desert Colossus", "color_DesertColossus", "Desert Colossus" },
    { "Colossus Great Fairy Fountain", "",                "color_Grotto",         "Desert Colossus" },
    { "Colossus Deku Scrub Grotto",    "",                "color_Grotto",         "Desert Colossus" },
    { "Spirit Temple Entrance",        "Spirit Temple",   "color_SpiritTemple",   "Desert Colossus" },
    { "Spirit Temple Boss Door",       "Spirit Temple",   "color_SpiritTemple",   "Desert Colossus" },
    { "Twinrova",                      "Spirit Temple",   "color_SpiritTemple",   "Desert Colossus" },
    { "Twinrova Blue Warp",            "Spirit Temple",   "color_SpiritTemple",   "Desert Colossus" },
    { "Spirit Temple Blue Warp",       "Spirit Temple",   "color_DesertColossus", "Desert Colossus" },
    { "Desert Colossus Warp Pad",      "Desert Colossus", "color_DesertColossus", "Ocarina Songs" },

    // Market
    { "Market Entrance South Exit",       "Market Entrance",          "color_HyruleMarket", "Hyrule Market" },
    { "Market Entrance North Exit",       "Market Entrance",          "color_HyruleMarket", "Hyrule Market" },
    { "MK Entrance Guard House Entry",    "Market Entrance",          "color_HyruleMarket", "Hyrule Market" },
    { "Market South Exit",                "Hyrule Market",            "color_HyruleMarket", "Hyrule Market" },
    { "Market Castle Exit",               "Hyrule Market",            "color_HyruleMarket", "Hyrule Market" },
    { "Market Temple Exit",               "Hyrule Market",            "color_HyruleMarket", "Hyrule Market" },
    { "MK Shooting Gallery Entry",        "Hyrule Market",            "color_HyruleMarket", "Hyrule Market" },
    { "MK Bombchu Bowling Entry",         "Hyrule Market",            "color_HyruleMarket", "Hyrule Market" },
    { "MK Treasure Chest Game Entry",     "Hyrule Market",            "color_HyruleMarket", "Hyrule Market" },
    { "MK Man-in-Green House Entry",      "Hyrule Market",            "color_HyruleMarket", "Hyrule Market" },
    { "MK Mask Shop Entry",               "Hyrule Market",            "color_HyruleMarket", "Hyrule Market" },
    { "MK Bazaar Entry",                  "Hyrule Market",            "color_HyruleMarket", "Hyrule Market" },
    { "MK Potion Shop Entry",             "Hyrule Market",            "color_HyruleMarket", "Hyrule Market" },
    { "MK Bombchu Shop Entry",            "Hyrule Market",            "color_HyruleMarket", "Hyrule Market" },
    { "Guard House",                      "",                         "color_HyruleMarket", "Hyrule Market" },
    { "MK Shooting Gallery",              "",                         "color_HyruleMarket", "Hyrule Market" },
    { "Bombchu Bowling",                  "",                         "color_HyruleMarket", "Hyrule Market" },
    { "Treasure Chest Game",              "",                         "color_HyruleMarket", "Hyrule Market" },
    { "Man-in-Green's House",             "",                         "color_HyruleMarket", "Hyrule Market" },
    { "Mask Shop",                        "",                         "color_HyruleMarket", "Hyrule Market" },
    { "MK Bazaar",                        "",                         "color_HyruleMarket", "Hyrule Market" },
    { "MK Potion Shop",                   "",                         "color_HyruleMarket", "Hyrule Market" },
    { "Bombchu Shop",                     "",                         "color_HyruleMarket", "Hyrule Market" },
    { "ToT Courtyard Gossip Stones Exit", "Temple of Time Courtyard", "color_TempleofTime", "Hyrule Market" },
    { "ToT Courtyard Temple Entry",       "Temple of Time Courtyard", "color_TempleofTime", "Hyrule Market" },
    { "Temple of Time Entrance",          "Temple of Time",           "color_TempleofTime", "Hyrule Market" },
    { "Temple of Time Warp Pad",          "Temple of Time",           "color_TempleofTime", "Ocarina Songs" },

    // Hyrule Castle
    { "Castle Grounds South Exit",      "Hyrule Castle", "color_HyruleCastle",  "Hyrule Castle" },
    { "HC Boulder Crawlspace",          "Hyrule Castle", "color_Grotto",        "Hyrule Castle" },
    { "HC Storms Grotto Entry",         "Hyrule Castle", "color_Grotto",        "Hyrule Castle" },
    { "HC Great Fairy Fountain",        "",              "color_Grotto",        "Hyrule Castle" },
    { "HC Storms Grotto",               "",              "color_Grotto",        "Hyrule Castle" },
    { "OGC Behind Pillar",              "Hyrule Castle", "color_HyruleCastle",  "Hyrule Castle" },
    { "OGC Rainbow Bridge Exit",        "Hyrule Castle", "color_HyruleCastle",  "Hyrule Castle" },
    { "OGC Great Fairy Fountain",       "",              "color_Grotto",        "Hyrule Castle" },
    { "Inside Ganon's Castle Entrance", "Ganon's Castle", "color_GanonsCastle", "Hyrule Castle" }

    // clang-format on
};

// Lambda function for finding a name in oevEntranceData
auto oevEntranceSearch = [](const std::string& name) -> OEVEntranceData {
    for (const OEVEntranceData data : oevEntranceData) {
        if (data.name == name)
            return data;
    }
    return {};
};

// Create the obsidian markdown file for a given entrance
void CreateObsidianFile(const std::filesystem::path path, const OEVEntranceData& data) {
    // Create folder path if it doesn't exist
    std::filesystem::create_directories(path.parent_path());

    // Build group file path
    const std::filesystem::path groupFilePath =
        std::filesystem::path(Ship::Context::GetPathRelativeToAppDirectory("Obsidian Entrance Visualizer")) /
        ("Save " + std::to_string(fileNumber)) / data.folder / (data.group + ".md");

    // Check and create group file if it doesn't exist
    if (!std::filesystem::exists(groupFilePath)) {
        // Write to the new markdown file
        std::ofstream outFile(path);
        outFile << "Links to:\n";
        outFile << "Color: " << data.color << "\n";
        outFile.close();
    }

    // Write to the new markdown file
    std::ofstream outFile(path);
    outFile << "Links to:\n";
    outFile << "Color: " << data.color << "\n";
    outFile.close();
}

// Create the obsidian markdown file for a given entrance
void AddLinksToFile(const std::filesystem::path path, const std::string& linksFrom, const std::string& linksTo,
                    const std::string& group,
                    const std::string& folder) {
    // Find the "Links to:" line and add the link if not already present
    std::ifstream inFile(path);
    std::string fileContent;
    std::string line;
    std::string markdownLink = "[[" + linksTo + "]]";
    std::string markdownLinkGroup = "[[" + group + "]]";
    while (std::getline(inFile, line)) {
        if (line.rfind("Links to:", 0) == 0) {
            if (line.find(markdownLink) == std::string::npos) {
                line += " " + markdownLink;
            }
            if (markdownLinkGroup != "[[]]" && line.find(markdownLinkGroup) == std::string::npos) {
                line += " " + markdownLinkGroup;
            }
        }
        fileContent += line + "\n";
    }
    inFile.close();

    std::ofstream outFile(path);
    outFile << fileContent;
    outFile.close();

    
    // Find the "Links to:" line of the group's markdown and add the link if not already present
    if (group != "") {
        const std::filesystem::path groupFilePath =
            std::filesystem::path(Ship::Context::GetPathRelativeToAppDirectory("Obsidian Entrance Visualizer")) /
            ("Save " + std::to_string(fileNumber)) / folder / (group + ".md");

        AddLinksToFile(groupFilePath, group, linksFrom, "", "");
    }


}

// Link two entrances together by creating markdown files in the OEV folder
void LinkEntrances(const int32_t& fileNumber, const std::string& fromName, const std::string& toName,
                   const bool& isOneWay) {
    // Create the folder structure
    OEVEntranceData toData = oevEntranceSearch(toName);
    OEVEntranceData fromData = oevEntranceSearch(fromName);

    const std::filesystem::path toFilePath =
        std::filesystem::path(Ship::Context::GetPathRelativeToAppDirectory("Obsidian Entrance Visualizer")) /
        ("Save " + std::to_string(fileNumber)) / toData.folder / (toData.name + ".md");

    const std::filesystem::path fromFilePath =
        std::filesystem::path(Ship::Context::GetPathRelativeToAppDirectory("Obsidian Entrance Visualizer")) /
        ("Save " + std::to_string(fileNumber)) / fromData.folder / (fromData.name + ".md");

    // Check and create file if it doesn't exist
    if (!std::filesystem::exists(fromFilePath)) {
        CreateObsidianFile(fromFilePath, fromData);
    }
    if (!std::filesystem::exists(toFilePath)) {
        CreateObsidianFile(toFilePath, toData);
    }

    // Add source->destination link
    AddLinksToFile(fromFilePath, fromData.name, toData.name, fromData.group, fromData.folder);

    if (!isOneWay) {
        // Add backlink
        AddLinksToFile(toFilePath, toData.name, fromData.name, toData.group, toData.folder);
    }
}

void CheckForUnlinkedEntrances() {
    // Only run if file has been loaded and sceneNum is valid
    if (!fileNumber) {
        return;
    }

    // Get the randomizer's entrance pool
    const std::span<const EntranceData> entranceData = GetAllEntranceData();

    // Get the entrance overrides
    auto entranceOverrides = Rando::Context::GetInstance()->GetEntranceShuffler()->entranceOverrides;

    // Loop the randomizer's entire entrance pool, using overriden entrances where applicable
    // If the entrance has been dscovered but not linked, create a markdown file
    // in the save's OEV folder and add it to the linkedEntrances array
    for (const EntranceData entrance : entranceData) {

        // If not discovered, skip this loop iteration
        if (!IsEntranceDiscovered(entrance.index)) {
            continue;
        }

        u16 fromIndex = entrance.reverseIndex;
        std::string fromName = entrance.source;

        u16 toIndex = entrance.index;
        std::string toName = entrance.destination;
        EntranceOverride entranceOverride{ -1 };

        // Use the entrance coverride if it exists
        for (EntranceOverride override : entranceOverrides) {
            if (entrance.index == override.index) {
                toIndex = override.override;
                toName = GetEntranceData(toIndex)->destination;
                entranceOverride = override;
                break;
            }
        }

        // Check if one or the other side of the entrance has already been linked
        bool alreadyLinked = false;
        for (LinkedEntrance linkedEntrance : linkedEntrances) {
            if ((fromIndex == linkedEntrance.toIndex && toIndex == linkedEntrance.fromIndex) ||
                (fromIndex == linkedEntrance.fromIndex && toIndex == linkedEntrance.toIndex)) {
                alreadyLinked = true;
                break;
            }
        }

        // If entrance has already been linked, skip this loop iteration
        if (alreadyLinked) {
            continue;
        }

        // Get the source and destination names from the randomizer's entrance data
        
        bool isOneWay = false;
        
        if (GetEntranceData(toIndex)->reverseIndex == -1 || entranceOverride.destination == -1) {
            isOneWay = true;
        };

        // Special case
        if (toName == "Temple of Time") {
            toName = "Temple of Time Warp Pad";
        }

        // Find the matching markdown metadata mapping in oevEntranceData
        OEVEntranceData fromOEVData = oevEntranceSearch(fromName);
        OEVEntranceData toOEVData = oevEntranceSearch(toName);

        // Link the entrance
        LinkEntrances(fileNumber, fromName, toName, isOneWay);

        // Add the entrance to linkedEntrances
        LinkedEntrance linkedEntrance = { fromIndex, toIndex };
        linkedEntrances.push_back(linkedEntrance);

        std::cout << "[OEV AutoLinker] Linked new entrance: " << fromName << " - " << toName << std::endl;
    }
}

// On scene initialization, check if any entrances that
// have not already been linked have been discovered and if so, link them
void OnSceneInit(u16 sceneNum) {
    std::cout << std::endl << "[OEV AutoLinker] Scene " << sceneNum << " initialized." << std::endl;

    // Check for discovered entrances that are not linked, and link them
    CheckForUnlinkedEntrances();

    std::cout << "" << std::endl;
    return;
}

// Initialize entrances and
void InitializeEntranceData(int32_t fileNum) {

    // Set file number to use
    fileNumber = fileNum + 1;

    // Check for discovered entrances that are not linked, and link them
    CheckForUnlinkedEntrances();

    std::cout << std::endl << "[OEV AutoLinker] Game " << fileNum << " loaded" << std::endl;
    std::cout << std::endl;
}

// On save unload
void ClearEntranceData(int32_t fileNum) {
    // Clear linkedEntrances
    linkedEntrances.clear();

    // Clear file number
    fileNumber = NULL;
}

// Register hooks
void RegisterOEVAutoLinker() {
    // Check if the OEV directory exists and create it if not
    if (!std::filesystem::exists(Ship::Context::GetPathRelativeToAppDirectory("Obsidian Entrance Visualizer"))) {
        std::filesystem::create_directory(Ship::Context::GetPathRelativeToAppDirectory("Obsidian Entrance Visualizer"));
    }

    // Hooks
    COND_HOOK(OnSceneInit, CVAR_OEV_AUTOLINKER_CONDITION, OnSceneInit);
    COND_HOOK(OnLoadGame, CVAR_OEV_AUTOLINKER_CONDITION, InitializeEntranceData);
    COND_HOOK(OnExitGame, CVAR_OEV_AUTOLINKER_CONDITION, ClearEntranceData);
}

static RegisterShipInitFunc initFunc(RegisterOEVAutoLinker, { CVAR_OEV_AUTOLINKER_NAME });
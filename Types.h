#pragma once
#include <string>
#include <vector>

// ===== 屬性系統 =====
enum class Element {
    NONE,
    FIRE,
    WATER,
    WIND,
    THUNDER
};

// ===== 技能類型 =====
enum class SkillType {
    ATTACK,
    HEAL,
    BUFF
};

// ===== 道具類型 =====
enum class ItemType {
    HEAL_HP,
    RECOVER_MP,
    BUFF
};

// ===== 怪物類型 =====
enum class MonsterType {
    NORMAL,
    ELITE,
    BOSS
};

// ===== 職業類型 =====
enum class JobClass {
    WARRIOR,
    MAGE,
    PRIEST,
    RANGER
};

// ===== 工具函數 =====
inline std::string elementToString(Element e) {
    switch (e) {
        case Element::FIRE:    return "火";
        case Element::WATER:   return "水";
        case Element::WIND:    return "風";
        case Element::THUNDER: return "雷";
        default:               return "無";
    }
}

inline std::string jobToString(JobClass j) {
    switch (j) {
        case JobClass::WARRIOR: return "戰士";
        case JobClass::MAGE:    return "法師";
        case JobClass::PRIEST:  return "牧師";
        case JobClass::RANGER:  return "遊俠";
        default:                return "未知";
    }
}

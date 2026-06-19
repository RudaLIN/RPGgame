#pragma once
#include <string>
#include <iostream>
#include "Types.h"

// ===== Skill 技能類別 =====
class Skill {
private:
    std::string name;
    int mpCost;
    int power;
    Element element;
    SkillType skillType;

public:
    Skill(const std::string& name, int mpCost, int power, Element element, SkillType skillType)
        : name(name), mpCost(mpCost), power(power), element(element), skillType(skillType) {}

    // Getter
    const std::string& getName() const { return name; }
    int getMpCost() const { return mpCost; }
    int getPower() const { return power; }
    Element getElement() const { return element; }
    SkillType getSkillType() const { return skillType; }

    // 取得技能傷害（需傳入使用者的魔攻）
    int getDamage(int userMagicAtk) const {
        if (skillType == SkillType::ATTACK)
            return userMagicAtk + power;
        return power; // HEAL / BUFF 回傳 power 作為治癒量
    }

    // 使用技能（顯示資訊，實際效果由 Game/Battle 處理）
    void use() const {
        std::cout << "  使用技能：【" << name << "】";
        std::cout << "（" << elementToString(element) << "屬性";
        if (skillType == SkillType::ATTACK) std::cout << " / 攻擊）";
        else if (skillType == SkillType::HEAL) std::cout << " / 治療）";
        else std::cout << " / 強化）";
        std::cout << std::endl;
    }

    // 顯示技能資訊
    void showInfo() const {
        std::cout << "【" << name << "】 屬性:" << elementToString(element)
                  << " MP消耗:" << mpCost << " 威力:" << power;
        if (skillType == SkillType::ATTACK) std::cout << " [攻擊]";
        else if (skillType == SkillType::HEAL) std::cout << " [治療]";
        else std::cout << " [強化]";
        std::cout << std::endl;
    }
};

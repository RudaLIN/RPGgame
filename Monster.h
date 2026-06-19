#pragma once
#include <string>
#include <iostream>
#include "Types.h"

// ===== Monster 怪物類別 =====
class Monster {
private:
    std::string name;
    MonsterType monsterType;
    int hp;
    int maxHp;
    int atk;
    Element weakElement;
    int expReward;
    int goldReward;

public:
    Monster(const std::string& name, MonsterType type,
            int maxHp, int atk,
            Element weakElement, int expReward, int goldReward)
        : name(name), monsterType(type),
          hp(maxHp), maxHp(maxHp), atk(atk),
          weakElement(weakElement), expReward(expReward), goldReward(goldReward) {}

    // ===== Getter =====
    const std::string& getName() const { return name; }
    MonsterType getMonsterType() const { return monsterType; }
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    int getAtk() const { return atk; }
    Element getWeakElement() const { return weakElement; }
    int getExpReward() const { return expReward; }
    int getGoldReward() const { return goldReward; }

    // ===== 戰鬥功能 =====
    // 普通攻擊
    int attack() const { return atk; }

    // 受到傷害，考慮弱點，回傳實際傷害值
    int takeDamage(int dmg, Element attackElement) {
        int finalDmg = dmg;
        if (attackElement != Element::NONE && attackElement == weakElement) {
            finalDmg = static_cast<int>(dmg * 1.5);
            std::cout << "  ★ 弱點攻擊！傷害 x1.5！" << std::endl;
        }
        hp -= finalDmg;
        if (hp < 0) hp = 0;
        return finalDmg;
    }

    // 受到無屬性傷害（普攻用）
    void takeDamage(int dmg) {
        hp -= dmg;
        if (hp < 0) hp = 0;
    }

    // 是否存活
    bool isAlive() const { return hp > 0; }

    // 顯示怪物資訊
    void showInfo() const {
        std::string typeStr;
        switch (monsterType) {
            case MonsterType::NORMAL: typeStr = "普通"; break;
            case MonsterType::ELITE:  typeStr = "菁英"; break;
            case MonsterType::BOSS:   typeStr = "魔王"; break;
        }
        std::cout << "【" << name << "】[" << typeStr << "]"
                  << "  HP:" << hp << "/" << maxHp
                  << "  ATK:" << atk
                  << "  弱點:" << elementToString(weakElement) << std::endl;
    }
};

// ===== Monster Factory 怪物工廠 =====
class MonsterFactory {
public:
    static Monster createNormal(int round) {
        static const std::vector<std::pair<std::string, Element>> pool = {
            {"哥布林",   Element::FIRE},
            {"骷髏兵",   Element::WIND},
            {"森林狼",   Element::THUNDER},
            {"泥土巨人", Element::WATER},
            {"蝙蝠群",   Element::FIRE},
            {"毒蜘蛛",   Element::WIND},
        };
        auto [mname, weak] = pool[rand() % pool.size()];
        return Monster(mname, MonsterType::NORMAL,
                       40 + round * 8,
                       8  + round * 2,
                       weak,
                       15 + round * 5,
                       10 + round * 3);
    }

    static Monster createElite(int round) {
        static const std::vector<std::pair<std::string, Element>> pool = {
            {"黑騎士",   Element::THUNDER},
            {"闇影刺客", Element::FIRE},
            {"石頭魔像", Element::WATER},
            {"風暴鷹",   Element::WIND},
            {"海蛇",     Element::FIRE},
        };
        auto [mname, weak] = pool[rand() % pool.size()];
        return Monster(mname, MonsterType::ELITE,
                       70 + round * 12,
                       14 + round * 3,
                       weak,
                       30 + round * 8,
                       20 + round * 5);
    }

    static Monster createBoss() {
        return Monster("暗黑魔王 Zarvok", MonsterType::BOSS,
                       500,
                       35,
                       Element::WIND,
                       300, 200);
    }
};

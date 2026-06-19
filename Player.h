#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "Types.h"
#include "Skill.h"
#include "Item.h"

// ===== Player 角色類別 =====
class Player {
private:
    // ── 個人屬性 ──
    std::string name;
    JobClass jobClass;
    int level;
    int exp;
    int expToNext;
    int hp;
    int maxHp;
    int mp;
    int maxMp;
    int atk;
    int magicAtk;
    int buffAtk;           // 臨時強化（道具/技能）
    std::vector<Skill> skills; // 最多 4 個技能

    // ── 全隊共用（static）──
    static std::vector<Item> inventory; // 共用背包
    static int gold;                    // 共用金幣

    int calcExpToNext(int lv) const { return 30 + lv * 20; }

public:
    Player(const std::string& name, JobClass job,
           int maxHp, int maxMp, int atk, int magicAtk)
        : name(name), jobClass(job), level(1), exp(0),
          hp(maxHp), maxHp(maxHp), mp(maxMp), maxMp(maxMp),
          atk(atk), magicAtk(magicAtk), buffAtk(0)
    {
        expToNext = calcExpToNext(1);
    }

    // ===== 個人 Getter =====
    const std::string& getName() const { return name; }
    JobClass getJobClass() const { return jobClass; }
    int getLevel() const { return level; }
    int getExp() const { return exp; }
    int getHp() const { return hp; }
    int getMaxHp() const { return maxHp; }
    int getMp() const { return mp; }
    int getMaxMp() const { return maxMp; }
    int getAtk() const { return atk + buffAtk; }
    int getMagicAtk() const { return magicAtk; }
    const std::vector<Skill>& getSkills() const { return skills; }
    bool hasBuff() const { return buffAtk > 0; }

    // ===== 個人 Setter =====
    void setHp(int v) { hp = std::min(v, maxHp); }
    void setMp(int v) { mp = std::min(v, maxMp); }
    void clearBuff() { buffAtk = 0; }

    // ===== 共用背包 / 金幣 static 介面 =====
    static std::vector<Item>& getInventory() { return inventory; }
    static int getGold() { return gold; }

    static void addGold(int amount) { gold += amount; }
    static bool spendGold(int amount) {
        if (gold < amount) return false;
        gold -= amount;
        return true;
    }

    // 加入道具，同名疊加數量
    static void addItem(const Item& item) {
        for (auto& it : inventory) {
            if (it.getName() == item.getName()) {
                it.setQuantity(it.getQuantity() + item.getQuantity());
                std::cout << "  獲得 " << item.getName()
                          << " x" << item.getQuantity() << std::endl;
                return;
            }
        }
        inventory.push_back(item);
        std::cout << "  獲得 " << item.getName()
                  << " x" << item.getQuantity() << std::endl;
    }

    static void showInventory() {
        if (inventory.empty()) {
            std::cout << "  （背包空空如也）" << std::endl;
            return;
        }
        for (int i = 0; i < (int)inventory.size(); i++) {
            std::cout << "  " << (i+1) << ". ";
            inventory[i].showInfo();
        }
    }

    // 重置共用資料（開新遊戲用）
    static void resetShared() {
        inventory.clear();
        gold = 50;
    }

    // ===== 戰鬥功能 =====
    // 普通攻擊，不消耗 MP，傷害固定使用 atk
    int attack() const { return atk + buffAtk; }

    // 使用技能（消耗 MP），回傳傷害/治癒量，MP不足回傳 -1
    int useSkill(int skillIndex) {
        if (skillIndex < 0 || skillIndex >= (int)skills.size()) return -1;
        Skill& sk = skills[skillIndex];
        if (mp < sk.getMpCost()) {
            std::cout << "  MP 不足，無法使用【" << sk.getName() << "】！" << std::endl;
            return -1;
        }
        mp -= sk.getMpCost();
        sk.use();
        return sk.getDamage(magicAtk);
    }

    // 使用道具（作用於 this），回傳效果值；-1 表示道具不可用
    int useItem(int itemIndex) {
        if (itemIndex < 0 || itemIndex >= (int)inventory.size()) return -1;
        if (!inventory[itemIndex].isAvailable()) return -1;
        int val = inventory[itemIndex].use();
        ItemType t = inventory[itemIndex].getItemType();
        if (t == ItemType::HEAL_HP)    heal(val);
        else if (t == ItemType::RECOVER_MP) recoverMp(val);
        else                           applyBuff(val);
        // 數量歸零則移除
        if (!inventory[itemIndex].isAvailable())
            inventory.erase(inventory.begin() + itemIndex);
        return val;
    }

    // 技能類型 / 屬性查詢
    SkillType getSkillType(int i) const {
        if (i < 0 || i >= (int)skills.size()) return SkillType::ATTACK;
        return skills[i].getSkillType();
    }
    Element getSkillElement(int i) const {
        if (i < 0 || i >= (int)skills.size()) return Element::NONE;
        return skills[i].getElement();
    }

    // 受到傷害
    void takeDamage(int dmg) {
        hp -= dmg;
        if (hp < 0) hp = 0;
    }

    // 治癒 HP
    void heal(int amount) { hp = std::min(hp + amount, maxHp); }

    // 回復 MP
    void recoverMp(int amount) { mp = std::min(mp + amount, maxMp); }

    // 暫時強化攻擊力
    void applyBuff(int amount) { buffAtk += amount; }

    // 判斷是否存活
    bool isAlive() const { return hp > 0; }

    // ===== 成長系統 =====
    // 獲得經驗值，回傳是否升級
    bool gainExp(int amount) {
        exp += amount;
        bool leveled = false;
        while (exp >= expToNext) {
            exp -= expToNext;
            levelUp();
            leveled = true;
        }
        return leveled;
    }

    // 升級：依職業提升屬性，並全額回復
    void levelUp() {
        level++;
        expToNext = calcExpToNext(level);
        switch (jobClass) {
            case JobClass::WARRIOR: maxHp+=20; atk+=4; magicAtk+=1; maxMp+=3;  break;
            case JobClass::MAGE:    maxHp+=8;  atk+=1; magicAtk+=6; maxMp+=12; break;
            case JobClass::PRIEST:  maxHp+=12; atk+=2; magicAtk+=4; maxMp+=10; break;
            case JobClass::RANGER:  maxHp+=14; atk+=3; magicAtk+=3; maxMp+=6;  break;
        }
        hp = maxHp;
        mp = maxMp;
        std::cout << "  ★ " << name << " 升到 Lv." << level << "！各項能力提升！" << std::endl;
    }

    // ===== 技能管理 =====
    // 學習技能（欄位未滿才成功，回傳 false = 需先 forgetSkill）
    bool learnSkill(const Skill& sk) {
        if ((int)skills.size() < 4) {
            skills.push_back(sk);
            std::cout << "  " << name << " 學會了【" << sk.getName() << "】！" << std::endl;
            return true;
        }
        return false;
    }

    // 遺忘技能
    bool forgetSkill(int index) {
        if (index < 0 || index >= (int)skills.size()) return false;
        std::cout << "  " << name << " 遺忘了【" << skills[index].getName() << "】。" << std::endl;
        skills.erase(skills.begin() + index);
        return true;
    }

    // ===== 顯示 =====
    void showStatus() const {
        std::cout << "  [" << jobToString(jobClass) << "] " << name
                  << "  Lv." << level
                  << "  HP:" << hp << "/" << maxHp
                  << "  MP:" << mp << "/" << maxMp
                  << "  ATK:" << atk << "  MATK:" << magicAtk;
        if (buffAtk > 0) std::cout << " (+強化" << buffAtk << ")";
        std::cout << std::endl;
    }

    void showSkills() const {
        if (skills.empty()) { std::cout << "  （無技能）" << std::endl; return; }
        for (int i = 0; i < (int)skills.size(); i++) {
            std::cout << "  " << (i+1) << ". ";
            skills[i].showInfo();
        }
    }
};

// ===== static 成員定義（inline，避免需要獨立 .cpp）=====
inline std::vector<Item> Player::inventory;
inline int Player::gold = 50;

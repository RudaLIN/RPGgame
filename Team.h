#pragma once
#include <vector>
#include <memory>
#include <iostream>
#include "Player.h"
#include "Item.h"

// ===== Team 隊伍類別 =====
// 管理最多 4 名角色與共用背包
class Team {
private:
    std::vector<Player> members;
    std::vector<Item> inventory; // 共用背包
    int gold;

public:
    Team() : gold(50) {}

    // ===== 金幣 =====
    int getGold() const { return gold; }
    void addGold(int amount) { gold += amount; }
    bool spendGold(int amount) {
        if (gold < amount) return false;
        gold -= amount;
        return true;
    }

    // ===== 成員管理 =====
    int size() const { return (int)members.size(); }
    bool isFull() const { return members.size() >= 4; }
    Player& getMember(int i) { return members[i]; }
    const Player& getMember(int i) const { return members[i]; }
    std::vector<Player>& getMembers() { return members; }

    bool addMember(const Player& p) {
        if (isFull()) return false;
        members.push_back(p);
        std::cout << "  " << p.getName() << " 加入了隊伍！" << std::endl;
        return true;
    }

    void removeMember(int index) {
        if (index < 0 || index >= (int)members.size()) return;
        std::cout << "  " << members[index].getName() << " 離開了隊伍。" << std::endl;
        members.erase(members.begin() + index);
    }

    // 是否全滅
    bool isWiped() const {
        for (const auto& p : members)
            if (p.isAlive()) return false;
        return true;
    }

    // 獲得存活成員數量
    int aliveCount() const {
        int cnt = 0;
        for (const auto& p : members)
            if (p.isAlive()) cnt++;
        return cnt;
    }

    // ===== 背包管理 =====
    std::vector<Item>& getInventory() { return inventory; }

    void addItem(const Item& item) {
        // 同名道具疊加
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

    void showInventory() const {
        if (inventory.empty()) {
            std::cout << "  （背包空空如也）" << std::endl;
            return;
        }
        for (int i = 0; i < (int)inventory.size(); i++) {
            std::cout << "  " << (i+1) << ". ";
            inventory[i].showInfo();
        }
    }

    // ===== 顯示 =====
    void showStatus() const {
        std::cout << "═══ 隊伍狀態 ═══  金幣：" << gold << "G" << std::endl;
        if (members.empty()) {
            std::cout << "  （隊伍空無一人）" << std::endl;
            return;
        }
        for (const auto& p : members) {
            if (!p.isAlive()) {
                std::cout << "  [倒下] " << p.getName() << std::endl;
            } else {
                p.showStatus();
            }
        }
    }

    // 戰鬥後恢復部分 HP/MP
    void restoreAfterBattle(int hpPercent, int mpPercent) {
        for (auto& p : members) {
            if (p.isAlive()) {
                p.heal(p.getMaxHp() * hpPercent / 100);
                p.recoverMp(p.getMaxMp() * mpPercent / 100);
                p.clearBuff();
            }
        }
    }
};

#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <limits>
#include "Team.h"
#include "Monster.h"

// ===== Battle 戰鬥系統 =====
class Battle {
private:
    Team& team;
    Monster& monster;

    void clearInput() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    int getInt(int lo, int hi) {
        int v;
        while (true) {
            if (std::cin >> v && v >= lo && v <= hi) { clearInput(); return v; }
            clearInput();
            std::cout << "  請輸入 " << lo << "~" << hi << " 的數字：";
        }
    }

    // 選擇存活隊員，回傳 index
    int chooseLivingMember(const std::string& purpose) {
        std::vector<int> alive;
        for (int i = 0; i < team.size(); i++)
            if (team.getMember(i).isAlive()) alive.push_back(i);
        if ((int)alive.size() == 1) return alive[0];

        std::cout << "  選擇" << purpose << "的角色：" << std::endl;
        for (int idx : alive)
            std::cout << "    " << (idx+1) << ". " << team.getMember(idx).getName()
                      << "  HP:" << team.getMember(idx).getHp()
                      << "/" << team.getMember(idx).getMaxHp() << std::endl;
        std::cout << "  > ";
        int chosen = getInt(1, team.size());
        while (!team.getMember(chosen-1).isAlive()) {
            std::cout << "  該角色已倒下，請重新選擇：";
            chosen = getInt(1, team.size());
        }
        return chosen - 1;
    }

    void showBattleStatus() {
        std::cout << std::endl;
        std::cout << "─────────────────────────────" << std::endl;
        monster.showInfo();
        std::cout << "─────────────────────────────" << std::endl;
        for (int i = 0; i < team.size(); i++) {
            if (team.getMember(i).isAlive())
                team.getMember(i).showStatus();
            else
                std::cout << "  ✝ " << team.getMember(i).getName() << "（倒下）" << std::endl;
        }
        std::cout << "─────────────────────────────" << std::endl;
    }

    // 玩家回合：每位存活成員依序行動
    void playerTurn() {
        for (int i = 0; i < team.size(); i++) {
            Player& p = team.getMember(i);
            if (!p.isAlive() || !monster.isAlive()) continue;

            std::cout << std::endl;
            std::cout << ">>> " << p.getName() << " 的回合 <<<" << std::endl;
            std::cout << "  1. 普通攻擊  2. 使用技能  3. 使用道具" << std::endl;
            std::cout << "  > ";
            int choice = getInt(1, 3);

            if (choice == 1) {
                // 普通攻擊
                int dmg = p.attack();
                monster.takeDamage(dmg);
                std::cout << "  " << p.getName() << " 攻擊了 "
                          << monster.getName() << "，造成 " << dmg << " 傷害！" << std::endl;

            } else if (choice == 2) {
                // 技能
                if (p.getSkills().empty()) {
                    std::cout << "  沒有技能！改為普通攻擊。" << std::endl;
                    int dmg = p.attack();
                    monster.takeDamage(dmg);
                    std::cout << "  造成 " << dmg << " 傷害！" << std::endl;
                } else {
                    std::cout << "  選擇技能：" << std::endl;
                    p.showSkills();
                    std::cout << "  0. 取消  > ";
                    int si = getInt(0, (int)p.getSkills().size());
                    if (si == 0) {
                        int dmg = p.attack();
                        monster.takeDamage(dmg);
                        std::cout << "  改為普通攻擊，造成 " << dmg << " 傷害！" << std::endl;
                    } else {
                        si--;
                        SkillType st = p.getSkillType(si);
                        Element   el = p.getSkillElement(si);
                        int val = p.useSkill(si);
                        if (val < 0) {
                            int dmg = p.attack();
                            monster.takeDamage(dmg);
                            std::cout << "  改為普通攻擊，造成 " << dmg << " 傷害！" << std::endl;
                        } else if (st == SkillType::HEAL) {
                            int tidx = chooseLivingMember("治療");
                            team.getMember(tidx).heal(val);
                            std::cout << "  為 " << team.getMember(tidx).getName()
                                      << " 回復了 " << val << " HP！" << std::endl;
                        } else if (st == SkillType::BUFF) {
                            p.applyBuff(val);
                            std::cout << "  " << p.getName()
                                      << " 攻擊力暫時提升 " << val << "！" << std::endl;
                        } else {
                            int finalDmg = monster.takeDamage(val, el);
                            std::cout << "  對 " << monster.getName()
                                      << " 造成 " << finalDmg << " 魔法傷害！" << std::endl;
                        }
                    }
                }

            } else {
                // 使用道具（透過 Player::getInventory() 取得共用背包）
                auto& inv = Player::getInventory();
                if (inv.empty()) {
                    std::cout << "  背包是空的！改為普通攻擊。" << std::endl;
                    int dmg = p.attack();
                    monster.takeDamage(dmg);
                    std::cout << "  造成 " << dmg << " 傷害！" << std::endl;
                } else {
                    std::cout << "  選擇道具：" << std::endl;
                    Player::showInventory();
                    std::cout << "  0. 取消  > ";
                    int ii = getInt(0, (int)inv.size());
                    if (ii == 0) {
                        int dmg = p.attack();
                        monster.takeDamage(dmg);
                        std::cout << "  改為普通攻擊，造成 " << dmg << " 傷害！" << std::endl;
                    } else {
                        ii--;
                        if (!inv[ii].isAvailable()) {
                            std::cout << "  道具已用完！" << std::endl;
                        } else {
                            // 選擇使用對象
                            int tidx = chooseLivingMember(
                                inv[ii].getItemType() == ItemType::BUFF ? "強化" : "使用道具");
                            // 透過 Player::useItem 使用（自動套用效果並移除空道具）
                            // 這裡手動操作以便套用到指定角色
                            int val = inv[ii].use();
                            Player& target = team.getMember(tidx);
                            if (inv[ii].getItemType() == ItemType::HEAL_HP) {
                                target.heal(val);
                                std::cout << "  " << target.getName()
                                          << " 回復了 " << val << " HP！" << std::endl;
                            } else if (inv[ii].getItemType() == ItemType::RECOVER_MP) {
                                target.recoverMp(val);
                                std::cout << "  " << target.getName()
                                          << " 回復了 " << val << " MP！" << std::endl;
                            } else {
                                target.applyBuff(val);
                                std::cout << "  " << target.getName()
                                          << " 攻擊力提升 " << val << "！" << std::endl;
                            }
                            if (!inv[ii].isAvailable())
                                inv.erase(inv.begin() + ii);
                        }
                    }
                }
            }
        }
    }

    // 怪物回合（純普攻，隨機打一名存活成員）
    void monsterTurn() {
        if (!monster.isAlive()) return;
        std::cout << std::endl;
        std::cout << ">>> " << monster.getName() << " 的回合 <<<" << std::endl;

        std::vector<int> alive;
        for (int i = 0; i < team.size(); i++)
            if (team.getMember(i).isAlive()) alive.push_back(i);
        if (!alive.empty()) {
            int tidx = alive[rand() % alive.size()];
            int dmg = monster.attack();
            team.getMember(tidx).takeDamage(dmg);
            std::cout << "  " << monster.getName() << " 攻擊了 "
                      << team.getMember(tidx).getName()
                      << "，造成 " << dmg << " 傷害！" << std::endl;
        }
    }

public:
    Battle(Team& team, Monster& monster) : team(team), monster(monster) {}

    // 執行戰鬥，回傳 true = 玩家勝
    bool run() {
        std::cout << std::endl;
        std::cout << "╔══════════════════════════╗" << std::endl;
        std::cout << "║  ⚔  戰鬥開始！           ║" << std::endl;
        std::cout << "╚══════════════════════════╝" << std::endl;
        monster.showInfo();

        int turn = 1;
        while (monster.isAlive() && !team.isWiped()) {
            std::cout << std::endl << "【第 " << turn++ << " 戰鬥回合】" << std::endl;
            showBattleStatus();
            playerTurn();
            if (!monster.isAlive()) break;
            monsterTurn();
        }

        std::cout << std::endl;
        if (!monster.isAlive()) {
            std::cout << "╔══════════════════════════╗" << std::endl;
            std::cout << "║  ★  擊敗了 " << monster.getName() << "！  ║" << std::endl;
            std::cout << "╚══════════════════════════╝" << std::endl;
            team.clearBuffs();
            return true;
        } else {
            std::cout << "╔══════════════════════════╗" << std::endl;
            std::cout << "║  ✝  隊伍全滅...          ║" << std::endl;
            std::cout << "╚══════════════════════════╝" << std::endl;
            return false;
        }
    }
};

#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <limits>
#include "Team.h"
#include "Monster.h"
#include "Battle.h"
#include "Skill.h"

enum class EventType {
    NORMAL_BATTLE,
    ELITE_BATTLE,
    MERCHANT,
    SAGE,
    RECRUIT,
    REST
};

class Event {
private:
    EventType type;
    std::string description;

    void clearInput() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    int getInt(int lo, int hi) {
        int v;
        while (true) {
            if (std::cin >> v && v >= lo && v <= hi) { clearInput(); return v; }
            clearInput();
            std::cout << "  請輸入 " << lo << "~" << hi << "：";
        }
    }

    // ===== 戰鬥事件 =====
    bool doBattle(Team& team, int round, bool elite) {
        Monster m = elite ? MonsterFactory::createElite(round)
                          : MonsterFactory::createNormal(round);
        Battle battle(team, m);
        bool won = battle.run();
        if (won) {
            int exp  = m.getExpReward();
            int gold = m.getGoldReward();
            std::cout << std::endl;
            std::cout << "  獲得 " << gold << " 金幣！" << std::endl;
            Player::addGold(gold);

            std::cout << "  每名存活成員獲得 " << exp << " 經驗值！" << std::endl;
            for (int i = 0; i < team.size(); i++)
                if (team.getMember(i).isAlive())
                    team.getMember(i).gainExp(exp);

            // 隨機掉落道具
            int dropThreshold = elite ? 60 : 40;
            if (rand() % 100 < dropThreshold) {
                int r = rand() % 3;
                if (r == 0) Player::addItem(Item("小型回復藥", ItemType::HEAL_HP,    50, 30));
                else if (r == 1) Player::addItem(Item("魔力水",   ItemType::RECOVER_MP, 30, 25));
                else             Player::addItem(Item("力量藥水", ItemType::BUFF,       8,  40));
            }

            // 菁英有機率掉技能書
            if (elite && rand() % 100 < 30) {
                std::cout << std::endl << "  !! 獲得了技能書！!!" << std::endl;
                static const std::vector<Skill> skillPool = {
                    Skill("烈焰衝擊", 10, 20, Element::FIRE,    SkillType::ATTACK),
                    Skill("冰水洪流", 10, 20, Element::WATER,   SkillType::ATTACK),
                    Skill("風刃斬",   8,  16, Element::WIND,    SkillType::ATTACK),
                    Skill("雷霆一擊", 12, 25, Element::THUNDER, SkillType::ATTACK),
                    Skill("治癒之光", 8,  40, Element::NONE,    SkillType::HEAL),
                    Skill("力量祝福", 6,  10, Element::NONE,    SkillType::BUFF),
                };
                const Skill& newSkill = skillPool[rand() % skillPool.size()];
                std::cout << "  技能："; newSkill.showInfo();
                std::cout << "  選擇哪位成員學習技能：" << std::endl;
                for (int i = 0; i < team.size(); i++)
                    if (team.getMember(i).isAlive())
                        std::cout << "    " << (i+1) << ". "
                                  << team.getMember(i).getName() << std::endl;
                std::cout << "  0. 放棄  > ";
                int idx = getInt(0, team.size());
                if (idx > 0) {
                    idx--;
                    Player& learner = team.getMember(idx);
                    if (!learner.learnSkill(newSkill)) {
                        std::cout << "  技能欄已滿！選擇遺忘哪個技能：" << std::endl;
                        learner.showSkills();
                        std::cout << "  > ";
                        int fi = getInt(1, (int)learner.getSkills().size());
                        learner.forgetSkill(fi - 1);
                        learner.learnSkill(newSkill);
                    }
                }
            }
        }
        return won;
    }

    // ===== 商人事件 =====
    void doMerchant(Team&) {
        std::cout << std::endl;
        std::cout << "╔═══════════════════════╗" << std::endl;
        std::cout << "║  🏪 旅行商人          ║" << std::endl;
        std::cout << "╚═══════════════════════╝" << std::endl;
        std::cout << "  「歡迎光臨！請選購您需要的物品。」" << std::endl;

        struct ShopItem { std::string name; ItemType type; int value; int price; };
        std::vector<ShopItem> shop = {
            {"中型回復藥", ItemType::HEAL_HP,    80,  50},
            {"大型回復藥", ItemType::HEAL_HP,    150, 90},
            {"魔力水",     ItemType::RECOVER_MP, 40,  40},
            {"大魔力水",   ItemType::RECOVER_MP, 80,  70},
            {"力量藥水",   ItemType::BUFF,       10,  55},
        };

        while (true) {
            std::cout << std::endl;
            std::cout << "  現有金幣：" << Player::getGold() << "G" << std::endl;
            std::cout << "  商品列表：" << std::endl;
            for (int i = 0; i < (int)shop.size(); i++)
                std::cout << "    " << (i+1) << ". [" << shop[i].name << "]"
                          << "  效果:" << shop[i].value
                          << "  售價:" << shop[i].price << "G" << std::endl;
            std::cout << "  0. 離開商店  > ";
            int choice = getInt(0, (int)shop.size());
            if (choice == 0) break;
            choice--;
            if (!Player::spendGold(shop[choice].price)) {
                std::cout << "  金幣不足！" << std::endl;
            } else {
                Player::addItem(Item(shop[choice].name, shop[choice].type,
                                     shop[choice].value, shop[choice].price));
                std::cout << "  購買成功！剩餘金幣：" << Player::getGold() << "G" << std::endl;
            }
        }
    }

    // ===== 賢者事件 =====
    void doSage(Team& team) {
        std::cout << std::endl;
        std::cout << "╔═══════════════════════╗" << std::endl;
        std::cout << "║  📖 古老賢者          ║" << std::endl;
        std::cout << "╚═══════════════════════╝" << std::endl;
        std::cout << "  「我可以傳授你古代的技術，但需要報酬。」" << std::endl;

        static const std::vector<Skill> sageSkills = {
            Skill("烈焰衝擊", 10, 20, Element::FIRE,    SkillType::ATTACK),
            Skill("冰水洪流", 10, 20, Element::WATER,   SkillType::ATTACK),
            Skill("風刃斬",   8,  16, Element::WIND,    SkillType::ATTACK),
            Skill("雷霆一擊", 12, 25, Element::THUNDER, SkillType::ATTACK),
            Skill("治癒之光", 8,  40, Element::NONE,    SkillType::HEAL),
            Skill("大治癒術", 15, 80, Element::NONE,    SkillType::HEAL),
            Skill("力量祝福", 6,  10, Element::NONE,    SkillType::BUFF),
        };
        const int SAGE_COST = 60;
        std::cout << "  學習費用：" << SAGE_COST << "G（現有：" << Player::getGold() << "G）" << std::endl;

        if (Player::getGold() < SAGE_COST) {
            std::cout << "  金幣不足，無法學習技能。" << std::endl;
            return;
        }

        std::cout << "  可學習技能：" << std::endl;
        for (int i = 0; i < (int)sageSkills.size(); i++) {
            std::cout << "    " << (i+1) << ". "; sageSkills[i].showInfo();
        }
        std::cout << "  0. 離開  > ";
        int si = getInt(0, (int)sageSkills.size());
        if (si == 0) return;
        si--;

        std::cout << "  選擇學習的成員：" << std::endl;
        for (int i = 0; i < team.size(); i++)
            std::cout << "    " << (i+1) << ". " << team.getMember(i).getName() << std::endl;
        std::cout << "  > ";
        int mi = getInt(1, team.size()) - 1;

        Player::spendGold(SAGE_COST);
        Player& learner = team.getMember(mi);
        if (!learner.learnSkill(sageSkills[si])) {
            std::cout << "  技能欄已滿！選擇遺忘哪個技能：" << std::endl;
            learner.showSkills();
            std::cout << "  > ";
            int fi = getInt(1, (int)learner.getSkills().size());
            learner.forgetSkill(fi - 1);
            learner.learnSkill(sageSkills[si]);
        }
    }

    // ===== 招募事件 =====
    void doRecruit(Team& team) {
        std::cout << std::endl;
        std::cout << "╔═══════════════════════╗" << std::endl;
        std::cout << "║  ⛺ 招募營地          ║" << std::endl;
        std::cout << "╚═══════════════════════╝" << std::endl;

        static const std::vector<std::tuple<std::string, JobClass, int, int, int, int>> candidates = {
            {"艾力克", JobClass::WARRIOR, 120, 30,  18, 6 },
            {"莉亞",   JobClass::MAGE,    60,  100, 6,  22},
            {"萊恩",   JobClass::PRIEST,  80,  80,  8,  16},
            {"薇拉",   JobClass::RANGER,  90,  55,  12, 12},
            {"鐵德",   JobClass::WARRIOR, 130, 25,  20, 5 },
            {"艾美",   JobClass::MAGE,    55,  110, 5,  24},
        };
        auto [cname, cjob, chp, cmp, catk, cmatk] = candidates[rand() % candidates.size()];
        Player candidate(cname, cjob, chp, cmp, catk, cmatk);

        std::cout << "  出現了一名冒險者！" << std::endl;
        candidate.showStatus();
        std::cout << std::endl;
        std::cout << "  1. 招募  2. 拒絕  > ";
        if (getInt(1, 2) == 2) { std::cout << "  「下次見吧…」" << std::endl; return; }

        if (!team.isFull()) {
            team.addMember(candidate);
        } else {
            std::cout << "  隊伍已滿！必須遣散一名成員。" << std::endl;
            for (int i = 0; i < team.size(); i++) {
                std::cout << "    " << (i+1) << ". ";
                team.getMember(i).showStatus();
            }
            std::cout << "  選擇遣散（0 = 取消招募）：> ";
            int di = getInt(0, team.size());
            if (di == 0) { std::cout << "  取消招募。" << std::endl; return; }
            team.removeMember(di - 1);
            team.addMember(candidate);
        }
    }

    // ===== 休息事件 =====
    void doRest(Team& team) {
        std::cout << std::endl;
        std::cout << "╔═══════════════════════╗" << std::endl;
        std::cout << "║  🏕 安全休息點        ║" << std::endl;
        std::cout << "╚═══════════════════════╝" << std::endl;
        std::cout << "  「一片寧靜之地，隊伍在此好好休憩。」" << std::endl;
        std::cout << "  所有成員回復 60% HP 與 50% MP！" << std::endl;
        for (int i = 0; i < team.size(); i++) {
            if (team.getMember(i).isAlive()) {
                team.getMember(i).heal(team.getMember(i).getMaxHp() * 60 / 100);
                team.getMember(i).recoverMp(team.getMember(i).getMaxMp() * 50 / 100);
            }
        }
        team.showStatus();
    }

public:
    Event(EventType type) : type(type) {
        switch (type) {
            case EventType::NORMAL_BATTLE: description = "⚔  一般戰鬥（普通怪物）"; break;
            case EventType::ELITE_BATTLE:  description = "💀 菁英戰鬥（強力怪物）"; break;
            case EventType::MERCHANT:      description = "🏪 旅行商人（購買道具）"; break;
            case EventType::SAGE:          description = "📖 古老賢者（學習技能）"; break;
            case EventType::RECRUIT:       description = "⛺ 招募營地（增加成員）"; break;
            case EventType::REST:          description = "🏕 安全休息點（回復HP/MP）"; break;
        }
    }

    EventType getType() const { return type; }
    const std::string& getDescription() const { return description; }

    bool execute(Team& team, int round) {
        switch (type) {
            case EventType::NORMAL_BATTLE: return doBattle(team, round, false);
            case EventType::ELITE_BATTLE:  return doBattle(team, round, true);
            case EventType::MERCHANT:      doMerchant(team); return true;
            case EventType::SAGE:          doSage(team);     return true;
            case EventType::RECRUIT:       doRecruit(team);  return true;
            case EventType::REST:          doRest(team);     return true;
        }
        return true;
    }
};

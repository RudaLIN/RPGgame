#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits>
#include "Team.h"
#include "Monster.h"
#include "Battle.h"
#include "Event.h"

// ===== Game 主遊戲類別 =====
class Game {
private:
    Team team;
    int currentRound;
    static const int MAX_ROUNDS = 10;

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

    // ===== 標題畫面 =====
    void showTitle() {
        std::cout << std::endl;
        std::cout << "╔══════════════════════════════════════════╗" << std::endl;
        std::cout << "║                                          ║" << std::endl;
        std::cout << "║   ★  闇影傳說：黎明遠征  ★            ║" << std::endl;
        std::cout << "║       Shadow Legend: Dawn Crusade        ║" << std::endl;
        std::cout << "║                                          ║" << std::endl;
        std::cout << "║   一款 Roguelike 文字冒險 RPG            ║" << std::endl;
        std::cout << "║   C++ OOP 課程專案                       ║" << std::endl;
        std::cout << "║                                          ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        std::cout << "  按下 Enter 開始遊戲..." << std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    // ===== 說明畫面 =====
    void showHelp() {
        std::cout << std::endl;
        std::cout << "══════════════ 遊戲說明 ══════════════" << std::endl;
        std::cout << "  • 目標：歷經 10 輪冒險後擊敗最終魔王" << std::endl;
        std::cout << "  • 每輪隨機出現兩個事件，選擇其中一個" << std::endl;
        std::cout << "  • 招募並強化你的隊伍（最多 4 人）" << std::endl;
        std::cout << "  • 隊伍全滅即 GAME OVER" << std::endl;
        std::cout << "  • 技能擁有屬性，命中弱點造成 1.5x 傷害" << std::endl;
        std::cout << "══════════════════════════════════════" << std::endl;
        std::cout << std::endl;
    }

    // ===== 初始化隊伍 =====
    void initTeam() {
        std::cout << std::endl;
        std::cout << "════ 選擇你的初始英雄 ════" << std::endl;
        std::cout << "  1. [戰士] 艾力克  HP:120 MP:30  ATK:18 MATK:6" << std::endl;
        std::cout << "  2. [法師] 莉亞    HP:60  MP:100 ATK:6  MATK:22" << std::endl;
        std::cout << "  3. [牧師] 萊恩    HP:80  MP:80  ATK:8  MATK:16" << std::endl;
        std::cout << "  4. [遊俠] 薇拉    HP:90  MP:55  ATK:12 MATK:12" << std::endl;
        std::cout << "  選擇你的主角（1-4）：> ";
        int choice = getInt(1, 4);

        Player protagonist =
            (choice == 1) ? Player("艾力克", JobClass::WARRIOR, 120, 30, 18, 6)  :
            (choice == 2) ? Player("莉亞",   JobClass::MAGE,    60, 100, 6, 22) :
            (choice == 3) ? Player("萊恩",   JobClass::PRIEST,  80, 80,  8, 16) :
                            Player("薇拉",   JobClass::RANGER,  90, 55, 12, 12);

        // 給初始技能
        switch (choice) {
            case 1:
                protagonist.learnSkill(Skill("雷霆一擊", 10, 18, Element::THUNDER, SkillType::ATTACK));
                break;
            case 2:
                protagonist.learnSkill(Skill("烈焰衝擊", 10, 22, Element::FIRE, SkillType::ATTACK));
                break;
            case 3:
                protagonist.learnSkill(Skill("治癒之光", 8, 40, Element::NONE, SkillType::HEAL));
                protagonist.learnSkill(Skill("風刃斬", 8, 14, Element::WIND, SkillType::ATTACK));
                break;
            case 4:
                protagonist.learnSkill(Skill("風刃斬", 8, 14, Element::WIND, SkillType::ATTACK));
                protagonist.learnSkill(Skill("力量祝福", 6, 8, Element::NONE, SkillType::BUFF));
                break;
        }

        team.addMember(protagonist);

        // 免費贈送一名初始夥伴
        std::cout << std::endl;
        std::cout << "  系統：一位陌生冒險者願意與你同行！" << std::endl;
        int partnerChoice = (choice == 1) ? 3 : 1; // 非同職業
        Player partner =
            (partnerChoice == 1) ? Player("艾力克", JobClass::WARRIOR, 120, 30, 18, 6)  :
            (partnerChoice == 3) ? Player("萊恩",   JobClass::PRIEST,  80, 80,  8, 16) :
                                   Player("薇拉",   JobClass::RANGER,  90, 55, 12, 12);
        if (partnerChoice == 3)
            partner.learnSkill(Skill("治癒之光", 8, 40, Element::NONE, SkillType::HEAL));
        else
            partner.learnSkill(Skill("雷霆一擊", 10, 18, Element::THUNDER, SkillType::ATTACK));
        team.addMember(partner);

        // 初始道具（存入共用背包）
        Player::addItem(Item("小型回復藥", ItemType::HEAL_HP, 50, 30, 2));
        Player::addItem(Item("魔力水", ItemType::RECOVER_MP, 30, 25, 1));
    }

    // ===== 產生兩個隨機附加事件（一般戰鬥固定在每輪最前面）=====
    std::vector<Event> generateEvents(int round) {
        std::vector<EventType> pool = {
            EventType::MERCHANT,
            EventType::SAGE,
            EventType::RECRUIT,
            EventType::REST,
        };
        if (round >= 4) pool.push_back(EventType::ELITE_BATTLE);
        if (round >= 7) pool.push_back(EventType::ELITE_BATTLE);

        // 隨機挑兩個不同事件
        int a = rand() % pool.size();
        int b;
        do { b = rand() % pool.size(); } while (b == a && pool.size() > 1);
        return { Event(pool[a]), Event(pool[b]) };
    }

    // ===== 魔王戰 =====
    bool bossBattle() {
        std::cout << std::endl;
        std::cout << "╔══════════════════════════════════════════╗" << std::endl;
        std::cout << "║  !!!  最終決戰！魔王降臨！  !!!          ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        std::cout << "  「愚蠢的凡人…你們竟敢挑戰本王？」" << std::endl;
        std::cout << "  「以黑暗之力，粉碎你們！」" << std::endl;
        std::cout << std::endl;
        std::cout << "  按下 Enter 迎戰..." << std::endl;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        Monster boss = MonsterFactory::createBoss();
        Battle battle(team, boss);
        return battle.run();
    }

    // ===== 勝利結局 =====
    void showVictory() {
        std::cout << std::endl;
        std::cout << "╔══════════════════════════════════════════╗" << std::endl;
        std::cout << "║                                          ║" << std::endl;
        std::cout << "║   ★★★  Victory！  ★★★              ║" << std::endl;
        std::cout << "║                                          ║" << std::endl;
        std::cout << "║   黑暗魔王已被擊敗！世界重回光明！      ║" << std::endl;
        std::cout << "║   你的傳說將永遠流傳於大地之上。        ║" << std::endl;
        std::cout << "║                                          ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════╝" << std::endl;
        std::cout << std::endl;
        std::cout << "  最終隊伍：" << std::endl;
        team.showStatus();
    }

    // ===== 失敗結局 =====
    void showGameOver() {
        std::cout << std::endl;
        std::cout << "╔══════════════════════════════════════════╗" << std::endl;
        std::cout << "║                                          ║" << std::endl;
        std::cout << "║   ✝  GAME OVER                          ║" << std::endl;
        std::cout << "║                                          ║" << std::endl;
        std::cout << "║   隊伍全滅，黑暗籠罩大地…              ║" << std::endl;
        std::cout << "║   英雄的傳說就此終結。                  ║" << std::endl;
        std::cout << "║                                          ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════╝" << std::endl;
    }

public:
    Game() : currentRound(1) {
        srand((unsigned)time(nullptr));
    }

    void run() {
        showTitle();
        showHelp();
        initTeam();

        // ===== 主迴圈：10 輪 =====
        while (currentRound <= MAX_ROUNDS) {
            std::cout << std::endl;
            std::cout << "╔══════════════════════════════════════════╗" << std::endl;
            std::cout << "║  第 " << currentRound << " / " << MAX_ROUNDS
                      << " 輪                                  ║" << std::endl;
            std::cout << "╚══════════════════════════════════════════╝" << std::endl;
            team.showStatus();

            // ── 強制一般戰鬥 ──
            std::cout << std::endl;
            std::cout << "  【必要遭遇】前方出現了怪物！" << std::endl;
            Event mandatoryBattle(EventType::NORMAL_BATTLE);
            if (!mandatoryBattle.execute(team, currentRound) || team.isWiped()) {
                showGameOver();
                return;
            }

            // ── 隨機附加事件二選一 ──
            auto events = generateEvents(currentRound);
            std::cout << std::endl;
            std::cout << "  繼續前進，出現了岔路，請選擇其中一個：" << std::endl;
            std::cout << "    1. " << events[0].getDescription() << std::endl;
            std::cout << "    2. " << events[1].getDescription() << std::endl;
            std::cout << "  > ";
            int choice = getInt(1, 2) - 1;

            bool alive = events[choice].execute(team, currentRound);
            if (!alive || team.isWiped()) {
                showGameOver();
                return;
            }

            // 回合結束提示
            std::cout << std::endl;
            std::cout << "  ── 第 " << currentRound << " 輪結束 ──" << std::endl;
            currentRound++;
        }

        // ===== 魔王戰 =====
        bool won = bossBattle();
        if (won)
            showVictory();
        else
            showGameOver();
    }
};

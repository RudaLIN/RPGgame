#pragma once
#include <string>
#include <iostream>
#include "Types.h"

// ===== Item 道具類別 =====
class Item {
private:
    std::string name;
    ItemType itemType;
    int value;      // 效果數值（回血量、回魔量、強化值）
    int price;      // 商店售價
    int quantity;   // 數量

public:
    Item(const std::string& name, ItemType itemType, int value, int price, int quantity = 1)
        : name(name), itemType(itemType), value(value), price(price), quantity(quantity) {}

    // Getter / Setter
    const std::string& getName() const { return name; }
    ItemType getItemType() const { return itemType; }
    int getValue() const { return value; }
    int getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    void setQuantity(int q) { quantity = q; }

    // 判斷道具是否可用
    bool isAvailable() const { return quantity > 0; }

    // 使用道具（消耗一個，回傳效果值）
    int use() {
        if (!isAvailable()) {
            std::cout << "  [" << name << "] 已用完！" << std::endl;
            return 0;
        }
        quantity--;
        std::cout << "  使用了 [" << name << "]！" << std::endl;
        return value;
    }

    // 顯示道具資訊
    void showInfo() const {
        std::cout << "[" << name << "] ";
        switch (itemType) {
            case ItemType::HEAL_HP:    std::cout << "回復HP +" << value; break;
            case ItemType::RECOVER_MP: std::cout << "回復MP +" << value; break;
            case ItemType::BUFF:       std::cout << "暫時強化 +" << value; break;
        }
        std::cout << "  數量:" << quantity << "  售價:" << price << "金" << std::endl;
    }
};

#pragma once

#include "AnimatedSprite.h"
#include "Sound.h"
#include "Transform.h"
#include "WeaponInfo.h"
#include "entt/entt.hpp"
#include <optional>
#include <functional>
#include <SDL3/SDL.h>
#include <vector>

struct Attack
{
    enum Type
    {
        CLOSE,
        RANGED,
        MAGIC
    };

    enum DamageType
    {
        DMG_WEAPON,
        DMG_MAGIC,
        DMG_FIXED
    };
    
    Type type = Type::CLOSE;
    DamageType damagetype = DamageType::DMG_WEAPON;
    
    SDL_FRect rect = {0, 0, 0, 0};
    // SDL_Point origin; see src_point
    
    int mobCount = 1;
    int attackCount = 1;
    int damage = 50;

    AnimatedSprite::Wrap *hit = nullptr;
    // 攻击源的坐标
    std::optional<SDL_FPoint> src_point = std::nullopt;
    // 攻击后击中音效
    Sound::Wrap *souw = nullptr;

    // 伤害浮动范围
    float min_damage = 0.8;
    float max_damage = 1.2;
    
    float critical = 0.0f;
    float ignoredef = 0.0f;
    int32_t matk = 0;
    int32_t accuracy = 0;
    int32_t fixdamage = 0;
    int16_t playerlevel = 1;
    
    uint8_t hitcount = 0;
    uint8_t mobcount = 0;
    uint8_t speed = 0;
    uint8_t stance = 0;
    int32_t skill = 0;
    int32_t bullet = 0;
    
    float hrange = 1.0f;
    bool toleft = false;
    
    std::optional<std::function<void(entt::entity, entt::entity, int)>> call_back = std::nullopt;

    Attack() = default;
    Attack(SDL_FPoint &lt, SDL_FPoint &rb, AnimatedSprite::Wrap *hit,
           int mobCount = 1, int attackCount = 1,
           Sound::Wrap *souw = nullptr, int damage = 50);
};

struct MobAttack
{
    Attack::Type type = Attack::Type::CLOSE;
    int32_t watk = 0;
    int32_t matk = 0;
    int32_t mobid = 0;
    int32_t oid = 0;
    SDL_FPoint origin;
    bool valid = false;

    // Create a mob attack for touch damage
    MobAttack() : valid(false) {}
    MobAttack(int32_t watk, SDL_FPoint origin, int32_t mobid, int32_t oid) : type(Attack::Type::CLOSE), watk(watk), origin(origin), mobid(mobid), oid(oid), valid(true) {}

    explicit operator bool() const
    {
        return valid;
    }
};

struct MobAttackResult
{
    int32_t damage;
    int32_t mobid;
    int32_t oid;
    uint8_t direction;

    MobAttackResult(const MobAttack& attack, int32_t damage, uint8_t direction) : damage(damage), direction(direction), mobid(attack.mobid), oid(attack.oid) {}
};

struct AttackResult
{
    AttackResult() {}

    AttackResult(const Attack& attack)
    {
        type = attack.type;
        hitcount = attack.hitcount;
        skill = attack.skill;
        speed = attack.speed;
        stance = attack.stance;
        bullet = attack.bullet;
        toleft = attack.toleft;
    }

    Attack::Type type;
    int32_t attacker = 0;
    uint8_t mobcount = 0;
    uint8_t hitcount = 1;
    int32_t skill = 0;
    int32_t charge = 0;
    int32_t bullet = 0;
    uint8_t level = 0;
    uint8_t display = 0;
    uint8_t stance = 0;
    uint8_t speed = 0;
    bool toleft = false;
    std::unordered_map<int32_t, std::vector<std::pair<int32_t, bool>>> damagelines;
    int32_t first_oid;
    int32_t last_oid;
};

struct AttackUser
{
    int32_t skilllevel;
    uint16_t level;
    bool secondweapon;
    bool flip;
};
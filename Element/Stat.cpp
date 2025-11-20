#include "Stat.h"
#include "../Log/LogSystem.h"
#include <algorithm>
#include <cassert>
#include <iostream>

const int Stat::Default_Attack = 1;
const int Stat::Default_Defense = 0;
const int Stat::Default_Speed = 5;
const int Stat::Default_Hitpoint = 10;
const int Stat::Default_Threat = 0;

Stat Stat::Empty() {
    return Stat(0, 0, 0, 0, 0);
}

Stat &Stat::operator+=(const Stat &other) {
    Attack += other.Attack;
    Defense += other.Defense;
    Hitpoint += other.Hitpoint;
    Speed += other.Speed;
    Threat += other.Threat;
    return *this;
}

Stat &Stat::operator-=(const Stat &other) {
    Attack -= other.Attack;
    Defense -= other.Defense;
    Hitpoint -= other.Hitpoint;
    Speed -= other.Speed;
    Threat -= other.Threat;
    return *this;
}
Stat Stat::operator+(const Stat &other) const {
    return Stat(Attack + other.Attack, Defense + other.Defense, Hitpoint + other.Hitpoint, Speed + other.Speed,
                Threat + other.Threat);
}

Stat Stat::operator*(int scalar) const {
    return Stat(Attack * scalar, Defense * scalar, Hitpoint * scalar, Speed * scalar, Threat * scalar);
}

Stat::Stat()
    : Attack(Default_Attack), Defense(Default_Defense), Hitpoint(Default_Hitpoint), Speed(Default_Speed),
      Threat(Default_Threat) {}

Stat::Stat(int attack, int defense, int hitpoint, int speed, int threat)
    : Attack(attack), Defense(defense), Hitpoint(hitpoint), Speed(speed), Threat(threat) {}

Stat::~Stat() {}

int Stat::GetSpeed() const {
    return Speed;
}

void Stat::SetSpeed(int s) {
    LogSystem::LogStream("Speed set from ", Speed, " to ", s);
    Speed = std::max(0, s);
}

int Stat::GetHP() const {
    return Hitpoint;
}

void Stat::SetHP(int hp) {
    LogSystem::LogStream("HP set from ", Hitpoint, " to ", hp);
    Hitpoint = std::max(0, hp);
}

int Stat::GetAttack() const {
    return Attack;
}

void Stat::SetAttack(int a) {
    int attack = std::max(0, a);
    LogSystem::LogStream("Attack set from ", Attack, " to ", attack);
    Attack = attack;
}

int Stat::GetDefense() const {
    return Defense;
}

void Stat::SetDefense(int d) {
    int defense = std::max(0, d);
    LogSystem::LogStream("Defense set from ", Defense, " to ", defense);
    Defense = defense;
}

int Stat::GetThreat() const {
    return Threat;
}

void Stat::SetThreat(int t) {
    LogSystem::LogStream("Threat set from ", Threat, " to ", t);
    Threat = std::max(0, t);
}
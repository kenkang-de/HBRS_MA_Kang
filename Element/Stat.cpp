#include "Stat.h"
#include <cassert>
#include <iostream>
#include <algorithm>

const int Stat::Default_Attack = 1;
const int Stat::Default_Defense = 0;
const int Stat::Default_Speed = 5;
const int Stat::Default_Hitpoint = 10;
const int Stat::Default_Threat = 0;

Stat Stat::Empty() {
    return Stat(0, 0, 0, 0, 0);
}

Stat& Stat::operator+=(const Stat& other) {
    Attack  += other.Attack;
    Defense += other.Defense;
    Hitpoint+= other.Hitpoint;
    Speed   += other.Speed;
    Threat  += other.Threat;
    return *this;
}

Stat& Stat::operator-=(const Stat& other) {
    Attack  -= other.Attack;
    Defense -= other.Defense;
    Hitpoint-= other.Hitpoint;
    Speed   -= other.Speed;
    Threat  -= other.Threat;
    return *this;
}
Stat Stat::operator+(const Stat& other) const
{
    return Stat(
        Attack + other.Attack,
        Defense + other.Defense,
        Hitpoint + other.Hitpoint,
        Speed + other.Speed,
        Threat + other.Threat
    );
}

Stat Stat::operator*(int scalar) const
{
    return Stat(
        Attack * scalar,
        Defense * scalar,
        Hitpoint * scalar,
        Speed * scalar,
        Threat * scalar
    );
}

Stat::Stat()
    : Attack(Default_Attack),
      Defense(Default_Defense),
      Hitpoint(Default_Hitpoint),
      Speed(Default_Speed),
      Threat(Default_Threat)
{}

Stat::Stat(int attack, int defense, int hitpoint, int speed, int threat):
Attack(attack),
Defense(defense),
Hitpoint(hitpoint),
Speed(speed),
Threat(threat)
{}

Stat::~Stat() {}

int Stat::GetSpeed() const
{
    return Speed;
}

void Stat::SetSpeed(int s)
{
    Speed = std::max(0, s);
}

int Stat::GetHP() const
{
    return Hitpoint;
}
    
void Stat::SetHP(int hp)
{
    Hitpoint = std::max(0, hp);
}

int Stat::GetAttack() const {
    return Attack;
}

void Stat::SetAttack(int a) {
    Attack = std::max(0, a);
}

int Stat::GetDefense() const {
    return Defense;
}

void Stat::SetDefense(int d) {
    Defense = std::max(0, d);
}

int Stat::GetThreat() const {
    return Threat;
}

void Stat::SetThreat(int t) {
    Threat = std::max(0, t);
}
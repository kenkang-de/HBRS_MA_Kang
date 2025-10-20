#ifndef STAT_H
#define STAT_H

class Stat
{
private:
    int Attack;
    int Defense;
    int Hitpoint;
    int Speed;
    int Threat;

static const int Default_Attack;
static const int Default_Defense;
static const int Default_Speed;
static const int Default_Hitpoint;
static const int Default_Threat;

public:
static Stat Empty();

Stat& operator+=(const Stat& other);  
Stat& operator-=(const Stat& other);  
Stat operator+(const Stat& other) const; 
Stat operator*(int scalar) const;

    Stat();
    ~Stat();
    Stat(int attack, int defense, int hitpoint, int speed, int threat);
    Stat(const Stat&) = default;
    Stat& operator=(const Stat&) = default;

    int GetHP() const;
    void SetHP(int hp);

    int GetAttack() const;
void SetAttack(int a);

int GetDefense() const;
void SetDefense(int d);

int GetThreat() const;
void SetThreat(int t);

    int GetSpeed() const;
    void SetSpeed(int s);
};


#endif
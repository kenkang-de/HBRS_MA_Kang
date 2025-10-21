#ifndef BALANCINGLANE_H
#define BALANCINGLANE_H

#include "../Element/Stat.h"

class Chromosome;

enum CHROMOSOMETYPE
{
    ALPHA,BETA
};


class BalancingLane
{
    private:
    CHROMOSOMETYPE chromosomeType;
    Chromosome* chromosome;
    bool ownsAdjustment;

    public:

    const Stat* adjustment;

    BalancingLane() : adjustment(nullptr), chromosomeType(ALPHA), chromosome(nullptr), ownsAdjustment(false) {};

    BalancingLane(const Stat* _adjustment, CHROMOSOMETYPE _chromosomeType) : 
        adjustment(_adjustment), chromosomeType(_chromosomeType), chromosome(nullptr), ownsAdjustment(false)
    {};

    // Copy constructor
    BalancingLane(const BalancingLane& other) : 
        adjustment(other.adjustment), chromosomeType(other.chromosomeType), 
        chromosome(other.chromosome), ownsAdjustment(false) // Don't copy ownership
    {};

    // Assignment operator
    BalancingLane& operator=(const BalancingLane& other) {
        if (this != &other) {
            adjustment = other.adjustment;
            chromosomeType = other.chromosomeType;
            chromosome = other.chromosome;
            ownsAdjustment = false; // Don't copy ownership
        }
        return *this;
    }

    ~BalancingLane() {
        if (ownsAdjustment) {
            delete adjustment;  
        }
    }

    Chromosome* GetChromosome() {return chromosome;}
    void SetChromosome(Chromosome* _chromosome); 

    CHROMOSOMETYPE GetChromosomeType() { return chromosomeType; }

    bool GetOwnAdjustment() {return ownsAdjustment;}
    void SetOwnAdjustment(bool value) {ownsAdjustment = value;}

 
};


#endif
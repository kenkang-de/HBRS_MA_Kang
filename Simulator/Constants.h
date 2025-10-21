// Constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H


const int UNITS_PER_TEAM =5;

const int TEST_TICK = 50;

// Simulation configuration constants
const int SIMULATION_COUNT = 100; // Number of simulations to run in batch testing

// Delay calculation constants
const float DELAY_MULTIPLIER = 2.0f; // Multiplier for damage-based delay calculation

//Multiplier for the counter strategy calculation.
const float MULTIPLIER_COUNTER = 1.5f;
const float MULTIPLIER_BASIC =1.0f;

//balancing target winrate
const int ALPHA_NUM=5;

const int BETA_NUM=20;

const float TARGET_WINRATE=0.5f;

const float TARGET_THRESHOLD=0.05f;

const float FITNESS_THRESHOLD=0.05f;

const float FITNESS_MAX =2.0f;

const int MAXGENERATION=30;


#endif

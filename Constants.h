// Constants.h
#ifndef CONSTANTS_H
#define CONSTANTS_H

const int UNITS_PER_TEAM = 5;

// Delay calculation constants
const float DELAY_MULTIPLIER = 2.0f; // Multiplier for damage-based delay calculation

// Multiplier for the counter strategy calculation.
const float MULTIPLIER_COUNTER = 1.5f;
const float MULTIPLIER_BASIC = 1.0f;

// Novelty K
const int NOVELTYK = 5;

// BALANCING
const float DEFENSE_RATIO = 0.5f;
const float SPEED_RATIO = 0.3f;

const float TARGET_WINRATE = 0.5f;
const float TARGET_THRESHOLD = 0.05f;

const float FITNESS_THRESHOLD = 0.1f;
const float FITNESS_MAX = 1.0f;
const float RMSE_WEIGHT = 0.9f;
const float DOC_WEIGHT = 0.1f;

#endif

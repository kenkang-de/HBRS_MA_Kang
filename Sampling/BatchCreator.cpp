#include <iostream>
#include <string>
#include <vector>

#include "SamplingController.h"
#include "Batch.h"

std::vector<Batch> main(int argc, char* argv[]) {
    
// argv[0] = "BatchCreator.exe"
// argv[1] = numBatches 
// argv[2] = teamsPerBatch

int numBatches;
int teamsPerBatch;

if (argc > 1) numBatches = std::atoi(argv[1]);         
if (argc > 2) teamsPerBatch = std::atoi(argv[2]);       

std::vector<Batch> batches(numBatches);

int batchCounter = 1;
for (Batch& batch : batches) {
    batch.batchId = "Batch_" + std::to_string(batchCounter++);
    batch.teams = std::vector<Team>(teamsPerBatch);
}


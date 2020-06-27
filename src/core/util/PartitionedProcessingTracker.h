//
// Created by rutio on 2020-06-02.
//

#ifndef LUCIMALIR_PARTITIONEDPROCESSINGTRACKER_H
#define LUCIMALIR_PARTITIONEDPROCESSINGTRACKER_H

#include <vector>
#include <cstdint>
#include <mutex>

class PartitionedProcessingTracker {
public:
  explicit PartitionedProcessingTracker(int count);

  int getNumDispatched();

  int getNumTotal();

  bool didDispatchAll();

  bool didProcessAll();

  int getNextIndex();

  void notifyProcessingEnd(int index);

private:
  std::vector<uint8_t> processed;
  int dispatched = 0;
  int amount;
  std::mutex mut;
};


#endif //LUCIMALIR_PARTITIONEDPROCESSINGTRACKER_H

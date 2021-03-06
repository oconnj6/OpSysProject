#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "Process.h"
#include <ostream>
#include <fstream>
#include <algorithm>


void printQueue(std::vector<Process*> &processes, std::vector<int> readyQueue) {
  if (readyQueue.size() == 0) {
    std::cout << " [Q <empty>]" << std::endl;
  }
  else {
    std::cout << " [Q ";
    for (unsigned int i = 0;i < readyQueue.size();i++) {
      std::cout << processes[readyQueue[i]]->getName();
      if (i != readyQueue.size()-1) {
        std::cout << " ";
      }
    }
    std::cout << "]" << std::endl;
  }
}

void printQueue(std::vector<Process*> readyQueue) {
  if (readyQueue.size() == 0) {
    std::cout << " [Q <empty>]" << std::endl;
  }
  else {
    std::cout << " [Q ";
    for (unsigned int i = 0;i < readyQueue.size();i++) {
      std::cout << readyQueue[i]->getName();
      if (i != readyQueue.size()-1) {
        std::cout << " ";
      }
    }
    std::cout << "]" << std::endl;
  }
}

void FCFS(std::vector<Process*> &processes, int contextSwitch, std::ofstream & outputFile) {
  std::vector<int> readyQueue;
  std::vector<int> waitQueue;
  unsigned int doneP = 0;
  int CPUTime = 0;
  int CPUProcess = 99;
  int CPUFinished = -1;
  int pAdded = 0;
  int CPUStart = -1;
  int waitBlock = 0;
  double totalBurstTime = 0;
  int totalBursts = 0;
  double totalWaitTime = 0;
  int totalWaits = 0;
  int totalTurnATime = 0;
  int totalTurns = 0;
  int totalCSwitch = 0;
  int time;
  int addToTime = 0;
  for (time = 0; time > -1;time++) {
    for (unsigned int i = 0;i < processes.size(); i++) {
      if (processes[i]->getIAT() == time) {
        readyQueue.push_back(i);
        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << processes[readyQueue[readyQueue.size()-1]]->getName();
          std::cout << " arrived; added to ready queue";
          printQueue(processes, readyQueue);
        }
        pAdded++;
      }
    }

    if ( CPUStart == time ) {
      if (time < 1000) {
        std::cout << "time " << time << "ms: Process " << processes[CPUProcess]->getName();
        std::cout << " started using the CPU for " << CPUTime << "ms burst";
        printQueue(processes, readyQueue);
      }
        totalBurstTime += CPUTime;
        totalBursts++;
        totalCSwitch++;

        totalTurnATime += (CPUTime + (contextSwitch / 2));
        totalTurns++;
    }

    if (readyQueue.size() > 0) {
      if ( CPUProcess == 99 ) {
        CPUProcess = readyQueue[0];
        readyQueue.erase(readyQueue.begin());
        if (processes[CPUProcess]->getBurstNum() != 0) {
          CPUTime = processes[CPUProcess]->getCPUTime();
          int cs = contextSwitch / 2;
          cs = cs - addToTime;
          addToTime = 0;
          CPUStart = time + cs;
          processes[CPUProcess]->setCPUDone( CPUTime );
          CPUFinished = time + CPUTime + cs;
          //std::cout << CPUFinished << std::endl;
        }
      }
    }

    if ( (time >= CPUFinished && CPUFinished != -1) || (CPUProcess != 99 && processes[CPUProcess]->getRemBursts() == 0)) {
      if (processes[CPUProcess]->getCPUDone() != 0 && waitBlock < 2 && processes[CPUProcess]->getRemBursts() != 0 ) {
        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << processes[CPUProcess]->getName();
          std::cout << " completed a CPU burst; " << processes[CPUProcess]->getRemBursts();
          std::cout << " bursts to go";
          printQueue(processes, readyQueue);
        }
        if (waitBlock > 0) {
          waitBlock++;
        }
      }

      if ( CPUProcess != 99) {
        if (processes[CPUProcess]->getRemBursts() != 0 && waitBlock == 0) {
          int IOTime = processes[CPUProcess]->getIOTime();
          if (time < 1000) {
            std::cout << "time " << time << "ms: Process " << processes[CPUProcess]->getName();
            std::cout << " switching out of CPU; will block on I/O until time ";
            std::cout << time + IOTime + contextSwitch / 2 << "ms";
            printQueue(processes, readyQueue);
          }
          waitBlock++;
          processes[CPUProcess]->setWaitDone(time + IOTime + (contextSwitch / 2));
          if (readyQueue.size() == 0) {
            CPUFinished = -1;
          }
          totalCSwitch++;
          totalWaitTime += IOTime;
          totalWaits ++;

          waitQueue.push_back(CPUProcess);
          CPUProcess = 99;
        }
        else if (processes[CPUProcess]->getRemBursts() == 0 ) {
          std::cout << "time " << time << "ms: Process " << processes[CPUProcess]->getName();
          std::cout << " terminated";

          printQueue(processes, readyQueue);
          doneP++;
          CPUProcess = 99;
          if (readyQueue.size() == 0) {
            CPUFinished = -1;
          }
          if (doneP == processes.size())
            break;
        }
      }
    }
    if ( waitQueue.size() > 0 ) {
      if (time == processes[waitQueue[0]]->getWaitDone()) {
        waitBlock = 0;

        readyQueue.push_back(waitQueue[0]);
        waitQueue.erase(waitQueue.begin());
        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << processes[waitQueue[0]]->getName();
          std::cout << " completed I/O; added to ready queue";
          printQueue(processes, readyQueue);
        }
        addToTime = 1;
      }
    }
  }
  std::cout << "time " << time + (contextSwitch / 2) << "ms: Simulator ended for FCFS";
  printQueue(processes, readyQueue);

  if (outputFile.is_open()) {
    outputFile << "Algorithm FCFS" << std::endl;
    outputFile.setf(std::ios::fixed,std::ios::floatfield);
    outputFile.precision(3);
    outputFile << "-- average CPU burst time: " << totalBurstTime / (double)totalBursts << " ms" << std::endl;
    outputFile << "-- average wait time: " << totalWaitTime / (double)totalWaits << " ms" << std::endl;
    outputFile << "-- average turnaround time: " << totalTurnATime / (double)totalTurns << " ms" << std::endl;
    outputFile << "-- total number of context switches: " << totalCSwitch << std::endl;
    outputFile << "-- total number of preemptions: 0" << std::endl;
  }
}


bool comparator(Process * & lhs, Process * & rhs) {
  return lhs->getIAT() > rhs->getIAT();
}

bool SJFcomparator(Process * & lhs, Process * & rhs) {
  return lhs->getCPUTimeNoSped() > rhs->getCPUTimeNoSped();
}

bool isIOBlockTime(std::vector<std::pair<int, Process*> > & ioBlockUntil, int time) {
  for (unsigned int i = 0; i < ioBlockUntil.size(); i++) {
    if (ioBlockUntil[i].first == time) return true;
  }
  return false;
}

bool isComplete(std::vector<Process*> & processes, int time, int offCPUTime) {
  bool t = true;
  for (unsigned int i = 0; i < processes.size(); i++) {
    if (!processes[i]->isDone()) t = false;
    else if (processes[i]->isDone() && !processes[i]->printedIsDone){
      processes[i]->printedIsDone = true;
      if (processes.size() == 1 || processes.size() == 2)
        std::cout << "time " << offCPUTime << "ms: Process " << processes[i]->getName() << " terminated [ADD Q]" << std::endl;
    }
  }
  return t;
}

void SJF(std::vector<Process*> &processes, double lambda, int contextSwitch, std::ofstream & outputFile) {
  std::vector<Process*> readyQueue;
  std::vector<Process*> waitQueue;
  unsigned int doneP = 0;
  int CPUTime = 0;
  Process * CPUProcess = NULL;
  int CPUFinished = -1;
  int pAdded = 0;
  int CPUStart = -1;
  int waitBlock = 0;
  double totalBurstTime = 0;
  int totalBursts = 0;
  double totalWaitTime = 0;
  int totalWaits = 0;
  int totalTurnATime = 0;
  int totalTurns = 0;
  int totalCSwitch = 0;
  int time;
  for (time = 0; time > -1;time++) {
    for (unsigned int i = 0;i < processes.size(); i++) {
      if (processes[i]->getIAT() == time) {
        processes[i]->addToBurstAvg(1 / lambda);
        processes[i]->getRunningBurstAvg();
        readyQueue.push_back(processes[i]);
        std::sort(readyQueue.begin(), readyQueue.end(), SJFcomparator);
        std::reverse(readyQueue.begin(), readyQueue.end());
        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << readyQueue[readyQueue.size()-1]->getName();
          std::cout << " (tau " << readyQueue[readyQueue.size()-1]->getTau() << "ms) arrived; added to ready queue";
          printQueue(readyQueue);
        }
        pAdded++;
      }
    }

    if ( CPUStart == time ) {
      if (time < 1000) {
        std::cout << "time " << time << "ms: Process " << CPUProcess->getName();
        std::cout << " (tau " << CPUProcess->getTau() << "ms) started using the CPU for " << CPUTime << "ms burst";
        printQueue(readyQueue);
      }
        totalBurstTime += CPUTime;
        totalBursts++;

        totalCSwitch++;
        totalTurnATime += (CPUTime + 2);
        totalTurns++;
    }

    if (readyQueue.size() > 0) {
      if ( CPUProcess == NULL ) {
        CPUProcess = readyQueue[0];
        readyQueue.erase(readyQueue.begin());
        CPUTime = CPUProcess->getCPUTime();
        int cs = 1;
        if (CPUProcess->getIAT() == time)
          cs = 2;

        CPUStart = time + cs;
        CPUProcess->setCPUDone( CPUTime );
        CPUFinished = time + CPUTime + cs;
        //std::cout << CPUFinished << std::endl;
      }
    }

    if ( time >= CPUFinished && CPUFinished != -1) {
      if (CPUProcess->getCPUDone() != 0 && waitBlock < 2 && CPUProcess->getRemBursts() != 0 ) {
        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << CPUProcess->getName();
          std::cout << " (tau " << CPUProcess->getTau() << "ms) completed a CPU burst; " << CPUProcess->getRemBursts();
          std::cout << " bursts to go";
          printQueue(readyQueue);
        }
        CPUProcess->addToBurstAvg(CPUTime);
        CPUProcess->getRunningBurstAvg();
        if (time < 1000) {
          std::cout << "time " << time << "ms: Recalculated tau = " << CPUProcess->getTau();
          std::cout << "ms for process " << CPUProcess->getName();
          printQueue(readyQueue);
        }

        if (waitBlock > 0) {
          waitBlock++;
        }
      }

      if ( CPUProcess != NULL) {
        if (CPUProcess->getRemBursts() != 0 && waitBlock == 0) {
          int IOTime = CPUProcess->getIOTime();
          if (time < 1000) {
            std::cout << "time " << time << "ms: Process " << CPUProcess->getName();
            std::cout << " switching out of CPU; will block on I/O until time ";
            std::cout << time + IOTime << "ms";
            printQueue(readyQueue);
          }
          waitBlock++;
          CPUProcess->setWaitDone(time + IOTime);
          if (readyQueue.size() == 0) {
            CPUFinished = -1;
          }
          totalCSwitch++;
          totalWaitTime += IOTime;
          totalWaits ++;

          waitQueue.push_back(CPUProcess);
          CPUProcess = NULL;
        }
        else if (CPUProcess->getRemBursts() == 0 ) {
          std::cout << "time " << time << "ms: Process " << CPUProcess->getName();
          std::cout << " terminated";
          printQueue(readyQueue);
          doneP++;
          CPUProcess = NULL;
          if (readyQueue.size() == 0) {
            CPUFinished = -1;
          }
          if (doneP == processes.size())
            break;
        }
      }
    }
    if ( waitQueue.size() > 0 ) {
      if (time == waitQueue[0]->getWaitDone()) {
        waitBlock = 0;

        readyQueue.push_back(waitQueue[0]);
        std::sort(readyQueue.begin(), readyQueue.end(), SJFcomparator);
        std::reverse(readyQueue.begin(), readyQueue.end());
        waitQueue.erase(waitQueue.begin());
        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << waitQueue[0]->getName();
          std::cout << " (tau " << waitQueue[0]->getTau() << "ms) completed I/O; added to ready queue";
          printQueue(readyQueue);
        }
      }
    }
  }
  std::cout << "time " << time + (contextSwitch / 2) << "ms: Simulator ended for SJF";
  printQueue(readyQueue);

  if (outputFile.is_open()) {
    outputFile << "Algorithm SJF" << std::endl;
    outputFile.setf(std::ios::fixed,std::ios::floatfield);
    outputFile.precision(3);
    outputFile << "-- average CPU burst time: " << totalBurstTime / (double)totalBursts << " ms" << std::endl;
    outputFile << "-- average wait time: " << totalWaitTime / (double)totalWaits << " ms" << std::endl;
    outputFile << "-- average turnaround time: " << totalTurnATime / (double)totalTurns << " ms" << std::endl;
    outputFile << "-- total number of context switches: " << totalCSwitch << std::endl;
    outputFile << "-- total number of preemptions: 0" << std::endl;
  }
}

void SRT(std::vector<Process*> &processes, double lambda, int contextSwitch, std::ofstream & outputFile) {
  std::vector<Process*> readyQueue;
  std::vector<Process*> waitQueue;
  unsigned int doneP = 0;
  int CPUTime = 0;
  Process* CPUProcess = NULL;
  int CPUFinished = -1;
  int pAdded = 0;
  int CPUStart = -1;
  int waitBlock = 0;
  double totalBurstTime = 0;
  int totalBursts = 0;
  double totalWaitTime = 0;
  int totalWaits = 0;
  double totalTurnATime = 0;
  int totalTurns = 0;
  int totalPreemptions = 0;
  int totalCSwitch = 0;

  int time;
  for (time = 0; time > -1;time++) {
    for (unsigned int i = 0;i < processes.size(); i++) {
      if (processes[i]->getIAT() == time) {
        processes[i]->addToBurstAvg(1 / lambda);
        processes[i]->getRunningBurstAvg();
        readyQueue.push_back(processes[i]);
        std::sort(readyQueue.begin(), readyQueue.end(), SJFcomparator);
        std::reverse(readyQueue.begin(), readyQueue.end());

        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << processes[i]->getName();
          std::cout << " (tau " << processes[i]->getTau() << "ms) ";
          std::cout << "arrived; added to ready queue";
          printQueue( readyQueue);
        }
        pAdded++;
      }
    }

    if ( CPUStart == time ) {
      if ( CPUProcess->getTimeRem() != 0 ) {
        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << CPUProcess->getName();
          std::cout << " (tau " << CPUProcess->getTau() << "ms) ";
          std::cout << "started using the CPU for " << CPUTime << "ms burst remaining";
          printQueue( readyQueue);
        }
        CPUProcess->addToBurstAvg(CPUTime);
        CPUProcess->getRunningBurstAvg();
        CPUProcess->setTimeRem(0);

        if (time < 1000) {
          std::cout << "time " << time << "ms: Recalculated tau = " << CPUProcess->getTau();
          std::cout << "ms for process " << CPUProcess->getName();
          printQueue(readyQueue);
        }
      }
      else {

        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << CPUProcess->getName();
          std::cout << " (tau " << CPUProcess->getTau() << "ms) ";
          std::cout << "started using the CPU for " << CPUTime << "ms burst";
          printQueue( readyQueue);
        }
        CPUProcess->addToBurstAvg(CPUTime);
        CPUProcess->getRunningBurstAvg();
      }
      totalBurstTime += CPUTime;
      totalBursts++;
      totalTurnATime += CPUTime + contextSwitch;
      totalTurns++;
      totalCSwitch++;
    }

    if (readyQueue.size() > 0) {
      if ( CPUProcess == NULL ) {
        CPUProcess = readyQueue[0];
        readyQueue.erase(readyQueue.begin());
        if (CPUProcess->getTimeRem() != 0) {
          CPUTime = CPUProcess->getTimeRem();
        }
        else {
          CPUTime = CPUProcess->getCPUTime();
        }
        int cs = 1;
        if (CPUProcess->getIAT() == time)
          cs = 2;

        CPUStart = time + cs;
        CPUProcess->setCPUDone( CPUTime );
        CPUFinished = time + CPUTime + cs;
      }
    }

    if ( time >= CPUFinished && CPUFinished != -1 && CPUProcess != NULL) {
      bool done = true;
      if (waitBlock == 0) {
        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << CPUProcess->getName();
          std::cout << " (tau " << CPUProcess->getTau() << "ms) ";
          std::cout << "completed a CPU burst; " << CPUProcess->getRemBursts();
          std::cout << " bursts to go";
          printQueue(readyQueue);
        }
      }

      if ( CPUProcess != NULL) {
        /*std::cout << "processes[CPUProcess]->getRemBursts(): " << processes[CPUProcess]->getRemBursts() << std::endl;
        std::cout << "done: " << done << std::endl;
        std::cout << "waitblock: " << waitBlock << std::endl;*/
        if (CPUProcess->getRemBursts() != 0 && done && waitBlock == 0) {
          int IOTime = CPUProcess->getIOTime();
          if (time < 1000) {
            std::cout << "time " << time << "ms: Process " << CPUProcess->getName();
            std::cout << " switching out of CPU; will block on I/O until time ";
            std::cout << time + IOTime + contextSwitch / 2 << "ms";
            printQueue( readyQueue);
          }
          waitBlock++;
          CPUProcess->setWaitDone(time + IOTime + contextSwitch / 2);
          if (readyQueue.size() == 0) {
            CPUFinished = -1;
          }

          waitQueue.push_back(CPUProcess);
          CPUProcess = NULL;
          totalWaitTime += IOTime;
          totalWaits++;
          totalCSwitch++;
        }
        else if (CPUProcess->getRemBursts() == 0 && done){
          std::cout << "time " << time << "ms: Process " << CPUProcess->getName();
          std::cout << " terminated";
          printQueue( readyQueue);
          doneP++;
          CPUProcess = NULL;
          if (doneP == processes.size())
            break;
        }
      }
    }
    if ( waitQueue.size() > 0 ) {
      if (time == waitQueue[0]->getWaitDone()) {
        waitBlock = 0;
        bool isPreemption = false;

        if (waitQueue[0]->getCPUTimeNoSped() < CPUFinished - time) {
          isPreemption = true;
        }

        if (!isPreemption) {
          readyQueue.push_back(waitQueue[0]);
          std::sort(readyQueue.begin(), readyQueue.end(), SJFcomparator);
          std::reverse(readyQueue.begin(), readyQueue.end());
          waitQueue.erase(waitQueue.begin());
          if (time < 1000) {
            std::cout << "time " << time << "ms: Process " << waitQueue[0]->getName();
            std::cout << " (tau " << waitQueue[0]->getTau() << "ms) ";
            std::cout << "completed I/O; added to ready queue";
            printQueue(readyQueue);
          }
        }
        else {
          if ( time < 1000 ) {
            std::cout << "time " << time << "ms: Process " << waitQueue[0]->getName();
            std::cout << " (tau " << CPUProcess->getTau() << "ms) ";
            std::cout << "completed I/O; preempting " << CPUProcess->getName();
            printQueue(readyQueue);
          }

          CPUProcess->setTimeRem( CPUFinished - time );
          readyQueue.push_back(CPUProcess);
          readyQueue.push_back(waitQueue[0]);
          std::sort(readyQueue.begin(), readyQueue.end(), SJFcomparator);
          std::reverse(readyQueue.begin(), readyQueue.end());
          waitQueue.erase(waitQueue.begin());
          CPUProcess = NULL;

        }
      }
    }
  }

  std::cout << "time " << time + (contextSwitch / 2) << "ms: Simulator ended for SRT";
  printQueue( readyQueue);

  if (outputFile.is_open()) {
    outputFile << "Algorithm SRT" << std::endl;
    outputFile.setf(std::ios::fixed,std::ios::floatfield);
    outputFile.precision(3);
    outputFile << "-- average CPU burst time: " << totalBurstTime / (double)totalBursts << " ms" << std::endl;
    outputFile << "-- average wait time: " << totalWaitTime / (double)totalWaits << " ms" << std::endl;
    outputFile << "-- average turnaround time: " << totalTurnATime / (double)totalTurns << " ms" << std::endl;
    outputFile << "-- total number of context switches: " << totalCSwitch << std::endl;
    outputFile << "-- total number of preemptions: " << totalPreemptions << std::endl;
  }
}

void RR(std::vector<Process*> &processes, int tSlice, int contextSwitch, std::string rrA, std::ofstream & outputFile) {
  std::vector<int> readyQueue;
  std::vector<int> waitQueue;
  unsigned int doneP = 0;
  int CPUTime = 0;
  int CPUProcess = 99;
  int CPUFinished = -1;
  int pAdded = 0;
  int CPUtimeRem = 0;
  int CPUStart = -1;
  int waitBlock = 0;
  double totalBurstTime = 0;
  int totalBursts = 0;
  double totalWaitTime = 0;
  int totalWaits = 0;
  double totalTurnATime = 0;
  int totalTurns = 0;
  int totalPreemptions = 0;
  int totalCSwitch = 0;

  int time;
  for (time = 0; time > -1;time++) {
    for (unsigned int i = 0;i < processes.size(); i++) {
      if (processes[i]->getIAT() == time) {
        if (rrA[0] == 'B') {
          readyQueue.insert(readyQueue.begin(), i);
        }
        else
          readyQueue.push_back(i);

        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << processes[i]->getName();
          std::cout << " arrived; added to ready queue";
          printQueue(processes, readyQueue);
        }
        pAdded++;
      }
    }

    if ( CPUStart == time ) {
      if ( processes[CPUProcess]->getTimeRem() != 0 ) {
        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << processes[CPUProcess]->getName();
          std::cout << " started using the CPU for " << CPUTime << "ms burst remaining";
          printQueue(processes, readyQueue);
        }
        processes[CPUProcess]->setTimeRem(0);
      }
      else {
        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << processes[CPUProcess]->getName();
          std::cout << " started using the CPU for " << CPUTime << "ms burst";
          printQueue(processes, readyQueue);
        }
      }
      totalBurstTime += CPUTime;
      totalBursts++;
      totalTurnATime += CPUTime + contextSwitch;
      totalTurns++;
      totalCSwitch++;
    }

    if (readyQueue.size() > 0) {
      if ( CPUProcess == 99 ) {
        CPUProcess = readyQueue[0];
        readyQueue.erase(readyQueue.begin());
        if (processes[CPUProcess]->getTimeRem() != 0) {
          CPUTime = processes[CPUProcess]->getTimeRem();
        }
        else {
          CPUTime = processes[CPUProcess]->getCPUTime();
        }
        int cs = 1;
        if (processes[CPUProcess]->getIAT() == time)
          cs = 2;

        CPUStart = time + cs;
        if ( CPUTime < tSlice ) {
          processes[CPUProcess]->setCPUDone( tSlice - CPUTime );
          CPUFinished = time + CPUTime + cs;
        }
        else if (CPUTime == tSlice ) {
          processes[CPUProcess]->setCPUDone( 1 );
          CPUFinished = time + CPUTime + cs;
        }
        else {
          //NEED VARIABLE HERE
          CPUFinished = time + tSlice + cs;
          CPUtimeRem = CPUTime - tSlice;
          processes[CPUProcess]->setCPUDone( 0 );
        }
      }
    }

    if ( time >= CPUFinished && CPUFinished != -1 && CPUProcess != 99) {
      bool done = true;
    //  std::cout << "CPUProcess is : " << CPUProcess << std::endl;
      if (processes[CPUProcess]->getCPUDone() != 0 && waitBlock == 0) {
        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << processes[CPUProcess]->getName();
          std::cout << " completed a CPU burst; " << processes[CPUProcess]->getRemBursts();
          std::cout << " bursts to go";
          printQueue(processes, readyQueue);
        }
      }
      else if (processes[CPUProcess]->getCPUDone() == 0){
        if (readyQueue.size() == 0 ) {
          if (time < 1000) {
            std::cout << "time " <<  time << "ms: Time slice expired; no preemption";
            std::cout << " because ready queue is empty";
            printQueue(processes, readyQueue);
          }
          done = false;
          CPUFinished = time + CPUtimeRem;
          processes[CPUProcess]->setCPUDone(1);
        }
        else {
          processes[CPUProcess]->setTimeRem(CPUtimeRem);
          if (rrA[0] == 'B') {
            readyQueue.insert(readyQueue.begin(), CPUProcess);
          }
          else
            readyQueue.push_back(CPUProcess);

          if (time < 1000) {
            std::cout << "time " <<  time << "ms: Time slice expired; process ";
            std::cout << processes[CPUProcess]->getName() << " preempted with ";
            std::cout << CPUtimeRem << "ms to go";
            printQueue(processes, readyQueue);
          }
          totalPreemptions++;
          CPUProcess = 99;
        }
      }

      if ( CPUProcess != 99) {
        /*std::cout << "processes[CPUProcess]->getRemBursts(): " << processes[CPUProcess]->getRemBursts() << std::endl;
        std::cout << "done: " << done << std::endl;
        std::cout << "waitblock: " << waitBlock << std::endl;*/
        if (processes[CPUProcess]->getRemBursts() != 0 && done && waitBlock == 0) {
          int IOTime = processes[CPUProcess]->getIOTime();
          if (time < 1000) {
            std::cout << "time " << time << "ms: Process " << processes[CPUProcess]->getName();
            std::cout << " switching out of CPU; will block on I/O until time ";
            std::cout << time + IOTime + contextSwitch / 2 << "ms";
            printQueue(processes, readyQueue);
          }
          waitBlock++;
          processes[CPUProcess]->setWaitDone(time + IOTime + contextSwitch / 2);
          if (readyQueue.size() == 0) {
            CPUFinished = -1;
          }

          waitQueue.push_back(CPUProcess);
          CPUProcess = 99;
          totalWaitTime += IOTime;
          totalWaits++;
          totalCSwitch++;
        }
        else if (processes[CPUProcess]->getRemBursts() == 0 && done){
          std::cout << "time " << time << "ms: Process " << processes[CPUProcess]->getName();
          std::cout << " terminated";
          printQueue(processes, readyQueue);
          doneP++;
          CPUProcess = 99;
          if (doneP == processes.size())
            break;
        }
      }
    }
    if ( waitQueue.size() > 0 ) {
      if (time == processes[waitQueue[0]]->getWaitDone()) {
        waitBlock = 0;

        if (rrA[0] == 'B') {
          readyQueue.insert(readyQueue.begin(), waitQueue[0]);
        }
        else
          readyQueue.push_back(waitQueue[0]);
        waitQueue.erase(waitQueue.begin());
        if (time < 1000) {
          std::cout << "time " << time << "ms: Process " << processes[waitQueue[0]]->getName();
          std::cout << " completed I/O; added to ready queue";
          printQueue(processes, readyQueue);
        }
      }
    }
  }
  std::cout << "time " << time + (contextSwitch / 2) << "ms: Simulator ended for RR";
  printQueue(processes, readyQueue);
  if (outputFile.is_open()) {
    outputFile << "Algorithm RR" << std::endl;
    outputFile.setf(std::ios::fixed,std::ios::floatfield);
    outputFile.precision(3);
    outputFile << "-- average CPU burst time: " << totalBurstTime / (double)totalBursts << " ms" << std::endl;
    outputFile << "-- average wait time: " << totalWaitTime / (double)totalWaits << " ms" << std::endl;
    outputFile << "-- average turnaround time: " << totalTurnATime / (double)totalTurns << " ms" << std::endl;
    outputFile << "-- total number of context switches: " << totalCSwitch << std::endl;
    outputFile << "-- total number of preemptions: " << totalPreemptions << std::endl;
  }
}


bool operator<(const Process & lhs, const Process & rhs) {
    return lhs.getIAT() < rhs.getIAT();
}

void orderQueue(int alg, int numP, std::vector<Process*> &processes, float lambda,
      int contextSwitch, int tSlice, std::string rrA, std::ofstream & outputFile) {
  if (alg == 1) {
    for (unsigned int i = 0;i < processes.size();i++) {
      std::cout << "Process " << processes[i]->getName() << " [NEW] (arrival time ";
      std::cout << processes[i]->getIAT() << " ms) " << processes[i]->getBurstNum();
      std::cout << " CPU bursts" << std::endl;
    }
    std::cout << "time 0ms: Simulator started for FCFS [Q <empty>]" << std::endl;

    //Need to sort array by IAT()
    sort(processes.begin(), processes.end(), comparator);
    std::reverse(processes.begin(), processes.end());
    FCFS(processes, contextSwitch, outputFile);
  }
  if (alg == 2) {
    for (unsigned int i = 0;i < processes.size();i++) {
      std::cout << "Process " << processes[i]->getName() << " [NEW] (arrival time ";
      std::cout << processes[i]->getIAT() << " ms) " << processes[i]->getBurstNum();
      std::cout << " CPU bursts (tau " << 1 / lambda << "ms)" << std::endl;
    }
    std::cout << "time 0ms: Simulator started for SJF [Q <empty>]" << std::endl;
    sort(processes.begin(), processes.end(), comparator);
    std::reverse(processes.begin(), processes.end());
    SJF(processes, lambda, contextSwitch, outputFile);
  }
  if (alg == 3) {
    for (unsigned int i = 0;i < processes.size();i++) {
      std::cout << "Process " << processes[i]->getName() << " [NEW] (arrival time ";
      std::cout << processes[i]->getIAT() << " ms) " << processes[i]->getBurstNum();
      std::cout << " CPU bursts (tau " << 1 / lambda << "ms)" << std::endl;
    }
    std::cout << "time 0ms: Simulator started for SRT [Q <empty>]" << std::endl;
    sort(processes.begin(), processes.end(), comparator);
    std::reverse(processes.begin(), processes.end());
    SRT(processes, lambda, contextSwitch, outputFile);
  }
  if (alg == 4) {
    for (unsigned int i = 0;i < processes.size();i++) {
      std::cout << "Process " << processes[i]->getName() << " [NEW] (arrival time ";
      std::cout << processes[i]->getIAT() << " ms) " << processes[i]->getBurstNum();
      std::cout << " CPU bursts (tau " << 1 / lambda << "ms)" << std::endl;
    }
    std::cout << "time 0ms: Simulator started for RR [Q <empty>]" << std::endl;
    sort(processes.begin(), processes.end(), comparator);
    std::reverse(processes.begin(), processes.end());
    RR(processes, tSlice, contextSwitch, rrA, outputFile);
  }
}

int main(int argc, char* argv[]) {
  //Loop to run all scheduling algorithms
  std::ofstream outputFile("simout.txt");
  for (int algo = 1; algo < 5; algo++) {
    int numProcesses = 0;
    double lambda;
    int maxNum;

    //Read and store command line information
    numProcesses = atoi(argv[1]);
    srand48(atoi(argv[2]));
    lambda = atof(argv[3]);
    maxNum = atoi(argv[4]);
    int contextSwitch = atoi(argv[5]);
    int tSlice = atoi(argv[7]);
    std::string rrA = "END";
    if (argv[8] != NULL) rrA = argv[8];
    //Vector of processes
    std::vector<Process*> queue(numProcesses);

    //Empty vector to be arranged by scheduling algorithm
    std::vector<Process> orderedQueue(numProcesses);

    std::vector<std::pair<double,double>* >* times;
    //Creation of processes
    for ( int k = 0; k <numProcesses;k++) {
      //Figures out name
      std::string letter;
      letter = char (65+k);

      //Creates Process object
      Process *temp = new Process(letter);

      //Adds to the vector of processes
      queue[k] = temp;

      //Calculates initial process arrival time
      double r = drand48();
      double y = -log( r ) / lambda;
      if (y > maxNum) {
        y = maxNum;
      }
      queue[k]->setIAT(floor(y));

      //Calculates number of CPU bursts for a process
      double x = trunc((drand48() * 100) + 1);
      queue[k]->setBurstNum(x);

      //Allocates data that is not yet freed
      times = new std::vector<std::pair<double,double>* >;
      double t;
      double t1;
      //Calculates CPU and IO burst times for each CPU bursts
      for (int i = 0; i < x ;i++) {

        //Allocates data that is not yet freed
        std::pair<double, double>* p = new std::pair<double, double>;
        t = -log(drand48()) / lambda;
        p -> first = ceil(t) + 1;

        //Prevents the last pair to have an io time since processes end in CPU
        if (i != x-1) {
          t1 = -log(drand48()) / lambda;
          p -> second = ceil(t1) + 1;
        }

        times->push_back(p);
        //std::cout << (*times)[i].first << std::endl;
      }
      //Sets burst times
      queue[k]->setBurstTimes(times);
    }
    //Orders the queue according to the proper algorithm
    orderQueue(algo, numProcesses, queue, lambda, contextSwitch, tSlice, rrA, outputFile);
    for (int i = 0;i<numProcesses;i++) {
      queue[i]->removeProcess();
      delete queue[i];
    }
  }
  return 0;

}

#include <iostream>
#include "signals.h"
#include "SmallShell.h"
#include <csignal>



using namespace std;

void ctrlCHandler(int sig_num) {
    SmallShell& smash = SmallShell::getInstance();
    cout <<"smash: got ctrl-C"<<endl;
    if(smash.getCurrPid() == -1){
        return;
    }
    if (smash.getCurrLine() == ""){
        return;
    }
    int result;
    DO_SYS(result = kill(smash.getCurrPid() , SIGKILL),kill);
    if(result == -1){
        return;
    }
    cout<< "smash: process " << smash.getCurrPid() << " was killed"<<endl;

  // TODO: Add your implementation
}

void alarmHandler(int sig_num) {
  // TODO: Add your implementation
}


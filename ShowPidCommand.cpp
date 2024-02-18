#include "ShowPidCommand.h"
#include "SmallShell.h"

void ShowPidCommand::execute(){
    SmallShell& shell = SmallShell::getInstance();
    cout << "smash pid is " << shell.getSmashPid() << endl;
}


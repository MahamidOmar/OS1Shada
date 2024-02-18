#ifndef COMMANDS_H_QUITCOMMAND_H
#define COMMANDS_H_QUITCOMMAND_H
#include "JobsList.h"
#include "BuiltInCommand.h"

class QuitCommand : public BuiltInCommand {
// TODO: Add your data members public:
private:
    JobsList* all_jobs;
 public:
    QuitCommand(const char* cmd_line, JobsList* jobs): BuiltInCommand(cmd_line),all_jobs(jobs){}
    virtual ~QuitCommand() {}
    void execute() override;
};

#endif //COMMANDS_H_QUITCOMMAND_H

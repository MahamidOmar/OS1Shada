#include "JobsCommand.h"

void JobsCommand::execute() {
    this->all_jobs->printJobsList();
}
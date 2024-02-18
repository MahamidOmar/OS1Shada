#include "GetCurrDirCommand.h"

void GetCurrDirCommand::execute() {
    char current_directory[PATH_MAX];
    char* syscall_result = getcwd(current_directory, sizeof(current_directory));
    if(syscall_result == NULL){
        perror("smash error: getcwd failed");
        return;
    }
    cout << current_directory << endl;
}
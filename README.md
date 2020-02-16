# shell
A Unix Shell built in C

1. Awaits for a user command and parses it
2. Expects a path_list.txt to include folders that it should look up when searching for the binaries to execute
3. If the command ends with '&', it will create the process to execute and loop back to 1st step awaiting for another user input

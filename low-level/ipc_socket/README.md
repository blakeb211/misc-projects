# IPC Sockets (Unix domain sockets)
## Examples
1. Linux name service
1. `dbus-daemon` message bus for apps to talk to eachother 
1. `docker` connecting to containers
1. GitLab to talk to its daemon managing git repositories
1. X-Server (server-client application for GUIs on linux)

## Facts
- Rely upon the OS kernel on the host computer
- Slower than shared memory, but convenient because less program structure required

## Relevant terminal commands
- `nc -U` command to listen on, or create, a unix socket 
- `lsof` command to list active sockets and some basic information about them

## Overview of Unix socket api calls
|  server  |   client  |
|:--------:|:---------:|
| socket() | socket()  |
| bind()   | connect() |
| listen() |           |
| accept() |           |
| recv()   | send()    |
| send()   | recv()    |
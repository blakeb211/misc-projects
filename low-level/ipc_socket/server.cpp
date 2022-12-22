// Server example using unix sockets
#include "helper_fn.cpp"
#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h> // provides sockaddr_un
#include <unistd.h>
using std::cout, std::endl, std::string;
using std::to_string;

// Constants
constexpr auto SOCK_FILE_PATH = "/tmp/serv42";
constexpr auto BUF_LENGTH = 1000;
constexpr auto RETRIES_MAX = 3; // bind() retry limit

int main() {
  int sd = -1, sd2 = -1;
  int rc, length;
  string buffer(BUF_LENGTH,'\0');
  struct sockaddr_un serveraddr{0,{}};

  {
    auto retries = 0;

  GET_NEW_SOCKET:
    retries += 1;
    // Ask OS for an AF_UNIX socket descriptor from the SOCK_STREAM address
    // family.
    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (check_error(sd, 0, "socket() failed")) {
      return 1;
    }

    // bind() gets a name for the socket. analogy: make a reservation at a
    // restaurant
    serveraddr.sun_family = AF_UNIX;
    strcpy(serveraddr.sun_path, SOCK_FILE_PATH);

    rc = bind(sd, (struct sockaddr *)&serveraddr, SUN_LEN(&serveraddr));

    // Prepare error message in case bind fails.
    {
      string errMsg =  "bind() failed. Retries left: " + to_string(RETRIES_MAX - retries);

      if (check_error(rc, 0, errMsg) && retries < RETRIES_MAX) {
        close(sd);
        unlink(SOCK_FILE_PATH);
        goto GET_NEW_SOCKET;
      }
    }
  }

  // The listen() function allows the server to accept incoming
  // client connections. The second argument is called 'backlog'
  // and it sets the queue length for incoming connections.
  rc = listen(sd, 10);
  check_error(rc, 0, "listen() failed");
  cout << "Ready for client connect()." << endl;

  // accept() waits until a connection request comes in and then returns a
  // socket descriptor. It can be run in a loop and will return a new
  // socket descriptor for each connection that gets made.
  sd2 = accept(sd, NULL, NULL);
  check_error(sd2, 0, "accept() failed");

  // Our client is set to send messages in 1000 byte chunks using the
  // SO_RCVLOWAT variable which stands for "receive low water mark"
  // so that we receive until we have at least that many bytes.
  length = BUF_LENGTH;
  rc =
      setsockopt(sd2, SOL_SOCKET, SO_RCVLOWAT, (char *)&length, sizeof(length));
  check_error(rc, 0, "setsockopt(SO_RCVLOWAT) failed");

  // Receive at least BUF_LEN bytes of data
  rc = recv(sd2, buffer.data(), buffer.size(), 0);
  check_error(rc, 0, "recv() failed");

  print_bytes_receieved(rc,buffer);

  if (rc == 0 || rc < static_cast<int>(sizeof(buffer))) {
    printf("The client closed the connection before all of the\n");
    printf("data was sent\n");
  }

  // use send() to tell client what was received. 
  rc = send(sd2, buffer.data(), buffer.size(), 0);
  check_error(rc, 0, "send() failed");

  // Call close() on open socket descriptors
  close(sd);
  close(sd2);

  // Tell OS it can remove the socket file
  unlink(SOCK_FILE_PATH);
  return 0;
}

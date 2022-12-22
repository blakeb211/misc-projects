// Client example using unix sockets
#include "helper_fn.cpp"
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

constexpr auto SOCK_FILE_PATH = "/tmp/serv42";
constexpr auto BUF_LENGTH = 1000;
using std::string;

int main() {
  int sd = -1, rc;
  // Send 'x' bytes to the server
  string buffer(BUF_LENGTH, 'x');
  struct sockaddr_un serveraddr {
    AF_UNIX, {}
  };

  strcpy(serveraddr.sun_path, SOCK_FILE_PATH);
  // Call to socket() is like opening a door that has no rooms or building
  // connected to it yet.
  sd = socket(AF_UNIX, SOCK_STREAM, 0);
  check_error(sd, 0, "socket() failed.");

  // Connect to server
  rc = connect(sd, (struct sockaddr *)&serveraddr, SUN_LEN(&serveraddr));
  if (check_error(rc, 0, "connect() failed."))
    return 1;

  rc = send(sd, buffer.data(), buffer.size(), 0);
  check_error(rc, 0, "send() failed.");

  // Set the SO_RCVLOWAT so that we receive until we get all BUF_LENGTH bytes.
  // An alternative, if you do not know the number of bytes, is to recv() in
  // a loop, counting the number of bytes received, until a threshold or a
  // certain type of content is received.
  auto length = BUF_LENGTH;
  rc = setsockopt(sd, SOL_SOCKET, SO_RCVLOWAT, (char *)&length, sizeof(length));
  check_error(rc, 0, "setsockopt(SO_RCVLOWAT) failed");

  rc = recv(sd, buffer.data(), buffer.size(), 0);
  check_error(rc, 0, "recv() failed.");
  print_bytes_receieved(rc, buffer);

  // Tell OS it can remove the socket descriptors
  close(sd);
  return 0;
}

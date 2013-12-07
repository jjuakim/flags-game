// Qt includes
#include <qsocket.h>

// local includes
#include "serversocket.h"

////////////////////////////////////////////////////////////////////////////////

ServerSocket::ServerSocket(int port, QObject* parent, const char* name)
  : QServerSocket(port, 1, parent, name)
{
}

////////////////////////////////////////////////////////////////////////////////


ServerSocket::~ServerSocket()
{
  delete m_socket;
}

////////////////////////////////////////////////////////////////////////////////

void ServerSocket::newConnection(int socketfd)
{
  QSocket* socket = new QSocket(parent());
  socket->setSocket(socketfd);
  m_socket = socket;
  emit newClient(socket);
}

// end of file

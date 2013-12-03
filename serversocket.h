#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

// Qt includes
#include <qserversocket.h>

// forward declarations
class QSocket;

class ServerSocket : public QServerSocket
{
  Q_OBJECT

public:
  ServerSocket(int port, QObject* parent=0, const char* name=0);
  ~ServerSocket();

  inline QSocket* socket() { return m_socket;}
  void newConnection(int socketfd);

protected:
  QSocket* m_socket;

signals:
  void newClient(QSocket* socket);
};

#endif

// end of file

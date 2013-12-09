#ifndef SERVERMAINWINDOW_H
#define SERVERMAINWINDOW_H

// Qt includes
#include <qptrdict.h>
#include <qimage.h>
#include <qtimer.h>
#include <qpixmap.h>
#include <qrect.h>

// local includes
#include "servermainwindowbase.h"

// forward declarations
class Client;
class QSocket;
class ServerSocket;

class ServerMainWindow : public MainWindowBase
{
  Q_OBJECT

public:
  void GetRGB24fromRGB565(unsigned char* r, unsigned char* g, unsigned char *b, unsigned char upper, unsigned char lower);
  void setImage();
  ServerMainWindow(QWidget* parent=0, const char* name=0);
  ~ServerMainWindow();
  //unsigned char* recv_buf[320*240*4];
  int recv_total_len;
  int recv_len;
  int recv_flag ;
  QImage _image;
  QImage _myImage;
  QImage _oldImage;
  QCString m_databuffer;
  int m_flag_err;

  int bytesToInt(const char *buffer, int size);
  void intToBytes(int value, char *buffer, int size);

  int timeCount;
  bool isGameStart;
  int lifePoint;
  int gameScore;
  int stage;
  QString command[4];
  int DetectMotion(int sectSum[]);
  void GameInit();

  QTimer *timer; 
  QPixmap img_gameover, img_main, img_ori, img_lt, img_lb, img_rt, img_rb;
  
protected:
  ServerSocket* m_server;

protected:
  void sendToClients(const QString& text);
  virtual void paintEvent(QPaintEvent*);

protected slots:
  void slotStartClicked();
  void slotStopClicked();
  void slotGameClicked();
  
  void slotNewClient(QSocket* socket);
  void slotClientDisconnected();

  void slotSocketRead();
 
  void slotTimeDone();
};

#endif

// end of file

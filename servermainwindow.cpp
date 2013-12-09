// system includes
#include <cstring>
#include <stdlib.h>
// Qt includes
#include <qtextview.h>
#include <qpushbutton.h>
#include <qsocket.h>
#include <qspinbox.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qcstring.h>
#include <qlabel.h>
#include <qpainter.h>
// local includes
#include "servermainwindow.h"
#include "serversocket.h"

#define SIZE_MUL  4
#define IMAGEWIDTH 64*SIZE_MUL //GETWIDTHBYTE(24, 320)//320
#define IMAGEHEIGHT 48*SIZE_MUL

//#define IMAGEWIDTH  192
//#define IMAGEHEIGHT 144
#define DEPTH 4

////////////////////////////////////////////////////////////////////////////////

ServerMainWindow::ServerMainWindow(QWidget* parent, const char* name)
  : MainWindowBase(parent, name),
  m_server(0)
{
  QObject::connect(m_start, SIGNAL(clicked()), this, SLOT(slotStartClicked()));
  QObject::connect(m_stop, SIGNAL(clicked()), this, SLOT(slotStopClicked()));
  QObject::connect(m_btnGame, SIGNAL(clicked()), this, SLOT(slotGameClicked()));
    recv_flag = 0;
  recv_total_len = 0;
  recv_len = 0;
  m_flag_err = 0;
  
  timeCount = 0;
  GameInit();
  stage = rand() % 4;
  command[0] = "White Flag Up";
  command[1] = "White Flag Down";
  command[2] = "Blue Flag Up";
  command[3] = "Blue Flag Down";
  

  m_databuffer.resize( IMAGEWIDTH*IMAGEHEIGHT*2 );
  
  unsigned char *dst;
  
  _image.create(IMAGEWIDTH, IMAGEHEIGHT, DEPTH*8);
 //_myImage.create(IMAGEWIDTH, IMAGEHEIGHT, DEPTH*8);
  _oldImage.create(IMAGEWIDTH, IMAGEHEIGHT, DEPTH*8);
//  _difImage.create(IMAGEWIDTH, IMAGEHEIGHT, DEPTH*8);

  
  dst = _image.bits();

  for (int i = 0 ; i < IMAGEHEIGHT ; i++) {
    for(int j = 0 ; j < IMAGEWIDTH ; j++) {
      int index = i*IMAGEWIDTH*DEPTH;
      dst[index] = 0;
      dst[index+1] = 0;
      dst[index+2] = 0;
    }
  }
  _myImage = _image.copy();
  _oldImage = _image.copy();
 // _difImage = _image.copy();

  
  // game init
  m_edit->setText("Please Connect a Clinet!");
  m_btnGame->setEnabled(false);

  img_main.load("main.bmp");
  img_ori.load("0.bmp");
  img_gameover.load("gameover.bmp");
  img_lt.load("lt.bmp");
  img_lb.load("lb.bmp");
  img_rt.load("rt.bmp");
  img_rb.load("rb.bmp");
}

////////////////////////////////////////////////////////////////////////////////

ServerMainWindow::~ServerMainWindow()
{
  slotStopClicked();
}

////////////////////////////////////////////////////////////////////////////////

void ServerMainWindow::sendToClients(const QString& text)
{
  if (text.isNull()) return;

  // iterate over all clients and send them the text
    QSocket* sock = m_server->socket();
    QTextStream stream(sock);
    stream << text;
}

/////////////////////////////////////////////////////////////////////////////////

void ServerMainWindow::slotStartClicked()
{
  qDebug("slotStartClicked");
  if (m_server != 0) return; // sanity check
  if (timer != 0) return; //  sanity check
  
  qDebug("timer....?"); 
  if(!timer) {
    qDebug("make new Timer");
    timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), SLOT(slotTimeDone()));
  }
 
  qDebug("New Server");
  m_server = new ServerSocket(m_port->value(), this);
  if (!m_server->ok())
  {
    qDebug("server has something wrong!!" );
    delete m_server;
    m_server = 0;
    return;
  }
  
  QObject::connect(m_server, SIGNAL(newClient(QSocket*)), this, SLOT(slotNewClient(QSocket*)));
  m_start->setEnabled(false);
  m_stop->setEnabled(true);

  m_gameStatus->setText("Please Connect a Client!!");
}

////////////////////////////////////////////////////////////////////////////////

void ServerMainWindow::slotStopClicked()
{
  qDebug("slotStopClicked");
  m_start->setEnabled(true);
  m_stop->setEnabled(false);
 
  timeCount = 0;
  m_btnGame->setEnabled(false);
  m_gameStatus->setText("Game Status");

  qDebug("0:delete timer");
  if(timer != 0) {
     delete timer;
     timer = 0; 
  }
  
  // disconnect the socket's signals from any slots or signals
  if(m_server->socket()) {
    qDebug("1: disconnect sokcet");
    m_server->socket()->disconnect();
  }
  qDebug("2. delete server");
  delete m_server;
  m_server = 0;
}

////////////////////////////////////////////////////////////////////////////////

void ServerMainWindow::slotNewClient(QSocket* socket)
{
  qDebug("client connected");
  
  timeCount = 0;
  GameInit();

  m_btnGame->setEnabled(true);
  QPainter p(this);
  p.drawPixmap(QRect(315, 195, 256, 192) , img_main);
  p.end();

  // notify all others about the newcomer
  sendToClients(QString("Server: Client connected\n"));

  QObject::connect(socket, SIGNAL(connectionClosed()), this, SLOT(slotClientDisconnected()));
  QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(slotSocketRead()));

  // great the newcomer
  QTextStream stream(socket);
  stream << "Server: Hi\nYou are client " << endl;
  m_edit->setText("");

  m_gameStatus->setText("Client is connected. Press Game Start Button");
}

////////////////////////////////////////////////////////////////////////////////

void ServerMainWindow::slotTimeDone()
{

  // 이미지 계산부분 
  unsigned char *oldDst;
  unsigned char *newDst;
 
  oldDst = _oldImage.bits();

  _myImage = _image.copy(); 
  newDst = _myImage.bits();

  // 새로운 이미지 흑백 변환
  for (int i = 0; i < IMAGEHEIGHT; i++) {
    for (int j = 0; j < IMAGEWIDTH; j++) {
      int index = i*IMAGEWIDTH*DEPTH + j*DEPTH;
      int mid = (newDst[index] + newDst[index+1] + newDst[index+2]) / 3;           
      newDst[index] = mid;
      newDst[index+1] = mid;
      newDst[index+2] = mid;
    }   
  } 

  //구역별차이구하기
 int sectSum[9] = {0,0,0,0,0,0,0,0,0};
 int oldSum[9] = {0,0,0,0,0,0,0,0,0};
 int newSum[9] = {0,0,0,0,0,0,0,0,0};
  
  for (int i = 0; i < IMAGEHEIGHT; i++) {
    for (int j = 0; j < IMAGEWIDTH; j++) {
       int index = i*IMAGEWIDTH*DEPTH + j*DEPTH;
       if (j < 85 && i < 64){
         oldSum[0] += oldDst[index]; newSum[0] += newDst[index];
       }
       else if (85 <= j && j < 170 && i < 64) {
         oldSum[1] += oldDst[index]; newSum[1] += newDst[index];
       }
       else if (170 <= j && i < 64) {
         oldSum[2] += oldDst[index]; newSum[2] += newDst[index];
       }
       else if (j < 85 && 64 <= i && i < 128) {
         oldSum[3] += oldDst[index]; newSum[3] += newDst[index];
       }
       else if (85<= j && j < 170 && 64 <= i && i < 128) {
         oldSum[4] += oldDst[index]; newSum[4] += newDst[index];
 
       }
       else if (170 <= j && 64 <= i && i < 128) {
         oldSum[5] += oldDst[index]; newSum[5] += newDst[index];
       }
       else if (j < 85 && 128 <= i) {
         oldSum[6] += oldDst[index]; newSum[6] += newDst[index];
       }
       else if (85 <= j && j < 170 && 128 <= i) {
         oldSum[7] += oldDst[index]; newSum[7] += newDst[index];
       }
       else if (170 <= j && 128 <= i) {
         oldSum[8] += oldDst[index]; newSum[8] += newDst[index];
       }
    }
  }
 
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      int x = i * 3 + j;
      sectSum[x] = abs(newSum[x] - oldSum[x]);
      if(sectSum[x] >70000) 
        sectSum[x] = 0;
    }
  }

  _oldImage = _myImage.copy();

  //게임 부분
  QRect rect(315, 195, IMAGEWIDTH, IMAGEHEIGHT);
  QPainter p(this);
  
  if(!isGameStart) {
     if(timeCount != 0) {
      m_gameStatus->setText("<h1>" + QString::number(timeCount) + "</h1>");
      timeCount--;
    }
    else
    {
      m_gameStatus->setText("Start!!!!");       
      isGameStart = true;
      p.drawPixmap(rect, img_ori);
      
      lifePoint = 5;
      gameScore = 0;
      stage = rand() % 4;
      
    }
  }
  else {
    //점수 매기는 부분
    m_gameStatus->setText("<h1><center>" + command[stage] + "</center></h1>");
    if (stage == DetectMotion(sectSum)) {
      gameScore += 10;
      m_gameScore->setText("<h1><center>Score : " + QString::number(gameScore) + "</center></h1>"); 
      m_gameResult->setText("<h1><center><font color=green>O</font></center></h1>");
      stage = rand() % 4;
    }
    else if (DetectMotion(sectSum) == 4 ){
    }
    else {
      m_gameResult->setText("<h1><center><font color=red>X</font></center></h1>");
      if(lifePoint == 0)
      {
        //게임 끝
        p.drawPixmap(rect, img_gameover);   //게임끝 이미지 띄우기
        timer->stop();                      // 타이머 스탑
        m_gameStatus->setText("<h1><font color=red><center>Game Over</center></font></h1>"); //게임satus창에 끝 표시
        m_gameResult->setText("XXX");
        // start버튼 활성화
        m_btnGame->setEnabled(true);
      }
      m_gameLife->setText("<h1><center>life : " + QString::number(lifePoint) + "</center></h1>");
      lifePoint--;
    }
  }
  p.end();
}
////////////////////////////////////////////////////////////////////////////////

int ServerMainWindow::DetectMotion(int sectSum[])
{
  QRect rect(315, 195, IMAGEWIDTH, IMAGEHEIGHT);
  QPainter p(this);
  int result = 4; //nothing;

  if(sectSum[2] == 0 && sectSum[5] == 0)
  {
    //left top
    m_edit->append("left top / blue up ");
    p.drawPixmap(rect, img_lt);
    result = 2;
  }
  else if(sectSum[5] == 0 && sectSum[8] == 0)
  {
   //left down
   m_edit->append("left down / blue down");
    p.drawPixmap(rect, img_lb);
    result = 3;
  }
  else if(sectSum[0] ==0 && sectSum[3] == 0)
  {
    //right top
    m_edit->append("right top / white up");
    p.drawPixmap(rect, img_rt);
    result = 0;
  }
  else if (sectSum[3] == 0 && sectSum[6] == 0)
  {
    //right bottom
    m_edit->append("right botton / whilte down");
    p.drawPixmap(rect, img_rb);
    result = 1;
  } 
  else {
    p.drawPixmap(rect, img_ori);
  }
  p.end();
  
  return result;
}

void ServerMainWindow::GameInit() 
{
  isGameStart = false;
  lifePoint = 5;
  gameScore = 0;

  m_gameScore->setText("<h1><center>Score : 0 </center></h1>");
  m_gameLife->setText("<h1><center>Life : 5</center></h1>");
  m_gameResult->setText("");

}
////////////////////////////////////////////////////////////////////////////////

void ServerMainWindow::slotGameClicked()
{
  qDebug("Button Clicked");
  m_btnGame->setEnabled(false);

  
  timeCount = 5;
  GameInit();
  QPainter p(this);    
  p.drawPixmap(QRect(315, 195, IMAGEWIDTH, IMAGEHEIGHT), img_main); 
  m_gameStatus->setText("<h1>READY.....!!!!!!</h1>");
  

  if(timer != 0) {
    qDebug("Timer is startted");
    timer->start(1000);
  }

}

////////////////////////////////////////////////////////////////////////////////
void ServerMainWindow::slotClientDisconnected()
{
  GameInit();

  QObject* sender = const_cast<QObject*>(QObject::sender());
  QSocket* socket = static_cast<QSocket*>(sender);

  qDebug("client disconnected");

  //stop time 
  qDebug("timer is stopped");
  timer->stop();

  //disconnect signals
  socket->disconnect();

  // remove from dict
  sendToClients(QString("Server: Client  disconnected\n"));

  isGameStart = false;
  m_gameStatus->setText("Clinet is disconnected");
}

////////////////////////////////////////////////////////////////////////////////

void ServerMainWindow::slotSocketRead()
{
  QObject* sender = const_cast<QObject*>(QObject::sender());
  QSocket* socket = static_cast<QSocket*>(sender);

  int over_flag = 0;
 

  //에러 복구 루틴 
  if(m_flag_err) {
  unsigned char temp_buffer[40000];
  while(1) {
      int read_len = socket->bytesAvailable();
    if(!read_len)
      return;
    if(socket->canReadLine()){
      QString temp_str; 
      int idx;
      temp_str = socket->readLine(); 
      if( (idx = temp_str.find("HU") == -1) )
        continue;
      temp_str.remove(idx,0);
      printf(" ## Error Recovery - header read length %d\r\n",temp_str.length());
      if(temp_str.left(2) == "HU"){
        temp_str.remove(0,2);
        bool ok;
        int data_len = temp_str.toInt( &ok, 10 );     
        printf("## Error Recovery - data length : %d\r\n",data_len);
        if(data_len){ 
          recv_total_len = data_len;
          recv_len = 0;
          recv_flag = 1;
          m_flag_err = 0;
          return;
        }
      }

    }
    //cracked data 
    else {
      if(read_len > 40000)
        read_len = 40000;
      int rtn = socket->readBlock( (char*)temp_buffer, read_len );
    }
  }
  }


  // 정상 data reading 루틴
  if(recv_flag) {
  //printf(".");

  int read_len = socket->bytesAvailable();

  // socket buffer over - 잔여 데이터 처리 필요
  if((recv_total_len - recv_len) < read_len){
    printf("over %d \r\n", read_len - (recv_total_len - recv_len));
    read_len = recv_total_len - recv_len;
    over_flag = 1;
  }

  int rtn = socket->readBlock( m_databuffer.data()+recv_len, read_len );
  recv_len += rtn;
  //정상 데이터 reading  -  Image Refresh
  if(recv_len == recv_total_len){
    //printf("Data recv complete\r\n");

    QString str;
    str.setNum( recv_total_len );     
    //m_edit->append(str);
    
    setImage();
    
    recv_total_len = 0;
    recv_len  = 0;
    recv_flag = 0;
  }
  // 데이터 over 리딩 - ERROR 
  else if( recv_len > recv_total_len) {
    printf(" !!! ERROR - data over %d reading \r\n",recv_len - recv_total_len);
    recv_total_len = 0;
    recv_len  = 0;
    recv_flag = 0;
    m_flag_err = 1;
  }
    
  //나머지 데이터 처리 루틴
  if(over_flag){
    while(read_len = socket->bytesAvailable()) {
      printf("%d byte left\r\n", read_len);
      slotSocketRead();
    }
  }
  }

  // 정상 data header reading 루틴
  else {
  //printf(";");
  if (socket->canReadLine()){
    QString line = socket->readLine(); // read the line of text
    //printf("header read length %d\r\n",line.length());
    if(line.left(2) == "HU"){
      line.remove(0,2);
      bool ok;
      int data_len = line.toInt( &ok, 10 );     
      //printf("data length : %d\r\n",data_len);
      if(data_len){ 
        recv_total_len = data_len;
        recv_len = 0;
        recv_flag = 1;
      }
    }
    else {
      printf("read header error\r\n");
      m_flag_err = 1;
    }
  }
  else {
    printf("read header error\r\n");
    m_flag_err = 1;
  }

    // and send it to everone including the sender
    //sendToClients(QString("Client %0: %1").arg(client->number()).arg(line));

    //client->item()->setText(2, line.left(line.length()-1));
    //m_list->update();
  }
 
}


void ServerMainWindow::paintEvent(QPaintEvent* e)
{
  //static QPixmap pixmap;
  QRect rect = e->rect();
  if(!_image.isNull()){
    QPainter p(this);
    p.translate(-rect.x(), -rect.y());
    p.setClipRegion(e->region());
    p.drawImage(15, 195, _image.mirror(true,false));
    p.end();
  }

  QPainter p(this);
  QBrush brush(white, SolidPattern);
  p.setBrush(brush);
  p.drawLine(100,190, 100, 400);
  p.drawLine(185,190, 185, 400);
  p.drawLine(10,259, 300, 259);
  p.drawLine(10,323, 300, 323);
  p.end();
}

// end of file

void ServerMainWindow::setImage()
{
  unsigned char *dst;
  //unsigned char *mydst;
  unsigned char *src;
  src =(unsigned char*)m_databuffer.data();
  dst = _image.bits();
  
  /* 
  //흑백
  mydst = _myImage.bits();

  for (int i = 0 ; i < IMAGEHEIGHT ; i++) {
    for (int j = 0 ; j < IMAGEWIDTH ; j++) {
         int index = i*IMAGEWIDTH*DEPTH + j*DEPTH;
          int mid = (dst[index] + dst[index+1] + dst[index+2]) / 3;     
              mydst[index] = mid;
              mydst[index+1] = mid;
               mydst[index+2] = mid;
     }   
  }   
*/
        
  
  int index;
  for(int i = 0 ; i < IMAGEHEIGHT ; i++){
    for(int j = 0 ; j < IMAGEWIDTH ; j++) {
      index = i*IMAGEWIDTH*DEPTH + j*DEPTH;
      GetRGB24fromRGB565(&dst[index+2] , &dst[index+1], &dst[index],
               src[i*IMAGEWIDTH*2 + j*2+1], src[i*IMAGEWIDTH*2 + j*2]);
    }
  }

  repaint(false);

}

void ServerMainWindow::GetRGB24fromRGB565(unsigned char *r, unsigned char *g, unsigned char *b, unsigned char upper, unsigned char lower)
{
  unsigned short red, green, blue;

  red = upper>>3;
  green = ((upper&0x07)<<3) | ((lower>>5)&0x07);
  blue = lower&0x1f;

  *r = ((unsigned int)red*8);
  *g = ((unsigned int)green*4);
  *b = ((unsigned int)blue*8);
}

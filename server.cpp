// Qt includes
#include <qapplication.h>

// local includes
#include "servermainwindow.h"

int main(int argc, char** args)
{
  QApplication app(argc, args);

  ServerMainWindow* w = new ServerMainWindow();
  app.setMainWidget(w);
  w->show();

  return app.exec();
}

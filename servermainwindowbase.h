/****************************************************************************
** Form interface generated from reading ui file 'servermainwindowbase.ui'
**
** Created: 수 12월 4 01:36:20 2013
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.7   edited Aug 31 2005 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef MAINWINDOWBASE_H
#define MAINWINDOWBASE_H

#include <qvariant.h>
#include <qwidget.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QSpinBox;
class QPushButton;
class QTextEdit;

class MainWindowBase : public QWidget
{
    Q_OBJECT

public:
    MainWindowBase( QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~MainWindowBase();

    QLabel* textLabel2;
    QSpinBox* m_port;
    QPushButton* m_start;
    QPushButton* m_stop;
    QTextEdit* m_edit;

protected:
    QHBoxLayout* layout5;
    QSpacerItem* spacer4;

protected slots:
    virtual void languageChange();

};

#endif // MAINWINDOWBASE_H

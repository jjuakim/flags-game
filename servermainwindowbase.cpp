/****************************************************************************
** Form implementation generated from reading ui file 'servermainwindowbase.ui'
**
** Created: 수 12월 4 02:01:12 2013
**      by: The User Interface Compiler ($Id: qt/main.cpp   3.3.7   edited Aug 31 2005 $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "servermainwindowbase.h"

#include <qvariant.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qtextedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/*
 *  Constructs a MainWindowBase as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 */
MainWindowBase::MainWindowBase( QWidget* parent, const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "MainWindowBase" );

    QWidget* privateLayoutWidget = new QWidget( this, "layout5" );
    privateLayoutWidget->setGeometry( QRect( 10, 10, 277, 40 ) );
    layout5 = new QHBoxLayout( privateLayoutWidget, 11, 6, "layout5"); 

    textLabel2 = new QLabel( privateLayoutWidget, "textLabel2" );
    layout5->addWidget( textLabel2 );

    m_port = new QSpinBox( privateLayoutWidget, "m_port" );
    m_port->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)1, (QSizePolicy::SizeType)1, 0, 0, m_port->sizePolicy().hasHeightForWidth() ) );
    m_port->setMaxValue( 65535 );
    m_port->setMinValue( 1024 );
    m_port->setValue( 2222 );
    layout5->addWidget( m_port );
    spacer4 = new QSpacerItem( 41, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout5->addItem( spacer4 );

    m_start = new QPushButton( privateLayoutWidget, "m_start" );
    layout5->addWidget( m_start );

    m_stop = new QPushButton( privateLayoutWidget, "m_stop" );
    m_stop->setEnabled( FALSE );
    layout5->addWidget( m_stop );

    m_edit = new QTextEdit( this, "m_edit" );
    m_edit->setGeometry( QRect( 10, 50, 331, 140 ) );
    languageChange();
    resize( QSize(595, 464).expandedTo(minimumSizeHint()) );
    clearWState( WState_Polished );
}

/*
 *  Destroys the object and frees any allocated resources
 */
MainWindowBase::~MainWindowBase()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MainWindowBase::languageChange()
{
    setCaption( tr( "Server" ) );
    textLabel2->setText( tr( "Port" ) );
    m_start->setText( tr( "Start" ) );
    m_stop->setText( tr( "Stop" ) );
}


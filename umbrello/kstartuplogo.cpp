/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kstartuplogo.h"
#include <kapp.h>
#include <kstddirs.h>

KStartupLogo::KStartupLogo(QWidget * parent, const char *name)
		: QWidget(parent,name, WStyle_NoBorderEx | WStyle_Customize | WDestructiveClose )
,m_bReadyToHide(false) {
	//pm.load(locate("appdata", "pics/startlogo.png"));
	KStandardDirs * dirs = KGlobal::dirs();
	QString dataDir = dirs -> findResourceDir("data", "umbrello/pics/object.xpm");
	dataDir += "/umbrello/pics/";
	QPixmap pm(dataDir + "startlogo.png");
	setBackgroundPixmap(pm);
	setGeometry(QApplication::desktop()->width()/2-pm.width()/2,
	            QApplication::desktop()->height()/2-pm.height()/2,
	            pm.width(),pm.height());
}

KStartupLogo::~KStartupLogo() {}

void KStartupLogo::mousePressEvent( QMouseEvent*) {
	// for the haters of raising startlogos
	if (m_bReadyToHide)
		hide();
}



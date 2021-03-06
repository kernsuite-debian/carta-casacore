//# Copyright (C) 2005
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
#ifndef BINCOUNTWIDGET_QO_H
#define BINCOUNTWIDGET_QO_H

#include <QWidget>
#include <ui/ui_BinCountWidget.h>

namespace casa {


class BinCountWidget : public QWidget
{
    Q_OBJECT

public:
    BinCountWidget(QWidget *parent = 0);
    void setBinCount( int binCount );
    int getBinCount() const;
    ~BinCountWidget();
    static int DEFAULT_COUNT;
signals:
	void binCountChanged( int binCount );

private slots:
	void binValueChanged( int value );
	void binCountTextChanged( const QString& binCount );

private:
	//Added because custom widgets as menu items were appearing transparent
	//on the MAC.
	void setDefaultBackground();
    Ui::BinCountWidgetClass ui;
};
}

#endif // BINCOUNTWIDGET_QO_H

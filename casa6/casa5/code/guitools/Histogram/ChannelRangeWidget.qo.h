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
#ifndef CHANNELRANGEWIDGET_QO_H
#define CHANNELRANGEWIDGET_QO_H

#include <QWidget>
#include <ui/ui_ChannelRangeWidget.h>

namespace casa {

class ChannelRangeWidget : public QWidget
{
    Q_OBJECT

public:
    ChannelRangeWidget(QWidget *parent = 0);
    void setAutomatic( bool autoChannels );
    void setRange( int minRange, int maxRange );
    void setChannelValue( int value );
    void setChannelCount( int count );
    ~ChannelRangeWidget();

signals:
	void rangeChanged(int minRange, int maxRange, bool allChannels, bool automatic );


private slots:
	void valueChangedMin( int value );
	void valueChangedMax( int value );
	void automaticChannelsChanged( bool enabled );

private:
	bool isAllChannels() const;
	bool isAutomatic() const;

	//Added because custom widgets as menu items were appearing transparent
	//on the MAC.
	void setDefaultBackground();

	int channel;
    Ui::ChannelRangeWidgetClass ui;
};

}

#endif // CHANNELRANGEWIDGET_QO_H

/*
   DrawPile - a collaborative drawing program.

   Copyright (C) 2006-2007 Calle Laakkonen

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "tools.h"
#include "netstate.h"

namespace drawingboard {
	class Board;
	class BoardEditor;
	class Point;
}

namespace interface {
	class BrushSource;
	class ColorSource;
}

namespace network {
	class Connection;
}

class QImage;

//! Controller for drawing and network operations
/**
 * The controller handles all drawing commands coming in from the
 * network or the user. Drawing commands received from the network
 * are committed to the board and user commands are sent to the server.
 *
 * Before finishing their roundtrip from the server, user commands
 * are displayed on a special preview layer. This provides immediate
 * feedback even when the network is congested. Preview strokes are
 * removed as the real ones are received from the server.
 */
class Controller : public QObject
{
	Q_OBJECT
	friend class tools::BrushBase;
	friend class tools::ColorPicker;
	public:
		Controller(QObject *parent=0);
		~Controller();

		void setModel(drawingboard::Board *board);

		//! Connect to host
		void connectHost(const QString& address, const QString& username);

		//! Start hosting a session
		void hostSession(const QString& title, const QString& password,
				const QImage& image);

		//! Join a session
		void joinSession();

		//! Check if connection is still established
		bool isConnected() const { return net_ != 0; }

	public slots:
		//! Join a specific session
		void joinSession(int id);

		//! Send a password
		void sendPassword(const QString& password);

		//! Disconnect from host
		void disconnectHost();

		void penDown(const drawingboard::Point& point, bool isEraser);
		void penMove(const drawingboard::Point& point);
		void penUp();
		void setTool(tools::Type tool);

	signals:
		//! This signal indicates that the drawing board has been changed
		void changed();

		//! Connection with remote host established
		void connected(const QString& address);

		//! Login succesfull
		void loggedin();

		//! Raster data transfer progress
		void rasterProgress(int percent);

		//! Host disconnected
		void disconnected(const QString& message);

		//! Session was joined
		void joined(const QString& title);

		//! Session was left
		void parted();

		//! There were no sessions to join
		void noSessions();

		//! A session should be selected from the list and joined
		void selectSession(const network::SessionList& list);

		//! A password is required
		void needPassword();

	private slots:
		void netConnected();
		void netDisconnected(const QString& message);
		void netError(const QString& message);
		void sessionJoined(int id);
		void sessionParted();
		void rasterDownload(int p);

	private:
		drawingboard::Board *board_;
		tools::Tool *tool_;
		drawingboard::BoardEditor *editor_;
		network::Connection *net_;
		network::HostState *netstate_;
		network::SessionState *session_;
		QString address_;

		QString username_;
};

#endif


/*
   Drawpile - a collaborative drawing program.

   Copyright (C) 2014 Calle Laakkonen

   Drawpile is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   Drawpile is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Drawpile.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tools/floodfill.h"
#include "tools/toolsettings.h"
#include "docks/toolsettingsdock.h"
#include "core/floodfill.h"
#include "scene/canvasscene.h"
#include "net/client.h"

#include <QDebug>

namespace tools {

void FloodFill::begin(const paintcore::Point &point, bool right)
{
	FillSettings *ts = settings().getFillSettings();

	paintcore::FillResult fill = paintcore::floodfill(
		scene().layers(),
		QPoint(point.x(), point.y()),
		right ? settings().backgroundColor() : settings().foregroundColor(),
		ts->fillTolerance(),
		ts->sampleMerged() ? 0 : layer()
	);

	if(fill.image.isNull())
		return;

	// Flood fill is implemented using PutImage rather than a native command.
	// This has the following advantages:
	// - backward and forward compatibility: changes in the algorithm can be made freely
	// - tolerates out-of-sync canvases (shouldn't normally happen, but...)
	// - bugs don't crash/freeze other clients
	//
	// The disadvantage is increased bandwith consumption. However, this is not as bad
	// as one might think: the effective bit-depth of the bitmap is 1bpp and most fills
	// consist of large solid areas, meaning they should compress ridiculously well.
	client().sendUndopoint();
	client().sendImage(layer(), fill.x, fill.y, fill.image, true);
}

void FloodFill::motion(const paintcore::Point &point, bool constrain, bool center)
{
	Q_UNUSED(point);
	Q_UNUSED(constrain);
	Q_UNUSED(center);
}

void FloodFill::end()
{
}

}

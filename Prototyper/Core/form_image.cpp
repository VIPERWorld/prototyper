
/*!
	\file

	\author Igor Mironchik (igor.mironchik at gmail dot com).

	Copyright (c) 2016 Igor Mironchik

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Prototyper include.
#include "form_image.hpp"

// Qt include.
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>


namespace Prototyper {

namespace Core {

//
// FormImagePrivate
//

class FormImagePrivate {
public:
	FormImagePrivate( FormImage * parent )
		:	q( parent )
	{
	}

	//! Init.
	void init();

	//! Parent.
	FormImage * q;
}; // class FormImagePrivate

void
FormImagePrivate::init()
{
}


//
// FormImage
//

FormImage::FormImage( QGraphicsItem * parent )
	:	QGraphicsPixmapItem( parent )
	,	d( new FormImagePrivate( this ) )
{
	d->init();
}

FormImage::~FormImage()
{
}

QRectF
FormImage::boundingRect() const
{
	return QGraphicsPixmapItem::boundingRect();
}

void
FormImage::paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
	QWidget * widget )
{
	QGraphicsPixmapItem::paint( painter, option, widget );
}

void
FormImage::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	QGraphicsPixmapItem::hoverEnterEvent( event );
}

void
FormImage::hoverMoveEvent( QGraphicsSceneHoverEvent * event )
{
	QGraphicsPixmapItem::hoverMoveEvent( event );
}

void
FormImage::mouseMoveEvent( QGraphicsSceneMouseEvent * event )
{
	QGraphicsPixmapItem::mouseMoveEvent( event );
}

void
FormImage::mousePressEvent( QGraphicsSceneMouseEvent * event )
{
	QGraphicsPixmapItem::mousePressEvent( event );
}

void
FormImage::mouseReleaseEvent( QGraphicsSceneMouseEvent * event )
{
	QGraphicsPixmapItem::mouseReleaseEvent( event );
}

} /* namespace Core */

} /* namespace Prototyper */

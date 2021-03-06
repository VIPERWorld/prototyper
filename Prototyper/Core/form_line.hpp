
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

#ifndef PROTOTYPER__CORE__FORM_LINE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_LINE_HPP__INCLUDED

// Qt include.
#include <QGraphicsLineItem>
#include <QScopedPointer>

// Prototyper include.
#include "form_object.hpp"
#include "form_move_handle.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

class Form;
class FormLineMoveHandle;


//
// FormLine
//

class FormLinePrivate;

//! Line on the form.
class FormLine
	:	public QGraphicsLineItem
	,	public FormObject
	,	public FormWithHandle
{
public:
	explicit FormLine( Form * form, QGraphicsItem * parent = 0 );
	~FormLine();

	//! \return Type.
	static ObjectType staticObjectType()
		{ return LineType; }

	//! \return Cfg.
	Cfg::Line cfg() const;
	//! Set cfg.
	void setCfg( const Cfg::Line & c );

	QRectF boundingRect() const Q_DECL_OVERRIDE;

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) Q_DECL_OVERRIDE;

	//! Set pen.
	void setObjectPen( const QPen & p,
		bool pushUndoCommand = true ) Q_DECL_OVERRIDE;

	//! Show handles.
	void showHandles( bool show = true );

	//! \return Point in the middle of handle if
	//! given point contained by handle.
	QPointF pointUnderHandle( const QPointF & point,
		bool & intersected, bool & intersectedEnds ) const;

	//! Handle mouse move in handles.
	bool handleMouseMoveInHandles( const QPointF & point );

	//! Position elements.
	void setPosition( const QPointF & pos,
		bool pushUndoCommand = true ) Q_DECL_OVERRIDE;

	//! \return Position of the element.
	QPointF position() const Q_DECL_OVERRIDE;

	//! \return Rectangle of the element.
	QRectF rectangle() const Q_DECL_OVERRIDE;

	//! Set rectangle.
	void setRectangle( const QRectF & rect,
		bool pushUndoCommand = true ) Q_DECL_OVERRIDE;

protected:
	//! Handle moved.
	void handleMoved( const QPointF & delta, FormMoveHandle * handle )
		Q_DECL_OVERRIDE;
	//! Handle released.
	void handleReleased( FormMoveHandle * handle ) Q_DECL_OVERRIDE;

protected:
	void mouseMoveEvent( QGraphicsSceneMouseEvent * event )
		Q_DECL_OVERRIDE;
	void mousePressEvent( QGraphicsSceneMouseEvent * event )
		Q_DECL_OVERRIDE;
	void mouseReleaseEvent( QGraphicsSceneMouseEvent * event )
		Q_DECL_OVERRIDE;

protected:
	friend class UndoChangeLine;

	//! Place handles.
	void placeHandles();

private:
	Q_DISABLE_COPY( FormLine )

	QScopedPointer< FormLinePrivate > d;
}; // class FormLine

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_LINE_HPP__INCLUDED

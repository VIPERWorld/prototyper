
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

#ifndef PROTOTYPER__CORE__FORM_POLYLINE_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_POLYLINE_HPP__INCLUDED

// Qt include.
#include <QGraphicsPathItem>
#include <QScopedPointer>

// Prototyper include.
#include "form_object.hpp"
#include "form_move_handle.hpp"
#include "project_cfg.hpp"


namespace Prototyper {

namespace Core {

class Form;


//
// FormPolyline
//

class FormPolylinePrivate;

//! Polyline on the form.
class FormPolyline
	:	public QGraphicsPathItem
	,	public FormObject
	,	public FormWithHandle
{
public:
	explicit FormPolyline( Form * form, QGraphicsItem * parent = 0 );
	~FormPolyline();

	//! \return Type.
	static ObjectType staticObjectType()
		{ return PolylineType; }

	//! \return Cfg.
	Cfg::Polyline cfg() const;
	//! Set cfg.
	void setCfg( const Cfg::Polyline & c );

	//! Draw polyline.
	static void draw( QPainter * painter, const Cfg::Polyline & cfg );

	//! \return Lines.
	const QList< QLineF > & lines() const;
	//! Set lines.
	void setLines( const QList< QLineF > & lns );
	//! Append line.
	void appendLine( const QLineF & line );
	//! Remove last line.
	void removeLine( const QLineF & line );
	//! \return Count of lines.
	int countOfLines() const;

	//! Show/hide handles.
	void showHandles( bool show = true );

	//! \return Is closed?
	bool isClosed() const;

	//! \return Point under handle if so.
	QPointF pointUnderHandle( const QPointF & p, bool & intersected ) const;

	//! Set pen.
	void setObjectPen( const QPen & p,
		bool pushUndoCommand = true ) Q_DECL_OVERRIDE;

	//! Set brush.
	void setObjectBrush( const QBrush & b ) Q_DECL_OVERRIDE;

	QRectF boundingRect() const Q_DECL_OVERRIDE;

	//! Handle mouse move in handles.
	void handleMouseMoveInHandles( const QPointF & p );

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) Q_DECL_OVERRIDE;

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

private:
	Q_DISABLE_COPY( FormPolyline )

	QScopedPointer< FormPolylinePrivate > d;
}; // class FormPolyline

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_POLYLINE_HPP__INCLUDED

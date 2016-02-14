
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

#ifndef PROTOTYPER__CORE__FORM_TEXT_HPP__INCLUDED
#define PROTOTYPER__CORE__FORM_TEXT_HPP__INCLUDED

// Qt include.
#include <QGraphicsTextItem>
#include <QScopedPointer>

// Prototyper include.
#include "form_object.hpp"
#include "form_resizable.hpp"


namespace Prototyper {

namespace Core {

class Form;


//
// FormText
//

class FormTextPrivate;

//! Text on the form.
class FormText
	:	public QGraphicsTextItem
	,	public FormObject
	,	public FormResizable
{
public:
	FormText( const QRectF & rect, Form * form, QGraphicsItem * parent = 0 );
	~FormText();

	//! Enable/disable editing.
	void enableEditing( bool on = true );

	//! Clear selection.
	void clearSelection();

	void paint( QPainter * painter, const QStyleOptionGraphicsItem * option,
		QWidget * widget = 0 ) Q_DECL_OVERRIDE;

protected:
	//! Resize.
	void resize( const QRectF & rect ) Q_DECL_OVERRIDE;
	//! Move resizable.
	void moveResizable( const QPointF & delta ) Q_DECL_OVERRIDE;

protected:
	void focusOutEvent( QFocusEvent * event ) Q_DECL_OVERRIDE;

private:
	Q_DISABLE_COPY( FormText )

	QScopedPointer< FormTextPrivate > d;
}; // class FormText

} /* namespace Core */

} /* namespace Prototyper */

#endif // PROTOTYPER__CORE__FORM_TEXT_HPP__INCLUDED

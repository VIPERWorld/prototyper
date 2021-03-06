
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
#include "form_undo_commands.hpp"

// Qt include.
#include <QTextDocument>


namespace Prototyper {

namespace Core {

//
// UndoGroup
//

UndoGroup::UndoGroup( Form * form, const QString & id )
	:	QUndoCommand( QObject::tr( "Group" ) )
	,	m_form( form )
	,	m_id( id )
	,	m_undone( false )
{
}

UndoGroup::~UndoGroup()
{
}

void
UndoGroup::undo()
{
	m_undone = true;

	TopGui::instance()->projectWindow()->switchToSelectMode();

	FormGroup * group = dynamic_cast< FormGroup* > ( m_form->findItem( m_id ) );

	QList< QGraphicsItem* > items = group->children();

	m_items.clear();

	foreach( QGraphicsItem * item, items )
		m_items.append( dynamic_cast< FormObject* > ( item )->objectId() );

	m_form->ungroup( group, false );
}

void
UndoGroup::redo()
{
	if( m_undone )
	{
		TopGui::instance()->projectWindow()->switchToSelectMode();

		QList< QGraphicsItem* > items;

		foreach( const QString & id, m_items )
		{
			QGraphicsItem * item = m_form->findItem( id );

			if( item )
				items.append( item );
		}

		FormGroup * group = m_form->group( items, false );

		m_form->renameObject( group, m_id, false );
	}
}


//
// UndoUngroup
//

UndoUngroup::UndoUngroup( const QStringList & items,
	const QString & groupId, Form * form )
	:	QUndoCommand( QObject::tr( "Ungroup" ) )
	,	m_items( items )
	,	m_id( groupId )
	,	m_form( form )
	,	m_undone( false )
{
}

UndoUngroup::~UndoUngroup()
{
}

void
UndoUngroup::undo()
{
	m_undone = true;

	TopGui::instance()->projectWindow()->switchToSelectMode();

	QList< QGraphicsItem* > items;

	foreach( const QString & id, m_items )
		items.append( m_form->findItem( id ) );

	FormGroup * group = m_form->group( items, false );

	m_form->renameObject( group, m_id, false );
}

void
UndoUngroup::redo()
{
	if( m_undone )
	{
		TopGui::instance()->projectWindow()->switchToSelectMode();

		FormGroup * group = dynamic_cast< FormGroup* > ( m_form->findItem( m_id ) );

		m_form->ungroup( group, false );
	}
}


//
// UndoAddLineToPoly
//

UndoAddLineToPoly::UndoAddLineToPoly( Form * form,
	const QString & id, const QLineF & line )
	:	QUndoCommand( QObject::tr( "Add Line" ) )
	,	m_line( line )
	,	m_form( form )
	,	m_id( id )
	,	m_undone( false )
{
}

UndoAddLineToPoly::~UndoAddLineToPoly()
{
}

void
UndoAddLineToPoly::undo()
{
	m_undone = true;

	TopGui::instance()->projectWindow()->switchToPolylineMode();

	FormPolyline * poly = dynamic_cast< FormPolyline* > (
		m_form->findItem( m_id ) );

	poly->removeLine( m_line );

	if( poly->countOfLines() == 1 )
	{
		const QLineF line = poly->lines().first();

		m_form->deleteItems( QList< QGraphicsItem* > () << poly, false );

		FormLine * lineItem = dynamic_cast< FormLine* > (
			m_form->createElement< FormLine > ( m_id ) );

		lineItem->setLine( line );

		lineItem->showHandles( true );

		m_form->setCurrentLine( lineItem );
	}
	else
	{
		m_form->setCurrentPolyLine( poly );

		poly->showHandles( true );
	}
}

void
UndoAddLineToPoly::redo()
{
	if( m_undone )
	{
		TopGui::instance()->projectWindow()->switchToPolylineMode();

		FormLine * lineItem = dynamic_cast< FormLine* > (
			m_form->findItem( m_id ) );

		if( lineItem )
		{
			const QLineF line = lineItem->line();

			m_form->deleteItems( QList< QGraphicsItem* > () << lineItem,
				false );

			FormPolyline * poly = dynamic_cast< FormPolyline* > (
				m_form->createElement< FormPolyline > ( m_id ) );

			poly->appendLine( line );

			poly->appendLine( m_line );

			poly->showHandles( true );

			m_form->setCurrentPolyLine( poly );
		}
		else
		{
			FormPolyline * poly = dynamic_cast< FormPolyline* > (
				m_form->findItem( m_id ) );

			poly->appendLine( m_line );

			if( poly->isClosed() )
				poly->showHandles( false );
			else
				poly->showHandles( true );
		}
	}
}


//
// UndoRenameObject
//

UndoRenameObject::UndoRenameObject( Form * form,
	const QString & oldName, const QString & newName )
	:	QUndoCommand( QObject::tr( "Rename Object" ) )
	,	m_form( form )
	,	m_oldName( oldName )
	,	m_newName( newName )
	,	m_undone( false )
{
}

UndoRenameObject::~UndoRenameObject()
{
}

void
UndoRenameObject::undo()
{
	m_undone = true;

	FormObject * obj = dynamic_cast< FormObject* > (
		m_form->findItem( m_newName ) );

	m_form->renameObject( obj, m_oldName, false );

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoRenameObject::redo()
{
	if( m_undone )
	{
		FormObject * obj = dynamic_cast< FormObject* > (
			m_form->findItem( m_oldName ) );

		m_form->renameObject( obj, m_newName, false );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}


//
// UndoChangeLine
//

UndoChangeLine::UndoChangeLine( Form * form, const QString & id,
	const QLineF & oldLine, const QLineF & newLine )
	:	QUndoCommand( QObject::tr( "Change Line" ) )
	,	m_form( form )
	,	m_id( id )
	,	m_oldLine( oldLine )
	,	m_newLine( newLine )
	,	m_undone( false )
{
}

UndoChangeLine::~UndoChangeLine()
{
}

void
UndoChangeLine::undo()
{
	m_undone = true;

	FormLine * line = dynamic_cast< FormLine* > ( m_form->findItem( m_id ) );

	line->setLine( m_oldLine );

	line->placeHandles();

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoChangeLine::redo()
{
	if( m_undone )
	{
		FormLine * line = dynamic_cast< FormLine* > ( m_form->findItem( m_id ) );

		line->setLine( m_newLine );

		line->placeHandles();

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}


//
// UndoChangePen
//

UndoChangePen::UndoChangePen( Form * form, const QString & id,
	const QPen & oldPen, const QPen & newPen )
	:	QUndoCommand( QObject::tr( "Change Pen" ) )
	,	m_form( form )
	,	m_id( id )
	,	m_oldPen( oldPen )
	,	m_newPen( newPen )
	,	m_undone( false )
{
}

UndoChangePen::~UndoChangePen()
{
}

void
UndoChangePen::undo()
{
	m_undone = true;

	FormObject * obj = dynamic_cast< FormObject* > ( m_form->findItem( m_id ) );

	if( obj )
		obj->setObjectPen( m_oldPen, false );

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoChangePen::redo()
{
	if( m_undone )
	{
		FormObject * obj = dynamic_cast< FormObject* > (
			m_form->findItem( m_id ) );

		if( obj )
			obj->setObjectPen( m_newPen, false );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}


//
// UndoChangeTextOnForm
//

UndoChangeTextOnForm::UndoChangeTextOnForm( Form * form, const QString & id )
	:	QUndoCommand( QObject::tr( "Change Text" ) )
	,	m_form( form )
	,	m_id( id )
	,	m_undone( false )
	,	m_doc( 0 )
{
}

UndoChangeTextOnForm::~UndoChangeTextOnForm()
{
	if( !m_doc.isNull() )
		m_doc->deleteLater();
}

void
UndoChangeTextOnForm::undo()
{
	m_undone = true;

	FormText * text = dynamic_cast< FormText* > (
		m_form->findItem( m_id ) );

	if( text )
	{
		m_doc = text->document();

		m_doc->setParent( Q_NULLPTR );

		text->document()->undo();
	}

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoChangeTextOnForm::redo()
{
	if( m_undone )
	{
		FormText * text = dynamic_cast< FormText* > (
			m_form->findItem( m_id ) );

		if( text )
		{
			if( !m_doc.isNull() && m_doc.data() != text->document() )
			{
				m_form->removeDocFromMap( text->document() );

				m_form->updateDocItemInMap( m_doc.data(), text );

				text->setDocument( m_doc.data() );
			}

			text->document()->redo();
		}

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}


//
// UndoChangeTextWithOpts
//

UndoChangeTextWithOpts::UndoChangeTextWithOpts( Form * form, const QString & id,
	const Cfg::TextStyle & oldOpts, const Cfg::TextStyle & newOpts )
	:	QUndoCommand( QObject::tr( "Change Text Options" ) )
	,	m_form( form )
	,	m_id( id )
	,	m_oldOpts( oldOpts )
	,	m_newOpts( newOpts )
	,	m_undone( false )
{
}

UndoChangeTextWithOpts::~UndoChangeTextWithOpts()
{
}

void
UndoChangeTextWithOpts::undo()
{
	m_undone = true;

	setTextOpts( m_oldOpts );

	TopGui::instance()->projectWindow()->switchToSelectMode();
}

void
UndoChangeTextWithOpts::redo()
{
	if( m_undone )
	{
		setTextOpts( m_newOpts );

		TopGui::instance()->projectWindow()->switchToSelectMode();
	}
}

void
UndoChangeTextWithOpts::setTextOpts( const Cfg::TextStyle & opts )
{
	FormObject * obj = dynamic_cast< FormObject* > ( m_form->findItem( m_id ) );

	if( obj )
	{
		switch( obj->objectType() )
		{
			case FormObject::ButtonType :
			{
				FormButton * e = dynamic_cast< FormButton* > ( obj );

				if( e )
					e->setText( opts );
			}
				break;

			case FormObject::CheckBoxType :
			case FormObject::RadioButtonType :
			{
				FormCheckBox * e = dynamic_cast< FormCheckBox* > ( obj );

				if( e )
					e->setText( opts );
			}
				break;

			case FormObject::SpinBoxType :
			{
				FormSpinBox * e = dynamic_cast< FormSpinBox* > ( obj );

				if( e )
					e->setText( opts );
			}
				break;

			default :
				break;
		}
	}
}

} /* namespace Core */

} /* namespace Prototyper */

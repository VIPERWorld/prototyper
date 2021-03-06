
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
#include "pdf_exporter.hpp"
#include "utils.hpp"

// Qt include.
#include <QPdfWriter>
#include <QPageLayout>
#include <QTextDocument>
#include <QList>
#include <QTemporaryFile>
#include <QSharedPointer>
#include <QSvgGenerator>
#include <QSvgRenderer>
#include <QTextCursor>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QPainter>

// C++ include.
#include <algorithm>


namespace Prototyper {

namespace Core {

static const int c_pageBreakType = QTextFormat::UserFormat + 1;

//
// PdfExporterPrivate
//

class PdfExporterPrivate
	:	public ExporterPrivate
{
public:
	PdfExporterPrivate( const Cfg::Project & cfg, PdfExporter * parent )
		:	ExporterPrivate( cfg, parent )
	{
	}

	//! Create tmp images.
	void createImages();
	//! Fill document.
	void fillDocument( QTextDocument & doc );
	//! Print document.
	void printDocument( const QTextDocument & doc, QPdfWriter & pdf,
		const QRectF & body );

	//! Images' files.
	QList< QSharedPointer< QTemporaryFile > > m_images;
}; // class PdfExporterPrivate

void
PdfExporterPrivate::createImages()
{
	foreach( const Cfg::Form & form, m_cfg.form() )
	{
		m_images.append( QSharedPointer< QTemporaryFile >
			( new QTemporaryFile ) );

		m_images.last()->open();

		QSvgGenerator svg;
		svg.setFileName( m_images.last()->fileName() );
		svg.setResolution( 72 );

		drawForm( svg, form );

		m_images.last()->close();
	}
}

void
PdfExporterPrivate::fillDocument( QTextDocument & doc )
{
	Cfg::fillTextDocument( &doc, m_cfg.description().text() );

	QTextCursor c( &doc );

	int i = 0;

	foreach( const Cfg::Form & form, m_cfg.form() )
	{
		QMap< QString, QString > lnks = links( form );

		c.movePosition( QTextCursor::End );

		c.insertText( QString( "\n" ) );

		c.movePosition( QTextCursor::End );

		QTextCharFormat fmt;
		fmt.setObjectType( c_pageBreakType );

		c.insertText( QString( QChar::ObjectReplacementCharacter ) +
			QString( "\n" ), fmt );

		c.movePosition( QTextCursor::End );

		QTextCharFormat header;
		header.setFontWeight( QFont::Bold );
		header.setFontPointSize( 20.0 );

		c.setCharFormat( header );

		c.insertText( form.tabName() + QLatin1String( "\n\n" ) );

		c.movePosition( QTextCursor::End );

		QTextImageFormat image;

		image.setName( m_images.at( i )->fileName() );

		++i;

		c.insertImage( image );

		c.movePosition( QTextCursor::End );

		c.insertText( QLatin1String( "\n" ) );

		c.movePosition( QTextCursor::End );

		auto formIt = std::find_if( form.desc().constBegin(),
			form.desc().constEnd(),
			[&form] ( const Cfg::Description & desc ) -> bool
				{
					return ( form.tabName() == desc.id() );
				}
		);

		if( formIt != form.desc().constEnd() && !formIt->text().isEmpty() )
		{
			c.insertText( QLatin1String( "\n\n" ) );

			c.movePosition( QTextCursor::End );

			Cfg::fillTextDocument( &doc, formIt->text() );

			c.movePosition( QTextCursor::End );

			c.insertText( QLatin1String( "\n\n" ) );

			c.movePosition( QTextCursor::End );

			if( lnks.contains( formIt->id() ) )
			{
				QTextCharFormat fmt;
				fmt.setFontItalic( true );
				fmt.setFontUnderline( true );
				fmt.setFontPointSize( 8.0 );

				c.insertText( QString( "Linked to: " ) +
					lnks[ formIt->id() ] + QLatin1String( "\n\n" ), fmt );

				lnks.remove( formIt->id() );

				c.movePosition( QTextCursor::End );
			}
		}

		for( auto it = form.desc().constBegin(), last = form.desc().constEnd();
			it != last; ++it )
		{
			if( it != formIt && !it->text().isEmpty() )
			{
				QTextCharFormat h2;
				h2.setFontWeight( QFont::Bold );
				h2.setFontItalic( true );
				h2.setFontPointSize( 15.0 );

				c.setCharFormat( h2 );

				c.insertText( it->id() + QLatin1String( "\n\n" ) );

				c.movePosition( QTextCursor::End );

				Cfg::fillTextDocument( &doc, it->text() );

				c.movePosition( QTextCursor::End );

				c.insertText( QLatin1String( "\n\n" ) );

				c.movePosition( QTextCursor::End );

				if( lnks.contains( it->id() ) )
				{
					QTextCharFormat fmt;
					fmt.setFontItalic( true );
					fmt.setFontUnderline( true );
					fmt.setFontPointSize( 8.0 );

					c.insertText( QString( "Linked to: " ) +
						lnks[ it->id() ] + QLatin1String( "\n\n" ), fmt );

					lnks.remove( it->id() );

					c.movePosition( QTextCursor::End );
				}
			}
		}

		if( !lnks.isEmpty() )
		{
			QTextCharFormat fmt;
			fmt.setFontWeight( QFont::Bold );
			fmt.setFontItalic( true );
			fmt.setFontPointSize( 15.0 );

			c.insertText( QLatin1String( "Links:\n\n" ), fmt );

			fmt.setFontWeight( QFont::Normal );
			fmt.setFontUnderline( true );
			fmt.setFontPointSize( 8.0 );

			QMapIterator< QString, QString > it( lnks );

			while( it.hasNext() )
			{
				it.next();

				c.insertText( it.key() + QLatin1String( " -> " ) +
					it.value() + QLatin1String( "\n" ), fmt );
			}
		}
	}
}

void
PdfExporterPrivate::printDocument( const QTextDocument & doc, QPdfWriter & pdf,
	const QRectF & body )
{
	QTextBlock block = doc.begin();

	QPainter p;
	p.begin( &pdf );

	qreal y = 0.0;

	while( block.isValid() )
	{
		QTextBlock::Iterator it = block.begin();

		QTextImageFormat imageFormat;

		bool isObject = false;
		bool isImage = false;
		bool isBreak = false;

		for( ; !it.atEnd(); ++it )
		{
			const QString txt = it.fragment().text();
			isObject = txt.contains(
				QChar::ObjectReplacementCharacter );
			isImage = isObject &&
				it.fragment().charFormat().isImageFormat();
			isBreak = isObject &&
				( it.fragment().charFormat().objectType() == c_pageBreakType );

			if( isImage )
				imageFormat = it.fragment().charFormat().toImageFormat();
		}

		if( isBreak )
		{
			pdf.newPage();

			y = 0.0;
		}
		else if( isImage )
		{
			QSvgRenderer svg( imageFormat.name() );
			QSize s = svg.viewBox().size();

			if( s.width() > body.size().width() ||
				s.height() > body.size().height() )
					s.scale( QSize( body.size().width(),
						body.size().height() ), Qt::KeepAspectRatio );

			if( ( y + s.height() ) > body.height() )
			{
				pdf.newPage();

				y = 0.0;
			}

			p.save();

			p.translate( ( body.size().width() - s.width() ) / 2, y );

			svg.render( &p, QRectF( 0, 0, s.width(), s.height() ) );

			y += s.height();

			p.restore();
		}
		else
		{
			const QRectF r = block.layout()->boundingRect();

			block.layout()->setPosition( QPointF( 0.0, 0.0 ) );

			if( ( y + r.height() ) > body.height() )
			{
				pdf.newPage();

				y = 0.0;
			}

			block.layout()->draw( &p, QPointF( 0.0, y ) );

			y += r.height();
		}

		block = block.next();
	}

	p.end();
}


//
// PdfExporter
//

PdfExporter::PdfExporter( const Cfg::Project & project )
	:	Exporter( QScopedPointer< ExporterPrivate >
			( new PdfExporterPrivate( project, this ) ) )
{
}

PdfExporter::~PdfExporter()
{
}

void
PdfExporter::exportToDoc( const QString & fileName )
{
	PdfExporterPrivate * d = d_ptr();

	d->m_images.clear();

	QPdfWriter pdf( fileName );

	pdf.setResolution( 150 );

	QPageLayout layout = pdf.pageLayout();
	layout.setUnits( QPageLayout::Point );
	const qreal margin = ( 2.0 / 2.54 ) * 72;
	layout.setMargins( QMarginsF( margin, margin,
		margin, margin ) );
	pdf.setPageLayout( layout );

	const QRectF body( 0, 0, pdf.width(), pdf.height() );

	d->createImages();

	QTextDocument doc;
	doc.setPageSize( body.size() );

	d->fillDocument( doc );

	doc.documentLayout()->setPaintDevice( &pdf );

	doc.setPageSize( body.size() );

	d->printDocument( doc, pdf, body );
}

} /* namespace Core */

} /* namespace Prototyper */

/*	
	This file is part of Ingnomia https://github.com/rschurade/Ingnomia
    Copyright (C) 2017-2020  Ralph Schurade, Ingnomia Team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#include "ProxyGameView.h"

#include "../aggregatortileinfo.h"
#include "../aggregatorneighbors.h"
#include "../aggregatorpopulation.h"
#include "../aggregatorcreatureinfo.h"
#include "../aggregatormilitary.h"
#include "../aggregatorinventory.h"
#include "../eventconnector.h"

#include "../../game/gamemanager.h"

#include "ViewModel.h"

#include <QDebug>
#include <QPainter>

ProxyGameView::ProxyGameView( QObject* parent ) :
	QObject( parent )
{
	connect( &EventConnector::getInstance(), &EventConnector::signalKeyEsc, this, &ProxyGameView::onKeyEscape, Qt::QueuedConnection );
	connect( this, &ProxyGameView::signalPropagateEscape, &EventConnector::getInstance(), &EventConnector::onPropagateEscape );

	connect( &EventConnector::getInstance(), &EventConnector::signalTimeAndDate, this, &ProxyGameView::onTimeAndDate, Qt::QueuedConnection );
	connect( &EventConnector::getInstance(), &EventConnector::signalKingdomInfo, this, &ProxyGameView::onKingdomInfo, Qt::QueuedConnection );
	connect( &EventConnector::getInstance(), &EventConnector::signalViewLevel, this, &ProxyGameView::onViewLevel, Qt::QueuedConnection );

	connect( &EventConnector::getInstance(), &EventConnector::signalUpdatePause, this, &ProxyGameView::onUpdatePause, Qt::QueuedConnection );
	connect( &EventConnector::getInstance(), &EventConnector::signalUpdateGameSpeed, this, &ProxyGameView::onUpdateGameSpeed, Qt::QueuedConnection );

	connect( &EventConnector::getInstance(), &EventConnector::signalBuild, this, &ProxyGameView::onBuild, Qt::QueuedConnection );
	connect( EventConnector::getInstance().aggregatorTileInfo(), &AggregatorTileInfo::signalShowTileInfo, this, &ProxyGameView::onShowTileInfo, Qt::QueuedConnection );

	connect( EventConnector::getInstance().aggregatorStockpile(), &AggregatorStockpile::signalOpenStockpileWindow, this, &ProxyGameView::onOpenStockpileWindow, Qt::QueuedConnection );
	connect( EventConnector::getInstance().aggregatorWorkshop(), &AggregatorWorkshop::signalOpenWorkshopWindow, this, &ProxyGameView::onOpenWorkshopWindow, Qt::QueuedConnection );
	//connect( EventConnector::getInstance().aggregatorAgri(), &AggregatorAgri::signalShowAgri, this, &ProxyGameView::onOpenAgriWindow, Qt::QueuedConnection );

	connect( this, &ProxyGameView::signalCloseStockpileWindow, EventConnector::getInstance().aggregatorStockpile(), &AggregatorStockpile::onCloseWindow, Qt::QueuedConnection );
	connect( this, &ProxyGameView::signalCloseWorkshopWindow, EventConnector::getInstance().aggregatorWorkshop(), &AggregatorWorkshop::onCloseWindow, Qt::QueuedConnection );
	connect( this, &ProxyGameView::signalCloseAgricultureWindow, EventConnector::getInstance().aggregatorAgri(), &AggregatorAgri::onCloseWindow, Qt::QueuedConnection );

	connect( this, &ProxyGameView::signalRequestPopulationUpdate, EventConnector::getInstance().aggregatorPopulation(), &AggregatorPopulation::onRequestPopulationUpdate, Qt::QueuedConnection );
	connect( this, &ProxyGameView::signalRequestCreatureUpdate, EventConnector::getInstance().aggregatorCreatureInfo(), &AggregatorCreatureInfo::onRequestCreatureUpdate, Qt::QueuedConnection );
	connect( this, &ProxyGameView::signalRequestNeighborsUpdate, EventConnector::getInstance().aggregatorNeighbors(), &AggregatorNeighbors::onRequestNeighborsUpdate, Qt::QueuedConnection );
	connect( this, &ProxyGameView::signalRequestMissionsUpdate, EventConnector::getInstance().aggregatorNeighbors(), &AggregatorNeighbors::onRequestMissions, Qt::QueuedConnection );

	connect( this, &ProxyGameView::signalRequestMilitaryUpdate, EventConnector::getInstance().aggregatorMilitary(), &AggregatorMilitary::onRequestMilitary, Qt::QueuedConnection );
	connect( this, &ProxyGameView::signalRequestInventoryUpdate, EventConnector::getInstance().aggregatorInventory(), &AggregatorInventory::onRequestCategories, Qt::QueuedConnection );

	connect( &Global::em(), &EventManager::signalEvent, this, &ProxyGameView::onEvent, Qt::QueuedConnection );
	connect( this, &ProxyGameView::signalEventAnswer, &Global::em(), &EventManager::onAnswer, Qt::QueuedConnection );

	connect( this, &ProxyGameView::signalSetPaused, &GameManager::getInstance(), &GameManager::setPaused, Qt::QueuedConnection );
	connect( this, &ProxyGameView::signalSetGameSpeed, &GameManager::getInstance(), &GameManager::setGameSpeed, Qt::QueuedConnection );

	connect( this, &ProxyGameView::signalSetRenderOptions, &EventConnector::getInstance(), &EventConnector::onSetRenderOptions, Qt::QueuedConnection );
	connect( &EventConnector::getInstance(), &EventConnector::signalUpdateRenderOptions, this, &ProxyGameView::onUpdateRenderOptions, Qt::QueuedConnection );
}

ProxyGameView::~ProxyGameView()
{
}

void ProxyGameView::setParent( IngnomiaGUI::GameModel* parent )
{
	m_parent = parent;
}

void ProxyGameView::onTimeAndDate( int minute, int hour, int day, QString season, int year, QString sunStatus )
{
	if ( m_parent )
	{
		m_parent->setTimeAndDate( minute, hour, day, season, year, sunStatus );
	}
}

void ProxyGameView::onKingdomInfo( QString name, QString info1, QString info2, QString info3 )
{
	if( m_parent )
	{
		m_parent->updateKingdomInfo( name, info1, info2, info3 );
	}
}

void ProxyGameView::onViewLevel( int level )
{
	if ( m_parent )
	{
		m_parent->setViewLevel( level );
	}
}

void ProxyGameView::onUpdatePause( bool value )
{
	if ( m_parent )
	{
		m_parent->updatePause( value );
	}
}

void ProxyGameView::onUpdateGameSpeed( GameSpeed speed )
{
	if ( m_parent )
	{
		m_parent->updateGameSpeed( speed );
	}
}

void ProxyGameView::onBuild()
{
	if ( m_parent )
	{
		m_parent->onBuild();
	}
}

void ProxyGameView::onShowTileInfo( unsigned int tileID )
{
	if ( m_parent )
	{
		m_parent->onShowTileInfo( tileID );
	}
}

void ProxyGameView::onOpenStockpileWindow( unsigned int stockpileID )
{
	if ( m_parent )
	{
		m_parent->onShowStockpileInfo( stockpileID );
	}
}

void ProxyGameView::onOpenWorkshopWindow( unsigned int workshopID )
{
	if ( m_parent )
	{
		m_parent->onShowWorkshopInfo( workshopID );
	}
}

void ProxyGameView::onOpenAgriWindow( unsigned int ID )
{
	if ( m_parent )
	{
		m_parent->onShowAgriculture( ID );
	}
}

void ProxyGameView::closeStockpileWindow()
{
	emit signalCloseStockpileWindow();
}

void ProxyGameView::closeWorkshopWindow()
{
	emit signalCloseWorkshopWindow();
}

void ProxyGameView::closeAgricultureWindow()
{
	emit signalCloseAgricultureWindow();
}

void ProxyGameView::requestPopulationUpdate()
{
	emit signalRequestPopulationUpdate();
}

void ProxyGameView::requestNeighborsUpdate()
{
	emit signalRequestNeighborsUpdate();
}

void ProxyGameView::requestMissionsUpdate()
{
	emit signalRequestMissionsUpdate();
}

void ProxyGameView::requestCreatureUpdate( unsigned int id )
{
	emit signalRequestCreatureUpdate( id );
}

void ProxyGameView::onEvent( unsigned int id, QString title, QString msg, bool pause, bool yesno )
{
	if( m_parent )
	{
		m_parent->eventMessage( id, title, msg, pause, yesno );
	}
}

void ProxyGameView::sendEventAnswer( unsigned int eventID, bool answer )
{
	emit signalEventAnswer( eventID, answer );
}

void ProxyGameView::requestMilitaryUpdate()
{
	emit signalRequestMilitaryUpdate();
}

void ProxyGameView::requestInventoryUpdate()
{
	emit signalRequestInventoryUpdate();
}


void ProxyGameView::onKeyEscape()
{
	if( m_parent )
	{
		m_parent->OnCmdBack( nullptr );
	}
}
	
void ProxyGameView::propagateEscape()
{
	emit signalPropagateEscape();
}

void ProxyGameView::setGameSpeed( GameSpeed speed )
{
	emit signalSetGameSpeed( speed );
}
	
void ProxyGameView::setPaused( bool paused )
{
	emit signalSetPaused( paused );
}

void ProxyGameView::setRenderOptions( bool designations, bool jobs, bool walls, bool axles )
{
	emit signalSetRenderOptions( designations, jobs, walls, axles );
}

void ProxyGameView::onUpdateRenderOptions( bool designation, bool jobs, bool walls, bool axles )
{
	if( m_parent )
	{
		m_parent->updateRenderOptions( designation, jobs, walls, axles );
	}
}
/*
 * Ubitrack - Library for Ubiquitous Tracking
 * Copyright 2006, Technische Universitaet Muenchen, and individual
 * contributors as indicated by the @authors tag. See the
 * copyright.txt in the distribution for a full listing of individual
 * contributors.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 */

/**
 * @ingroup api
 * @file
 * Implements the advanced interface to the ubitrack dataflow network.
 *
 * @author Daniel Pustka <daniel.pustka@in.tum.de>
 */

#include <fstream>
#include <sstream>
#include <log4cpp/Category.hh>

#include <utUtil/Exception.h>
#include <utDataflow/EventQueue.h>
#include <utGraph/UTQLReader.h>
#include <utGraph/DataflowGeneration.h>

#include "AdvancedFacade.h"
#include "DataflowObserver.h"

#include <utDataflow/ComponentFactory.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <utClientServer/TcpConnection.h>
#include <utDataflow/DataflowNetwork.h>
#include <utFacade/Config.h>
#ifdef _WIN32
	#include <utUtil/CleanWindows.h>
#endif


// get a logger
static log4cpp::Category& logger( log4cpp::Category::getInstance( "Ubitrack.Facade.AdvancedFacade" ) );

static const char* g_defaultPort = "3000";


namespace Ubitrack { namespace Facade {

AdvancedFacade::AdvancedFacade( const std::string& sComponentPath )
	: m_bStarted( false )
	, m_pIoService( new boost::asio::io_service )
{
	if ( !sComponentPath.empty() )
		m_pComponentFactory.reset( new Dataflow::ComponentFactory( sComponentPath ) );
	else
	{
#ifdef _WIN32
		try
		{
			// try "ubitrack"-directory relative to ubitrack.dll
			HMODULE hModule = GetModuleHandle( "Ubitrack.dll" );
			if ( !hModule )
				UBITRACK_THROW( "No ubitrack.dll loaded" );
			char modulePath[ 256 ];
			DWORD pathLen = GetModuleFileName( hModule, modulePath, 256 );
			// just strip the ".dll" from the name
			std::string compPath( modulePath, pathLen - 4 );
			m_pComponentFactory.reset( new Dataflow::ComponentFactory( compPath ) );
		}
		catch( Util::Exception& e )
		{
			LOG4CPP_INFO( logger, e )
		}
#endif

		// try compile-time prefix
		if ( !m_pComponentFactory )
			m_pComponentFactory.reset( new Dataflow::ComponentFactory( UBITRACK_COMPONENTS_PATH ) );
	}

}


AdvancedFacade::~AdvancedFacade()
{
	LOG4CPP_DEBUG( logger, "~AdvancedFacade" );

	// kill server connection
	if ( m_pNetworkThread )
	{
		LOG4CPP_DEBUG( logger, "Stopping network thread" );
		m_pIoService->stop();
		m_pNetworkThread->join();
	}

	m_pTcpConnection.reset();
	
	// kill data flow network
	LOG4CPP_DEBUG( logger, "Removing data flow network" );
	
	m_pDataflowNetwork.reset();
	//Dataflow::EventQueue::singleton().clear();

	
	LOG4CPP_DEBUG( logger, "AdvancedFacade destroyed" );
}


void AdvancedFacade::loadDataflow( const std::string& sDfUtql, bool bReplace )
{
	LOG4CPP_NOTICE( logger, "AdvancedFacade::loadDataflow from file " << sDfUtql );

	// load srg from file
	std::ifstream input( sDfUtql.c_str() );
	if ( !input.good() )
		UBITRACK_THROW( "Unable to open file " + sDfUtql );
	
	loadDataflow( input, bReplace );
}


void AdvancedFacade::loadDataflow( std::istream& stream, bool bReplace )
{
	LOG4CPP_DEBUG( logger, "AdvancedFacade::loadDataflow from stream" );

	// load srg from stream
	boost::shared_ptr< Graph::UTQLDocument > doc;
	doc = Graph::UTQLReader::processInput( stream );

	// check if the document is a UTQLQuery and run pattern matching in this case
	if ( doc->isRequest() )
		doc = Graph::generateDataflow( *doc );

	// notify observers of deletions
	for ( ObserverList::iterator itObserver = m_observers.begin(); itObserver != m_observers.end(); itObserver++ )
		for ( Graph::UTQLDocument::SubgraphList::iterator it = doc->m_Subgraphs.begin(); it != doc->m_Subgraphs.end(); it++ )
			if ( (*it)->empty() )
			{
				try
				{ 
					LOG4CPP_TRACE( logger, "notifying observer of deletion of " << (*it)->m_Name << "/" << (*it)->m_ID );
					(*itObserver)->notifyDeleteComponent( (*it)->m_Name, (*it)->m_ID ); 
				}
				catch ( const Util::Exception& e )
				{ LOG4CPP_WARN( logger, "Caught exception calling observer: " << e ); }
				catch ( ... )
				{ LOG4CPP_WARN( logger, "Caught exception calling observer" ); }
			}
	
	if ( bReplace || !m_pDataflowNetwork )
	{
		m_pDataflowNetwork.reset();

		// create a new data flow network
		boost::shared_ptr< Dataflow::DataflowNetwork > pDfn( new Dataflow::DataflowNetwork( *m_pComponentFactory ) );

		// instantiate the df network from UTQL
		pDfn->processUTQLResponse( doc );

		// finally, copy to global pointer (here for exception safety)
		m_pDataflowNetwork = pDfn;
		
		if ( m_bStarted )
			startDataflow();
	}
	else
		m_pDataflowNetwork->processUTQLResponse( doc );

	// notify observers of additions
	for ( ObserverList::iterator itObserver = m_observers.begin(); itObserver != m_observers.end(); itObserver++ )
		for ( Graph::UTQLDocument::SubgraphList::iterator it = doc->m_Subgraphs.begin(); it != doc->m_Subgraphs.end(); it++ )
			if ( !(*it)->empty() )
			{
				try
				{ 
					LOG4CPP_TRACE( logger, "notifying observer of addition of " << (*it)->m_Name << "/" << (*it)->m_ID );
					(*itObserver)->notifyAddComponent( (*it)->m_Name, (*it)->m_ID, **it ); 
				}
				catch ( const Util::Exception& e )
				{ LOG4CPP_WARN( logger, "Caught exception calling observer: " << e ); }
				catch ( ... )
				{ LOG4CPP_WARN( logger, "Caught exception calling observer" ); }
			}
}


void AdvancedFacade::clearDataflow()
{
	if ( m_bStarted )
		stopDataflow();
		
	m_pDataflowNetwork.reset();
}


void AdvancedFacade::startDataflow()
{
	LOG4CPP_DEBUG( logger, "AdvancedFacade::startDataflow" );

	// start the event queue
	if ( m_pDataflowNetwork )
	{
		Dataflow::EventQueue::singleton().clear(); // FIXME
		m_pDataflowNetwork->startNetwork();
		Dataflow::EventQueue::singleton().start();
	}
	m_bStarted = true;
}


void AdvancedFacade::stopDataflow()
{
	LOG4CPP_DEBUG( logger, "AdvancedFacade::stopDataflow" );
		
	// stop the event queue
	if ( m_pDataflowNetwork )
	{
		Dataflow::EventQueue::singleton().stop();
		m_pDataflowNetwork->stopNetwork();
		Dataflow::EventQueue::singleton().clear(); // FIXME
	}
	m_bStarted = false;
}


void AdvancedFacade::connectToServer( const std::string& sAddress )
{
	LOG4CPP_DEBUG( logger, "AdvancedFacade::connectToServer " << sAddress );

	using boost::asio::ip::tcp;
	
	// split address into host and port
	std::string sHost( sAddress );
	std::string sPort( g_defaultPort );

	std::string::size_type iSplit = sAddress.find( ':', 0 );
	if ( iSplit != std::string::npos )
	{
		sHost = sAddress.substr( 0, iSplit );
		sPort = sAddress.substr( iSplit + 1, std::string::npos );
	}

	// resolve host name
	tcp::resolver resolver( *m_pIoService );
	tcp::resolver::query query ( sHost, sPort );
	tcp::resolver::iterator endpoint_iterator = resolver.resolve( query );
	tcp::resolver::iterator end;

	// connect to host
	boost::shared_ptr< tcp::socket > pSocket( new tcp::socket( *m_pIoService ) );
	boost::system::error_code error = boost::asio::error::host_not_found;
	while ( error && endpoint_iterator != end )
	{
		pSocket->close();
		pSocket->connect( *endpoint_iterator++, error );
	}
	
	if ( error )
	{
		UBITRACK_THROW( "Cannot connect to server \"" + sHost + "\" at port \"" + sPort + "\"" );
	}

	LOG4CPP_INFO( logger, "Connected to server " << sAddress );

	// create & initialize Ubitrack TcpConnection object
	m_pTcpConnection.reset( new ClientServer::TcpConnection( pSocket ) );
	m_pTcpConnection->setReceiver( boost::bind( &AdvancedFacade::receiveUtqlResponse, this, _1 ) );

	// start network thread
	if ( !m_pNetworkThread )
		m_pNetworkThread.reset( new boost::thread( boost::bind( &boost::asio::io_service::run, m_pIoService.get() ) ) );
}


void AdvancedFacade::sendUtqlToServer( const std::string& sUtqlFile )
{
	LOG4CPP_INFO( logger, "About to send file " << sUtqlFile << " to server" );

	// load Utql from file
	std::ifstream input( sUtqlFile.c_str() );
	if ( !input.good() )
		UBITRACK_THROW( "Unable to open file " + sUtqlFile );
	
	sendUtqlToServer( input );
}


void AdvancedFacade::sendUtqlToServer( std::istream& stream )
{
	LOG4CPP_DEBUG( logger, "sending stream to server" );

	// copy stream content to a string...
	std::string buffer;
	while ( stream.good() && !stream.eof() )
	{
		std::string tmp;
		std::getline( stream, tmp );
		buffer += tmp;
		buffer += "\n";
	}

	sendUtqlToServerString( buffer );
}


void AdvancedFacade::sendUtqlToServerString( const std::string& buffer )
{
	LOG4CPP_DEBUG( logger, "sending string to server" );
	LOG4CPP_TRACE( logger, "sending:\n" << buffer );
	
	// check connection
	if ( !m_pTcpConnection || m_pTcpConnection->badConnection() )
		UBITRACK_THROW( "no connection to server" );
	
	// send to server
	m_pTcpConnection->send( buffer );
}


void AdvancedFacade::receiveUtqlResponse( boost::shared_ptr< Ubitrack::ClientServer::ClientServerConnection::BufferType > pBuffer )
{
	LOG4CPP_INFO( logger, "Received reconfiguration from server" );

	// copy buffer to string
	std::string result( pBuffer->begin(), pBuffer->end() );
	LOG4CPP_DEBUG( logger, "received:\n" << result );
	std::istringstream input( result );

	bool bPrevStarted = m_bStarted;
	if ( bPrevStarted )
		stopDataflow();
	
	loadDataflow( input, false );
	
	if ( bPrevStarted )
		startDataflow();
}

void AdvancedFacade::killEverything(){
	Dataflow::EventQueue::destroyEventQueue();
}


} } // namespace Ubitrack::Facade

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
 * Defines an advanced interface to the ubitrack dataflow network.
 *
 * @author Daniel Pustka <daniel.pustka@in.tum.de>
 */
#ifndef __UBITRACK_FACADE_ADVANCEDFACADE_H_INCLUDED__
#define __UBITRACK_FACADE_ADVANCEDFACADE_H_INCLUDED__

#include <utFacade/utFacade.h>
#include <istream>
#include <list>
#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>
#include <utClientServer/ClientServerConnection.h>
#include <utDataflow/DataflowNetwork.h>

#include "../utComponents/ApplicationPushSink.h"
#include "../utComponents/ApplicationPushSource.h"

// lots of forward decls to avoid applications having to deal with these internals
namespace Ubitrack { 
	namespace Dataflow {
		class ComponentFactory;
	}
	namespace ClientServer {
		class TcpConnection;
	}
}

namespace boost 
{
	namespace asio {
		class io_service;
	}
}


namespace Ubitrack { namespace Facade {

// forward decls
class DataflowObserver;


/**
 * Provides an interface to the ubitrack library with more powerful functions than 
 * the simple interface at the price of full boost exposure.
 */
class UTFACADE_EXPORT AdvancedFacade
{
public:
	/**
	 * Initializes the Ubitrack library.
	 *
	 * @param sComponentPath Path to component directory. Uses default directory if none is specified
	 */
	AdvancedFacade( const std::string& sComponentPath = std::string() );

	/**
	 * Destroys the dataflow network and removes all ubitrack resources
	 */
	virtual ~AdvancedFacade();
	
	
	/** 
	 * Loads and instantiates a dataflow network from a UTQL XML file
	 *
	 * @param sDfUtql filename of dataflow description
	 * @param bReplace if true, replaces the existing data flow network with the new one. 
	 *   Otherwise, the new file is considered as an update.
	 */
	void loadDataflow( const std::string& sDfUtql, bool bReplace = true );

	/** 
	 * Loads and instantiates a dataflow network from a stream containing a UTQL XML file
	 *
	 * @param sDfUtql stream of dataflow description
	 * @param bReplace if true, replaces the existing data flow network with the new one. 
	 *   Otherwise, the new file is considered as an update.
	 */
	void loadDataflow( std::istream& stream, bool bReplace = true );	
	
	/** removes all dataflow component instances */
	void clearDataflow();
	
	
	/** starts components and the event queue */
	void startDataflow();
	
	/** stops components and the event queue */
	void stopDataflow();

	
	/** 
	 * connect to a ubitrack server.
	 * @param sAddress format: <hostname> [":" <port>]
	 */
	void connectToServer( const std::string& sAddress );

	/**
	 * sends the contents of a file to a connected ubitrack server.
	 */
	void sendUtqlToServer( const std::string& sUtqlFile );
	
	/**
	 * sends the contents of a stream to a connected ubitrack server.
	 */
	void sendUtqlToServer( std::istream& stream );

	/**
	 * sends a string to a connected ubitrack server.
	 */
	void sendUtqlToServerString( const std::string& buffer );
	
	
	/**
	 * Returns a pointer to a component.
	 * Throws an exception if not found.
	 *
	 * @param ComponentClass class of the component to find
	 * @param sComponentName name of the component to find
	 * @return \c shared_ptr to component
	 */
	template< class ComponentClass >
	boost::shared_ptr< ComponentClass > componentByName( const std::string& sComponentName )
	{ return m_pDataflowNetwork->componentByName< ComponentClass >( sComponentName ); }
	
	/**
	 * Sets a callback on an \c ApplicationPushSink.
	 * Throws an exception if not found.
	 *
	 * @param EventType type of events to send
	 * @param sComponentName name of the \c ApplicationPushSink component on which to set the callback
	 * @param callback \c boost::function to call when an event is received
	 */
	template< class EventType >
	void setCallback( const std::string& sComponentName, boost::function< void( const EventType& ) > callback )
	{ componentByName< Components::ApplicationPushSink< EventType > >( sComponentName )->setCallback( callback ); }

	
	/**
	 * Adds a data flow observer to the observer list
	 *
	 * @param pObserver pointer to DataflowObserver
	 */
	void addDataflowObserver( DataflowObserver* pObserver )
	{ m_observers.push_back( pObserver ); }
	
	/**
	 * Removes a data flow observer from the observer list
	 *
	 * @param pObserver pointer to DataflowObserver
	 */
	void removeDataflowObserver( DataflowObserver* pObserver )
	{ m_observers.remove( pObserver ); }
	
	void killEverything();
protected:
	/** a component factory */
	boost::scoped_ptr< Dataflow::ComponentFactory > m_pComponentFactory;
	
	/** pointer to a dataflow network */
	boost::shared_ptr< Dataflow::DataflowNetwork > m_pDataflowNetwork;
	
	/** has the data flow network been started? */
	bool m_bStarted;
	
	/** Boost ASIO IO service for client-server communication */
	boost::scoped_ptr< boost::asio::io_service > m_pIoService;
	
	/** ubitrack TCP connection object for communication with the server */
	boost::shared_ptr< ClientServer::TcpConnection > m_pTcpConnection;
	
	/** thread for the network */
	boost::shared_ptr< boost::thread > m_pNetworkThread;

	/** handles a response from the ubitrack server */
	void receiveUtqlResponse( boost::shared_ptr< Ubitrack::ClientServer::ClientServerConnection::BufferType > pBuffer );

	/** typedef for data flow observer list */
	typedef std::list< DataflowObserver* > ObserverList;
	
	/** list of data flow observers */
	ObserverList m_observers;
};


 } } // namespace Ubitrack::Facade

#endif

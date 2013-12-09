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
 * @ingroup dataflow_components
 * @file
 * Sink component with push input port
 * This files contains an sink component with which the application
 * can interface which uses an PushConsumer input port and calls
 * an application registered callback.
 *
 * @author Florian Echtler <echtler@in.tum.de>
 */
#ifndef __UBITRACK_COMPONENTS_APPLICATIONPUSHSINK_H_INCLUDED__
#define __UBITRACK_COMPONENTS_APPLICATIONPUSHSINK_H_INCLUDED__

// does not require applications to know log4cpp then
#define APPLICATIONPUSHSINK_NOLOGGING

#include <string>
#include <iostream>

#include <boost/bind.hpp>

#include <utDataflow/PushConsumer.h>
#include <utDataflow/Component.h>
#include <utMeasurement/Measurement.h>
#include <utUtil/SimpleStringOArchive.h>
#include <utFacade/SimpleDatatypes.h>
#ifndef APPLICATIONPUSHSINK_NOLOGGING
#include <log4cpp/Category.hh>
#endif

// forward decls
namespace Ubitrack {
	namespace Graph {
		class UTQLSubgraph;
	}
}


namespace Ubitrack { namespace Components {

using namespace Dataflow;

/**
 * Common base class for application push sinks.
 * Allows setting string receivers.
 */
class ApplicationPushSinkBase
{
public:
	/** sets a SimplePoseReceiver */
	virtual void setStringCallback( Facade::SimpleStringReceiver* pReceiver ) = 0;

	/** virtual destructor. always good to have one. */
	virtual ~ApplicationPushSinkBase()
	{}
};


/**
 * @ingroup dataflow_components
 * This is an sink component which may be used to interface
 * the dataflow network to an user application.
 *
 * This sink uses a push input port and calls a callback
 * function supplied by the application upon reception of
 * a new event.
 *
 * @par Input Ports
 * PushConsumer<EventType> port with name "Input".
 *
 * @par Output Ports
 * None.
 *
 * @par Configuration
 * None.
 *
 * @par Operation
 * Whenever the dataflow network pushes a new event to the
 * PushConsumer of the ApplicationPushSink the class calls
 * the callback the user application has registered via
 * setCallback.
 *
 */
template <class EventType>
class ApplicationPushSink
	: public Component
	, public ApplicationPushSinkBase
{
public:
	/**
	 * UTQL component constructor.
	 *
	 * @param sName Unique name of the component.
	 * @param subgraph UTQL subgraph
	 */
	ApplicationPushSink( const std::string& nm, boost::shared_ptr< Graph::UTQLSubgraph >  )
		: Ubitrack::Dataflow::Component( nm )
		, m_InPort( "Input", *this, boost::bind( &ApplicationPushSink::pushHandler, this, _1 ) )
		, m_callback( 0 )
#ifndef APPLICATIONPUSHSINK_NOLOGGING
		, m_logger( log4cpp::Category::getInstance( "Ubitrack.Components.ApplicationPushSink" ) )
#endif
	{
	}

	/**
	 * Set the callback.
	 * Set the callback in the user application which will be called for
	 * incoming events.
	 * @param slot callback function of the user application.
	 */
	void setCallback ( typename PushConsumer< EventType >::SlotType slot )
	{
		m_callback = slot;
	}

	/** sets a string receiver */
	void setStringCallback( Facade::SimpleStringReceiver* pReceiver )
	{
		m_callback = boost::bind( &ApplicationPushSink< EventType >::sendString, _1, pReceiver );
	}

protected:
	/**
	 * Handler method for push consumer
	 * This is the handler method for the input port.
	 * Pushes any incoming event to the application if callback is
	 * already registered.
	 * @param m the received event.
	 * @todo calls user code. wrap with try{} block..
	 */
	void pushHandler( const EventType& m )
	{
#ifndef APPLICATIONPUSHSINK_NOLOGGING
		LOG4CPP_DEBUG( m_logger, getName() << " received event" );
#endif

		if( m_callback )
			m_callback( m );
#ifndef APPLICATIONPUSHSINK_NOLOGGING
		else
			LOG4CPP_INFO( m_logger, "ApplicationPushSink " << getName() << " has no consumer connected" );
#endif
	}

	/** converts events to string */
	static void sendString( const EventType& m, Facade::SimpleStringReceiver* pReceiver )
	{
		Util::SimpleStringOArchive ar;
		ar << m;
		std::string s( ar.str() );
		pReceiver->receiveString( s.c_str() );
	}
	
	/** Input port of the function. */
	PushConsumer< EventType > m_InPort;

	/** Type of the callback functions */
	typename PushConsumer< EventType >::SlotType m_callback;

#ifndef APPLICATIONPUSHSINK_NOLOGGING
	/** reference to logger */
	log4cpp::Category& m_logger;
#endif
};


typedef ApplicationPushSink< Measurement::Pose > ApplicationPushSinkPose;
typedef ApplicationPushSink< Measurement::ErrorPose > ApplicationPushSinkErrorPose;
typedef ApplicationPushSink< Measurement::Position > ApplicationPushSinkPosition;
typedef ApplicationPushSink< Measurement::Position2D > ApplicationPushSinkPosition2D;
typedef ApplicationPushSink< Measurement::Rotation > ApplicationPushSinkRotation;
typedef ApplicationPushSink< Measurement::PositionList > ApplicationPushSinkPositionList;
typedef ApplicationPushSink< Measurement::PositionList2 > ApplicationPushSinkPositionList2;
typedef ApplicationPushSink< Measurement::Matrix4x4 > ApplicationPushSinkMatrix4x4;
typedef ApplicationPushSink< Measurement::ErrorPositionList > ApplicationPushSinkErrorPositionList;
typedef ApplicationPushSink< Measurement::ErrorPositionList2 > ApplicationPushSinkErrorPositionList2;
} } // namespace Ubitrack::Components
#endif //__UBITRACK_COMPONENTS_APPLICATIONPUSHSINK_H_INCLUDED__


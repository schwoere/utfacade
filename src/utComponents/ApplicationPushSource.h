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
 * Source component with push output port
 * This files contains a source component with which the application
 * can interface which uses an PushSupplier output port
 *
 * @author Daniel Pustka <daniel.pustka@in.tum.de>
 */
#ifndef __UBITRACK_COMPONENTS_APPLICATIONPUSHSOURCE_H_INCLUDED__
#define __UBITRACK_COMPONENTS_APPLICATIONPUSHSOURCE_H_INCLUDED__

#include <string>
#include <iostream>

#include <boost/bind.hpp>

#include <utDataflow/PushSupplier.h>
#include <utDataflow/Component.h>
#include <utMeasurement/Measurement.h>
#include <utFacade/SimpleDatatypes.h>
#include <utUtil/SimpleStringIArchive.h>

#include <log4cpp/Category.hh>

// forward decls
namespace Ubitrack {
	namespace Graph {
		class UTQLSubgraph;
	}
}

namespace Ubitrack { namespace Components {

using namespace Dataflow;

/**
 * @ingroup dataflow_components
 * This is an source component which may be used to interface
 * the dataflow network to an user application.
 *
 * This source uses a push output port and provides a callback
 * function to the application that can be used to send
 * events into the dataflow network.
 *
 * @par Input Ports
 * None.
 *
 * @par Output Ports
 * PushSupplier<EventType> port with name "Output".
 *
 * @par Configuration
 * None.
 */
template< class EventType >
class ApplicationPushSource 
	: public Component
	, public Facade::SimpleStringReceiver
{
public:
	// type of callback
	typedef boost::function< void( const EventType& ) > CallbackType;

	/**
	 * Standard component constructor.
	 *
	 * @param nm Unique name of the component.
	 * @param cfg ComponentConfiguration containing all configuration.
	 */
	ApplicationPushSource( const std::string& nm, boost::shared_ptr< Graph::UTQLSubgraph >  )
		: Ubitrack::Dataflow::Component( nm )
		, m_outPort( "Output", *this )
	{
	}

	/**
	 * Sends an event.
	 * @param evt the event to send
	 */
	void send( const EventType& evt )
	{ m_outPort.send( evt ); }
	
	/**
	 * Get the callback.
	 * Get the callback for the user application to supply events
	 *
	 * @return callback function for the user application.
	 */
	CallbackType getCallback ()
	{ return boost::bind( &PushSupplier< EventType >::send, &m_outPort, _1 ); }

	/**
	 * Method to call to send stringified data.
	 * For simple java/python wrapping.
	 */
	void receiveString( const char* s ) throw()
	{
		try
		{
			// read from string
			Util::SimpleStringIArchive ar( s );
			EventType e( boost::shared_ptr< typename EventType::value_type >( new typename EventType::value_type() ) );
			ar >> e;
			
			// add timestamp if necessary
			if ( !e.time() )
				e = EventType( Measurement::now(), e );
			
			m_outPort.send( e );
		}
		catch ( const std::runtime_error& )
		{}
		catch( ... )
		{}
	}
	
protected:
	/** Input port of the function. */
	PushSupplier< EventType > m_outPort;
};

typedef ApplicationPushSource< Measurement::Rotation > ApplicationPushSourceRotation;
//typedef ApplicationPushSource< Measurement::Position > ApplicationPushSourcePosition;
//typedef ApplicationPushSource< Measurement::PositionList > ApplicationPushSourcePositionList;
typedef ApplicationPushSource< Measurement::PositionList2 > ApplicationPushSourcePosition2DList;
typedef ApplicationPushSource< Measurement::PoseList > ApplicationPushSourcePoseList;


/**
 * Specialization of \c ApplicationPushSource for \c Measurement::Pose, which also supports the
 * \c Facade::SimplePoseReceiver interface for interfacing with Python, Java, etc.
 */
class ApplicationPushSourcePose
	: public ApplicationPushSource< Measurement::Pose >
    , public Facade::SimplePoseReceiver
{
public:
	/**
	 * UTQL component constructor.
	 *
	 * @param nm Unique name of the component.
	 * @param cfg \c UTQL subgraph
	 */
	ApplicationPushSourcePose( const std::string& nm, boost::shared_ptr< Graph::UTQLSubgraph > subgraph )
		: ApplicationPushSource< Measurement::Pose >( nm, subgraph )
	{}

	/** implements the \c Facade::SimplePoseReceiver interface */
	void receivePose( const Facade::SimplePose& pose ) throw()
	{
		// convert SimplePose to Measurement::Pose
		m_outPort.send( Measurement::Pose( pose.timestamp,
			Math::Pose(
				Math::Quaternion( pose.rx, pose.ry, pose.rz, pose.rw ),
				Math::Vector< double, 3 >( pose.tx, pose.ty, pose.tz )
				)
			) );
	}
};


/**
 * Specialization of \c ApplicationPushSource for \c Measurement::Position2D, which also supports the
 * \c Facade::SimplePosition2DReceiver interface for interfacing with Python, Java, etc.
 */
class ApplicationPushSourcePosition2D
	: public ApplicationPushSource< Measurement::Position2D >
    , public Facade::SimplePosition2DReceiver
{
public:
	/**
	 * UTQL component constructor.
	 *
	 * @param nm Unique name of the component.
	 * @param cfg \c UTQL subgraph
	 */
	ApplicationPushSourcePosition2D( const std::string& nm, boost::shared_ptr< Graph::UTQLSubgraph > subgraph )
		: ApplicationPushSource< Measurement::Position2D >( nm, subgraph )
		, m_logger( log4cpp::Category::getInstance( "Ubitrack.Components.ApplicationPushSourcePosition2D" ) )
	{}

	/** implements the \c Facade::SimplePosition2DReceiver interface */
	void receivePosition2D( const Facade::SimplePosition2D& position2d ) throw()
	{
		// convert SimplePosition2D to Measurement::Position2D
		m_outPort.send( Measurement::Position2D( position2d.timestamp,
			Math::Vector< double, 2 >(position2d.x, position2d.y)
			) );
		LOG4CPP_INFO( m_logger, "ApplicationPushSourcePosition2D receivePosition2D: x=" << position2d.x << " y=" << position2d.y );
	}

	/** reference to logger */
	log4cpp::Category& m_logger;
};

class ApplicationPushSourcePosition
	: public ApplicationPushSource< Measurement::Position >
    , public Facade::SimplePosition3DReceiver
{
public:
	/**
	 * UTQL component constructor.
	 *
	 * @param nm Unique name of the component.
	 * @param cfg \c UTQL subgraph
	 */
	ApplicationPushSourcePosition( const std::string& nm, boost::shared_ptr< Graph::UTQLSubgraph > subgraph )
		: ApplicationPushSource< Measurement::Position >( nm, subgraph )
		, m_logger( log4cpp::Category::getInstance( "Ubitrack.Components.ApplicationPushSourcePosition3D" ) )
	{}

	/** implements the \c Facade::SimplePosition2DReceiver interface */
	void receivePosition3D( const Facade::SimplePosition3D& position3d ) throw()
	{
		// convert SimplePosition2D to Measurement::Position2D
		m_outPort.send( Measurement::Position( position3d.timestamp,
			Math::Vector< double, 3 >(position3d.x, position3d.y, position3d.z)
			) );
		LOG4CPP_INFO( m_logger, "ApplicationPushSourcePosition3D receivePosition3D: x=" << position3d.x << " y=" << position3d.y<< " z=" << position3d.z  );
	}

	/** reference to logger */
	log4cpp::Category& m_logger;
};


/**
 * Specialization of \c ApplicationPushSource for \c Measurement::Button, which also supports the
 * \c Facade::SimpleButtonReceiver interface for interfacing with Python, Java, etc.
 */
class ApplicationPushSourceButton
	: public ApplicationPushSource< Measurement::Button >
    , public Facade::SimpleButtonReceiver
{
public:
	/**
	 * UTQL component constructor.
	 *
	 * @param nm Unique name of the component.
	 * @param cfg \c UTQL subgraph
	 */
	ApplicationPushSourceButton( const std::string& nm, boost::shared_ptr< Graph::UTQLSubgraph > subgraph )
		: ApplicationPushSource< Measurement::Button >( nm, subgraph )
	{}

	/** implements the \c Facade::SimpleButtonReceiver interface */
	void receiveButton( const Facade::SimpleButton& button ) throw()
	{
		// convert SimpleButton to Measurement::Button
		m_outPort.send( Measurement::Button( button.timestamp,
											 Math::Scalar<int>( button.event ) ) );
	}

};

class ApplicationPushSourcePositionList
	: public ApplicationPushSource< Measurement::PositionList >
    , public Facade::SimplePositionList3DReceiver
{
public:
	/**
	 * UTQL component constructor.
	 *
	 * @param nm Unique name of the component.
	 * @param cfg \c UTQL subgraph
	 */
	ApplicationPushSourcePositionList( const std::string& nm, boost::shared_ptr< Graph::UTQLSubgraph > subgraph )
		: ApplicationPushSource< Measurement::PositionList >( nm, subgraph )
		, m_logger( log4cpp::Category::getInstance( "Ubitrack.Components.ApplicationPushSourcePositionList" ) )
	{}

	/** implements the \c Facade::SimplePosition2DReceiver interface */
	void receivePositionList3D( const Facade::SimplePositionList3D& positionlist3d ) throw()
	{
		const std::vector<SimplePosition3DValue>& values = positionlist3d.values;
		std::vector< Math::Vector< double, 3 > > newValues;
		
		for(unsigned int i=0;i<values.size();i++){
			newValues.push_back(Math::Vector< double, 3 >(values[i].x,values[i].y,values[i].z));
		}
		
		m_outPort.send( Measurement::PositionList( positionlist3d.timestamp,
			newValues
			) );
		LOG4CPP_INFO( m_logger, "ApplicationPushSourcePositionList receivePosition3DList: " );
	}

	/** reference to logger */
	log4cpp::Category& m_logger;
};

} } // namespace Ubitrack::Components
#endif //__UBITRACK_COMPONENTS_APPLICATIONPUSHSOURCE_H_INCLUDED__

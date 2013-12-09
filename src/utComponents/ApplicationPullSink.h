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
 * Sink component with pull input port
 * This files containes an sink component with which the application
 * can interface which uses an PullConsumer input port.
 *
 * @author Manuel Huber <huberma@in.tum.de>
 */

#ifndef __UBITRACK_COMPONENTS_APPLICATIONPULLSINK_H_INCLUDED__
#define __UBITRACK_COMPONENTS_APPLICATIONPULLSINK_H_INCLUDED__

#include <string>
#include <iostream>

#include <boost/bind.hpp>

#include <utDataflow/PullConsumer.h>
#include <utDataflow/Component.h>
#include <utDataflow/ComponentFactory.h>
#include <utMeasurement/Measurement.h>

namespace Ubitrack { namespace Components {

using namespace Dataflow;

/**
 * @ingroup dataflow_components
 * This is an sink component which may be used to interface
 * the dataflow network to an user application.
 *
 * This sink uses a pull input port and also exhibits a pull
 * interface to the application.
 *
 * @par Input Ports
 * PullConsumer<EventType> port with name "Input".
 *
 * @par Output Ports
 * None.
 *
 * @par Configuration
 * None.
 *
 * @par Operation
 * Whenever the user requests a measurement via the get method
 * of the sink the input port and thus the dataflow network
 * is queried for the corresponding measurement. The user
 * has to supply the requested timestamp.
 *
 * @par Instances
 * Registered for the following EventTypes and names:
 * - Ubitrack::Measurement::Pose : ApplicationPullSinkPose
 * - Ubitrack::Measurement::Position : ApplicationPullSinkPosition
 * - Ubitrack::Measurement::Rotation : ApplicationPullSinkRotation
 * - Ubitrack::Measurement::PositionList : ApplicationPullSinkPositionList
 */
template < class EventType >
class ApplicationPullSink
	: public Component
{
public:
	/**
	 * UTQL component constructor.
	 *
	 * @param sName Unique name of the component.
	 * @param subgraph UTQL subgraph
	 */
	ApplicationPullSink( const std::string& nm, boost::shared_ptr< Graph::UTQLSubgraph> )
      : Ubitrack::Dataflow::Component( nm )
      , m_InPort( "Input", *this )
    {
    }

	/**
	 * Get function as interface to user.
	 * This function queries the dataflow network and delivers the
	 * requested data to the user.
	 * @param t Timestamp for which the data is requested.
	 * @return Event as requested.
	 * @throws Ubitrack::Util::Exception if the connected pull supplier cannot deliver data
	 */
    EventType get( Ubitrack::Measurement::Timestamp t )
    {
      return m_InPort.get ( t );
    }

protected:
	/**
	 * Input port of the function.
	 */
	PullConsumer< EventType > m_InPort;
};

typedef ApplicationPullSink< Measurement::Pose > ApplicationPullSinkPose;
typedef ApplicationPullSink< Measurement::ErrorPose > ApplicationPullSinkErrorPose;
typedef ApplicationPullSink< Measurement::Position > ApplicationPullSinkPosition;
typedef ApplicationPullSink< Measurement::ErrorPosition > ApplicationPullSinkErrorPosition;
typedef ApplicationPullSink< Measurement::Rotation > ApplicationPullSinkRotation;
typedef ApplicationPullSink< Measurement::PositionList > ApplicationPullSinkPositionList;
typedef ApplicationPullSink< Measurement::ErrorPositionList > ApplicationPullSinkErrorPositionList;
typedef ApplicationPullSink< Measurement::PositionList2 > ApplicationPullSinkPositionList2;
typedef ApplicationPullSink< Measurement::Button > ApplicationPullSinkSkalar;
typedef ApplicationPullSink< Measurement::Matrix3x3 > ApplicationPullSinkMatrix3x3;
typedef ApplicationPullSink< Measurement::Matrix4x4 > ApplicationPullSinkMatrix4x4;


} } // namespace Ubitrack::Components

#endif //__UBITRACK_COMPONENTS_APPLICATIONPULLSINK_H_INCLUDED__

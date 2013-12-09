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
 * Implements a simple interface to the ubitrack dataflow network.
 *
 * @author Daniel Pustka <daniel.pustka@in.tum.de>
 */

#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <boost/bind.hpp>
#include <log4cpp/Category.hh>

#include "../utComponents/ApplicationPullSink.h"

#ifdef HAVE_OPENCV
#include <utVision/Image.h>
#endif

#include "SimpleFacade.h"
#include "AdvancedFacade.h"
#include "DataflowObserver.h"
#include "SimpleApplicationPrivate.h"
// get a logger
static log4cpp::Category& logger( log4cpp::Category::getInstance( "Ubitrack.Facade.SimpleFacade" ) );


namespace {

// this function converts Measurement::Pose to SimplePose in a callback
void convertPoseCallback( Ubitrack::Facade::SimplePoseReceiver* receiver, const Ubitrack::Measurement::Pose& measurement )
{
	using namespace Ubitrack;
	Facade::SimplePose p;
	p.tx = measurement->translation()( 0 );
	p.ty = measurement->translation()( 1 );
	p.tz = measurement->translation()( 2 );
	p.rx = measurement->rotation().x();
	p.ry = measurement->rotation().y();
	p.rz = measurement->rotation().z();
	p.rw = measurement->rotation().w();
	p.timestamp = measurement.time();
	receiver->receivePose( p );
}

// this function converts Measurement::Pose to SimplePose in a callback
void convertErrorPoseCallback( Ubitrack::Facade::SimpleErrorPoseReceiver* receiver, const Ubitrack::Measurement::ErrorPose& measurement )
{
	using namespace Ubitrack;
	Facade::SimpleErrorPose p;
	p.tx = measurement->translation()( 0 );
	p.ty = measurement->translation()( 1 );
	p.tz = measurement->translation()( 2 );
	p.rx = measurement->rotation().x();
	p.ry = measurement->rotation().y();
	p.rz = measurement->rotation().z();
	p.rw = measurement->rotation().w();
	
	p.co11 = measurement->covariance()(0, 0); p.co12 = measurement->covariance()(0, 1); p.co13 = measurement->covariance()(0, 2);
	p.co14 = measurement->covariance()(0, 3); p.co15 = measurement->covariance()(0, 4); p.co16 = measurement->covariance()(0, 5);

	p.co21 = measurement->covariance()(1, 0); p.co22 = measurement->covariance()(1, 1); p.co23 = measurement->covariance()(1, 2);
	p.co24 = measurement->covariance()(1, 3); p.co25 = measurement->covariance()(1, 4); p.co26 = measurement->covariance()(1, 5);

	p.co31 = measurement->covariance()(2, 0); p.co32 = measurement->covariance()(2, 1); p.co33 = measurement->covariance()(2, 2);
	p.co34 = measurement->covariance()(2, 3); p.co35 = measurement->covariance()(2, 4); p.co36 = measurement->covariance()(2, 5);

	p.co41 = measurement->covariance()(3, 0); p.co42 = measurement->covariance()(3, 1); p.co43 = measurement->covariance()(3, 2);
	p.co44 = measurement->covariance()(3, 3); p.co45 = measurement->covariance()(3, 4); p.co46 = measurement->covariance()(3, 5);

	p.co51 = measurement->covariance()(4, 0); p.co52 = measurement->covariance()(4, 1); p.co53 = measurement->covariance()(4, 2);
	p.co54 = measurement->covariance()(4, 3); p.co55 = measurement->covariance()(4, 4); p.co56 = measurement->covariance()(4, 5);

	p.co61 = measurement->covariance()(5, 0); p.co62 = measurement->covariance()(5, 1); p.co63 = measurement->covariance()(5, 2);
	p.co64 = measurement->covariance()(5, 3); p.co65 = measurement->covariance()(5, 4); p.co66 = measurement->covariance()(5, 5);	
	
	p.timestamp = measurement.time();
	receiver->receiveErrorPose( p );
}

// this function converts Measurement::Matrix3x4 to SimpleMatrix3x4 in a callback
void convertMatrix3x4Callback( Ubitrack::Facade::SimpleMatrix3x4Receiver* receiver, const Ubitrack::Measurement::Matrix3x4& measurement )
{
	using namespace Ubitrack;
	Facade::SimpleMatrix3x4 p;
	p.e11 = (*measurement)(0,0);
	p.e21 = (*measurement)(1,0);
	p.e31 = (*measurement)(2,0);
	p.e12 = (*measurement)(0,1);
	p.e22 = (*measurement)(1,1);
	p.e32 = (*measurement)(2,1);
	p.e13 = (*measurement)(0,2);
	p.e23 = (*measurement)(1,2);
	p.e33 = (*measurement)(2,2);
	p.e14 = (*measurement)(0,3);
	p.e24 = (*measurement)(1,3);
	p.e34 = (*measurement)(2,3);
	p.timestamp = measurement.time();
	LOG4CPP_INFO( logger, "Trying to send 3x4 Matrix at timestamp: " << p.timestamp );
	receiver->receiveMatrix3x4( p );
}

// this function converts Measurement::Matrix4x4 to SimpleMatrix4x4 in a callback
void convertMatrix4x4Callback( Ubitrack::Facade::SimpleMatrix4x4Receiver* receiver, const Ubitrack::Measurement::Matrix4x4& measurement )
{
	using namespace Ubitrack;
	Facade::SimpleMatrix4x4 p;
	
	memcpy(p.values,measurement->content() , 16*sizeof(double));		
	
	p.timestamp = measurement.time();
	LOG4CPP_INFO( logger, "Trying to send 4x4 Matrix at timestamp: " << p.timestamp );	
	receiver->receiveMatrix4x4( p );
}

// this function converts Measurement::Distance to SimpleDistance in a callback
void convertDistanceCallback( Ubitrack::Facade::SimpleDistanceReceiver* receiver, const Ubitrack::Measurement::Distance& measurement )
{
	using namespace Ubitrack;
	Facade::SimpleDistance d;
	d.d = *measurement;
	d.timestamp = measurement.time();
	receiver->receiveDistance( d );
}

void convertPosition2DCallback( Ubitrack::Facade::SimplePosition2DReceiver* receiver, const Ubitrack::Measurement::Position2D& measurement )
{
	using namespace Ubitrack;
	Facade::SimplePosition2D p;
	p.x = (*measurement)(0);
	p.y = (*measurement)(1);
	p.timestamp = measurement.time();
	receiver->receivePosition2D( p );
}

void convert2DPositionListCallback( Ubitrack::Facade::SimplePosition2DListReceiver* receiver, const Ubitrack::Measurement::PositionList2& measurement )
{
	using namespace Ubitrack;
	Facade::SimplePosition2DList p;
	const std::size_t count( measurement->size() );
	for( std::size_t i( 0 ); i<count; i++ )
	{
		SimplePosition2DValue value;
		Math::Vector< double, 2 >  v2 = (*measurement)[i];
		value.x = v2[0];
		value.y = v2[1];
		p.values.push_back(value);
	}
	
	p.timestamp = measurement.time();
	receiver->receivePosition2DList(p);
}

#ifdef HAVE_OPENCV
// this function converts Measurement::ImageMeasurements to SimpleImage in a callback
void convertImageCallback( Ubitrack::Facade::SimpleImageReceiver* receiver, const Ubitrack::Measurement::ImageMeasurement& measurement )
{
	using namespace Ubitrack;
	Facade::SimpleImage i;

	i.width = measurement->width;
	i.height = measurement->height;
	i.imageData = (unsigned char *) measurement->imageData;
	i.widthStep = measurement->widthStep;
	i.depth = measurement->depth;
	i.nChannels = measurement->nChannels;
	i.imageSize = measurement->widthStep * measurement->height;

	i.timestamp = measurement.time();

	receiver->receiveImage( i );
}
#endif

}

void convert3DPositionCallback( Ubitrack::Facade::SimplePosition3DReceiver* receiver, const Ubitrack::Measurement::Position& measurement )
{
	using namespace Ubitrack;
	Facade::SimplePosition3D value;
	
	value.x = (*measurement)[0];
	value.y = (*measurement)[1];
	value.z = (*measurement)[2];
	
	value.timestamp = measurement.time();
	receiver->receivePosition3D(value);
}

void convert3DErrorPositionCallback( Ubitrack::Facade::SimpleErrorPosition3DReceiver* receiver, const Ubitrack::Measurement::ErrorPosition& measurement )
{
	using namespace Ubitrack;	
	
	Facade::SimpleErrorPosition3D value;	
	
	Math::Vector< double, 3 >  v3 = measurement->value;
	value.x = v3[0];
	value.y = v3[1];
	value.z = v3[2];
	memcpy(value.covariance, measurement->covariance.content(), sizeof(double)*9);
	value.timestamp = measurement.time();
	receiver->receiveErrorPosition3D(value);
}

void convert3DPositionListCallback( Ubitrack::Facade::SimplePositionList3DReceiver* receiver, const Ubitrack::Measurement::PositionList& measurement )
{
	using namespace Ubitrack;
	Facade::SimplePositionList3D p;
	const std::size_t count( measurement->size() );
	for( std::size_t i( 0 ); i<count; i++ )
	{
		SimplePosition3DValue value;
		Math::Vector< double, 3 >  v3 = (*measurement)[i];
		value.x = v3[0];
		value.y = v3[1];
		value.z = v3[2];
		p.values.push_back(value);
	}
	
	p.timestamp = measurement.time();
	receiver->receivePositionList3D(p);
}

void convert3DErrorPositionListCallback( Ubitrack::Facade::SimpleErrorPositionList3DReceiver* receiver, const Ubitrack::Measurement::ErrorPositionList& measurement )
{
	using namespace Ubitrack;
	Facade::SimpleErrorPositionList3D p;
	const std::size_t count( measurement->size() );
	for( std::size_t i( 0 ); i<count; i++ )
	{
		SimpleErrorPosition3DValue value;
		Math::Vector< double, 3 >  v3 = (*measurement)[i].value;
		value.x = v3[0];
		value.y = v3[1];
		value.z = v3[2];
		
		memcpy(value.covariance, (*measurement)[i].covariance.content(), sizeof(double)*9)		;
		
		p.values.push_back(value);
	}
	
	p.timestamp = measurement.time();
	receiver->receiveErrorPositionList3D(p);
}



namespace Ubitrack { namespace Facade {




/** private things */
class SimpleFacadePrivate
	: public AdvancedFacade
	, public DataflowObserver
{
public:
	SimpleFacadePrivate( const char* sComponentPath )
		: AdvancedFacade( sComponentPath ? sComponentPath : std::string() )
		, m_pSimpleObserver( 0 )
	{}

	// translate from DataflowObserver to SimpleDataflowObserver
	void notifyAddComponent( const std::string& sPatternName, const std::string& sComponentName, const Graph::UTQLSubgraph& )
	{
		if ( m_pSimpleObserver )
			m_pSimpleObserver->notifyAddComponent( sPatternName.c_str(), sComponentName.c_str() );
	}

	void notifyDeleteComponent( const std::string& sPatternName, const std::string& sComponentName )
	{
		if ( m_pSimpleObserver )
			m_pSimpleObserver->notifyDeleteComponent( sPatternName.c_str(), sComponentName.c_str() );
	}
	
	SimpleDataflowObserver* m_pSimpleObserver;
};



/**
 * Override SimpleApplicationPullSinkPose to provide proxy functionality for the
 * actual dataflow component
 */
class SimpleApplicationPullSinkPosePrivate 
	: public SimpleApplicationPullSinkPose {

private:
	/** dataflow component this proxy object represents */
	Ubitrack::Components::ApplicationPullSinkPose * pSink;

	/** content of the last error message */
	char* m_sError;

	/** sets the internal error string */
	void setError( const char* sMsg ) throw();


public:
	SimpleApplicationPullSinkPosePrivate( Ubitrack::Components::ApplicationPullSinkPose * pSink );
	
	~SimpleApplicationPullSinkPosePrivate();
	
	virtual bool getPose( SimplePose & pose, unsigned long long int timestamp );
};



SimpleApplicationPullSinkPosePrivate::SimpleApplicationPullSinkPosePrivate( Ubitrack::Components::ApplicationPullSinkPose * pSink )
	: m_sError( 0 )
{
	this->pSink = pSink;
}


SimpleApplicationPullSinkPosePrivate::~SimpleApplicationPullSinkPosePrivate()
{
	if ( m_sError )
		free( m_sError );
}


/** retrieves a pose for the given timestamp */
bool SimpleApplicationPullSinkPosePrivate::getPose( SimplePose & pose, unsigned long long int timestamp )
{
	if ( pSink == NULL )
		return false;
	
	try {
		LOG4CPP_INFO( logger, "Trying to pull pose in SimpleApplicationPullSinkPosePrivate::getPose() for timestamp: " << timestamp );
		// Retrieve measurement for current timestamp
		Ubitrack::Measurement::Pose p = pSink->get(timestamp);
		LOG4CPP_INFO( logger, "Sucessfully pulled pose in SimpleApplicationPullSinkPosePrivate::getPose(): " << p );
		// Convert measurement
		pose.tx = p->translation()( 0 );
		pose.ty = p->translation()( 1 );
		pose.tz = p->translation()( 2 );
		pose.rx = p->rotation().x();
		pose.ry = p->rotation().y();
		pose.rz = p->rotation().z();
		pose.rw = p->rotation().w();
		pose.timestamp = p.time();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleApplicationPullSinkPosePrivate::getPose(): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}


void SimpleApplicationPullSinkPosePrivate::setError( const char* sMsg ) throw()
{
	if ( m_sError )
		free( m_sError );
	m_sError = strdup( sMsg );
}


SimpleFacade::SimpleFacade( const char* sComponentPath ) throw()
	: m_pPrivate( 0 )
	, m_sError( 0 )
{
	try
	{
		m_pPrivate = new SimpleFacadePrivate( sComponentPath );
		LOG4CPP_TRACE( logger, "SimpleFacadePrivate created successfully" );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception constructing SimpleFacade: " << e );
		setError( e.what() );
	}
}


SimpleFacade::~SimpleFacade()
{
	delete m_pPrivate;
	if ( m_sError )
		free( m_sError );
}

unsigned long long int SimpleFacade::now()
{
	return Measurement::now();
}


bool SimpleFacade::loadDataflow( const char* sDfSrg ) throw()
{
	try
	{
		m_pPrivate->loadDataflow( sDfSrg );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::loadDataflow: " << e );
		setError( e.what() );
		return false;
	}

	return true;
}


bool SimpleFacade::loadDataflowString( const char* sDataflow ) throw()
{
	try
	{
		std::istringstream ss( sDataflow );
		m_pPrivate->loadDataflow( ss );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::loadDataflowString: " << e );
		setError( e.what() );
		return false;
	}

	return true;
}


void SimpleFacade::clearDataflow() throw()
{
	try
	{
		// start the event queue
		m_pPrivate->clearDataflow();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::clearDataflow: " << e );
		setError( e.what() );
	}
}


void SimpleFacade::startDataflow() throw()
{
	try
	{
		// start the event queue
		m_pPrivate->startDataflow();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::startDataflow: " << e );
		setError( e.what() );
	}
}


void SimpleFacade::stopDataflow() throw()
{
	try
	{
		// stop the event queue
		m_pPrivate->stopDataflow();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::stopDataflow: " << e );
		setError( e.what() );
	}
}


void SimpleFacade::connectToServer( const char* sAddress ) throw()
{
	try
	{
		m_pPrivate->connectToServer( sAddress );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::connectToServer: " << e );
		setError( e.what() );
	}
}


void SimpleFacade::sendUtqlToServer( const char* sUtqlFile ) throw()
{
	try
	{
		m_pPrivate->sendUtqlToServer( sUtqlFile );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::sendUtqlToServer: " << e );
		setError( e.what() );
	}
}


void SimpleFacade::sendUtqlToServerString( const char* buffer ) throw()
{
	try
	{
		m_pPrivate->sendUtqlToServerString( buffer );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::sendUtqlToServerString: " << e );
		setError( e.what() );
	}
}


bool SimpleFacade::setPoseCallback( const char* sCallbackName, SimplePoseReceiver* pCallback ) throw()
{
	try
	{
		m_pPrivate->setCallback< Measurement::Pose >( sCallbackName, boost::bind( &convertPoseCallback, pCallback, _1 ) );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::setPoseCallback( " << sCallbackName <<" ): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

bool SimpleFacade::setErrorPoseCallback( const char* sCallbackName, SimpleErrorPoseReceiver* pCallback ) throw()
{
	try
	{
		m_pPrivate->setCallback< Measurement::ErrorPose >( sCallbackName, boost::bind( &convertErrorPoseCallback, pCallback, _1 ) );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::setErrorPoseCallback( " << sCallbackName <<" ): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

bool SimpleFacade::setMatrix3x4Callback( const char* sCallbackName, SimpleMatrix3x4Receiver* pCallback ) throw()
{
	try
	{
		m_pPrivate->setCallback< Measurement::Matrix3x4 >( sCallbackName, boost::bind( &convertMatrix3x4Callback, pCallback, _1 ) );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::setMatrix3x4Callback( " << sCallbackName <<" ): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

bool SimpleFacade::setMatrix4x4Callback( const char* sCallbackName, SimpleMatrix4x4Receiver* pCallback ) throw()
{
	try
	{
		m_pPrivate->setCallback< Measurement::Matrix4x4 >( sCallbackName, boost::bind( &convertMatrix4x4Callback, pCallback, _1 ) );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::setMatrix4x4Callback( " << sCallbackName <<" ): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

bool SimpleFacade::setDistanceCallback( const char* sCallbackName, SimpleDistanceReceiver* pCallback ) throw()
{
	try
	{
		m_pPrivate->setCallback< Measurement::Distance >( sCallbackName, boost::bind( &convertDistanceCallback, pCallback, _1 ) );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::setDistanceCallback( " << sCallbackName <<" ): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}


bool SimpleFacade::setStringCallback( const char* sCallbackName, SimpleStringReceiver* pCallback ) throw()
{
	try
	{
		m_pPrivate->componentByName< Components::ApplicationPushSinkBase >( sCallbackName )->setStringCallback( pCallback );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::setStringCallback( " << sCallbackName <<" ): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

bool SimpleFacade::setPosition2DCallback( const char* sCallbackName, SimplePosition2DReceiver* pCallback ) throw()
{
	try
	{
		//m_pPrivate->componentByName< Components::ApplicationPushSinkBase >( sCallbackName )->setPosition2DCallback( pCallback );
		m_pPrivate->setCallback< Measurement::Position2D >( sCallbackName, boost::bind( &convertPosition2DCallback, pCallback, _1 ) );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::setPosition2DCallback( " << sCallbackName <<" ): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

bool SimpleFacade::setPosition2DListCallback( const char* sCallbackName, SimplePosition2DListReceiver* pCallback ) throw()
{
	try
	{
		m_pPrivate->setCallback< Measurement::PositionList2 >( sCallbackName, boost::bind( &convert2DPositionListCallback, pCallback, _1 ) );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::setPosition2DListCallback( " << sCallbackName <<" ): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

#ifdef HAVE_OPENCV
bool SimpleFacade::setImageCallback( const char* sCallbackName, SimpleImageReceiver* pCallback ) throw()
{
	try
	{
		m_pPrivate->setCallback< Measurement::ImageMeasurement >( sCallbackName, boost::bind( &convertImageCallback, pCallback, _1 ) );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::setImageCallback( " << sCallbackName <<" ): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}
#endif

bool SimpleFacade::set3DPositionCallback( const char* sCallbackName, SimplePosition3DReceiver* pCallback ) throw()
{
	try
	{
		m_pPrivate->setCallback< Measurement::Position >( sCallbackName, boost::bind( &convert3DPositionCallback, pCallback, _1 ) );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::set3DPositionCallback( " << sCallbackName <<" ): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

bool SimpleFacade::set3DErrorPositionCallback( const char* sCallbackName, SimpleErrorPosition3DReceiver* pCallback ) throw()
{
	try
	{
		m_pPrivate->setCallback< Measurement::ErrorPosition >( sCallbackName, boost::bind( &convert3DErrorPositionCallback, pCallback, _1 ) );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::set3DErrorPositionCallback( " << sCallbackName <<" ): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

bool SimpleFacade::set3DPositionListCallback( const char* sCallbackName, SimplePositionList3DReceiver* pCallback ) throw()
{
	try
	{
		m_pPrivate->setCallback< Measurement::PositionList >( sCallbackName, boost::bind( &convert3DPositionListCallback, pCallback, _1 ) );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::set3DPositionListCallback( " << sCallbackName <<" ): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

bool SimpleFacade::set3DErrorPositionListCallback( const char* sCallbackName, SimpleErrorPositionList3DReceiver* pCallback ) throw()
{
	try
	{
		m_pPrivate->setCallback< Measurement::ErrorPositionList >( sCallbackName, boost::bind( &convert3DErrorPositionListCallback, pCallback, _1 ) );
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::set3DErrorPositionListCallback( " << sCallbackName <<" ): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}



SimplePosition2DReceiver* SimpleFacade::getPushSourcePosition2D( const char* sComponentName ) throw()
{
	try
	{
		return m_pPrivate->componentByName< Components::ApplicationPushSourcePosition2D >( sComponentName ).get();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getPushSourcePosition2D( " << sComponentName <<" ): " << e );
		setError( e.what() );
		return 0;
	}
}

SimplePosition3DReceiver* SimpleFacade::getPushSourcePosition3D( const char* sComponentName ) throw()
{
	try
	{
		LOG4CPP_INFO( logger, "Trying to get SimpleFacade::getPushSourcePosition3D for "<<sComponentName);
		SimplePosition3DReceiver* result = m_pPrivate->componentByName< Components::ApplicationPushSourcePosition >( sComponentName ).get();
		LOG4CPP_INFO( logger, "Getting SimpleFacade::getPushSourcePosition3D  "<<result);
		return result;
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getPushSourcePosition3D( " << sComponentName <<" ): " << e );
		setError( e.what() );
		return 0;
	}
}

SimplePoseReceiver* SimpleFacade::getPushSourcePose( const char* sComponentName ) throw()
{
	try
	{
		return m_pPrivate->componentByName< Components::ApplicationPushSourcePose >( sComponentName ).get();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getPushSourcePose( " << sComponentName <<" ): " << e );
		setError( e.what() );
		return 0;
	}
}





SimpleButtonReceiver* SimpleFacade::getPushSourceButton( const char* sComponentName ) throw()
{
	try
	{
	  SimpleButtonReceiver * pr = m_pPrivate->componentByName< Components::ApplicationPushSourceButton >( sComponentName ).get();
	  LOG4CPP_DEBUG( logger, "Successfully retrieved component " << sComponentName << " in SimpleFacade::getPushSourceButton " );
	  return pr;
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getPushSourceButton( " << sComponentName <<" ): " << e );
		setError( e.what() );
		return 0;
	}
}


SimpleStringReceiver* SimpleFacade::getPushSourceString( const char* sComponentName ) throw()
{
	try
	{
	  SimpleStringReceiver * pr = m_pPrivate->componentByName< SimpleStringReceiver >( sComponentName ).get();
	  LOG4CPP_DEBUG( logger, "Successfully retrieved component " << sComponentName << " in SimpleFacade::getPushSourceString " );
	  return pr;
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getPushSourceString( " << sComponentName <<" ): " << e );
		setError( e.what() );
		return 0;
	}
}

SimplePositionList3DReceiver* SimpleFacade::getPushSourcePositionList3D( const char* sComponentName ) throw()
{
	try
	{
	  SimplePositionList3DReceiver * pr = m_pPrivate->componentByName< SimplePositionList3DReceiver >( sComponentName ).get();
	  LOG4CPP_DEBUG( logger, "Successfully retrieved component " << sComponentName << " in SimpleFacade::getPushSourcePositionList3D " );
	  return pr;
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getPushSourcePositionList3D( " << sComponentName <<" ): " << e );
		setError( e.what() );
		return 0;
	}
}


SimpleApplicationPullSinkPose* SimpleFacade::getSimplePullSinkPose( const char* sComponentName ) throw()
{
	Ubitrack::Components::ApplicationPullSinkPose * pSink;

	try
	{
		 pSink = m_pPrivate->componentByName< Components::ApplicationPullSinkPose >( sComponentName ).get();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getSimplePullSinkPose( " << sComponentName <<" ): " << e );
		setError( e.what() );
		return 0;
	}

	// Create proxy object and return it
	SimpleApplicationPullSinkPose * pSinkPriv = new SimpleApplicationPullSinkPosePrivate( pSink );

	return pSinkPriv;
}

SimpleApplicationPullSinkErrorPose * SimpleFacade::getSimplePullSinkErrorPose( const char* sComponentName ) throw()
{
	Ubitrack::Components::ApplicationPullSinkErrorPose * pSink;

	try
	{
		 pSink = m_pPrivate->componentByName< Components::ApplicationPullSinkErrorPose >( sComponentName ).get();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getSimplePullSinkPose( " << sComponentName <<" ): " << e );
		setError( e.what() );
		return 0;
	}

	// Create proxy object and return it
	SimpleApplicationPullSinkErrorPose * pSinkPriv = new SimpleApplicationPullSinkErrorPosePrivate( pSink );

	return pSinkPriv;
}

SimpleApplicationPullSinkMatrix3x3 * SimpleFacade::getPullSinkMatrix3x3( const char* sComponentName ) throw()
{
	Ubitrack::Components::ApplicationPullSinkMatrix3x3 * pSink;

	try
	{
		 pSink = m_pPrivate->componentByName< Components::ApplicationPullSinkMatrix3x3 >( sComponentName ).get();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getPullSinkMatrix3x3( " << sComponentName <<" ): " << e );
		setError( e.what() );
		return 0;
	}

	// Create proxy object and return it
	SimpleApplicationPullSinkMatrix3x3 * pSinkPriv = new SimpleApplicationPullSinkMatrix3x3Private( pSink );

	return pSinkPriv;
}

SimpleApplicationPullSinkMatrix4x4 * SimpleFacade::getPullSinkMatrix4x4( const char* sComponentName ) throw()
{
	Ubitrack::Components::ApplicationPullSinkMatrix4x4 * pSink;

	try
	{
		 pSink = m_pPrivate->componentByName< Components::ApplicationPullSinkMatrix4x4 >( sComponentName ).get();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getPullSinkMatrix4x4( " << sComponentName <<" ): " << e );
		setError( e.what() );
		return 0;
	}

	// Create proxy object and return it
	SimpleApplicationPullSinkMatrix4x4 * pSinkPriv = new SimpleApplicationPullSinkMatrix4x4Private( pSink );

	return pSinkPriv;
}
	
SimpleApplicationPullSinkPosition3D * SimpleFacade::getPullSinkPosition3D( const char* sComponentName ) throw()
{
	Ubitrack::Components::ApplicationPullSinkPosition * pSink;

	try
	{
		 pSink = m_pPrivate->componentByName< Components::ApplicationPullSinkPosition >( sComponentName ).get();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getPullSinkPosition3D( " << sComponentName <<" ): " << e );
		setError( e.what() );
		return 0;
	}

	// Create proxy object and return it
	SimpleApplicationPullSinkPosition3D * pSinkPriv = new SimpleApplicationPullSinkPosition3DPrivate( pSink );

	return pSinkPriv;
}
SimpleApplicationPullSinkErrorPosition3D * SimpleFacade::getPullSinkErrorPosition3D( const char* sComponentName ) throw()
{
	Ubitrack::Components::ApplicationPullSinkErrorPosition * pSink;

	try
	{
		 pSink = m_pPrivate->componentByName< Components::ApplicationPullSinkErrorPosition >( sComponentName ).get();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getPullSinkErrorPosition3D( " << sComponentName <<" ): " << e );
		setError( e.what() );
		return 0;
	}

	// Create proxy object and return it
	SimpleApplicationPullSinkErrorPosition3D * pSinkPriv = new SimpleApplicationPullSinkErrorPosition3DPrivate( pSink );

	return pSinkPriv;
}
SimpleApplicationPullSinkPositionList3D * SimpleFacade::getPullSinkPosition3DList( const char* sComponentName ) throw()
{
	Ubitrack::Components::ApplicationPullSinkPositionList * pSink;

	try
	{
		 pSink = m_pPrivate->componentByName< Components::ApplicationPullSinkPositionList >( sComponentName ).get();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getPullSinkPosition3DList( " << sComponentName <<" ): " << e );
		setError( e.what() );
		return 0;
	}

	// Create proxy object and return it
	SimpleApplicationPullSinkPositionList3D * pSinkPriv = new SimpleApplicationPullSinkPositionList3DPrivate( pSink );

	return pSinkPriv;
}
SimpleApplicationPullSinkErrorPositionList3D * SimpleFacade::getPullSinkErrorPosition3DList( const char* sComponentName ) throw()
{
	Ubitrack::Components::ApplicationPullSinkErrorPositionList * pSink;

	try
	{
		 pSink = m_pPrivate->componentByName< Components::ApplicationPullSinkErrorPositionList >( sComponentName ).get();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleFacade::getPullSinkErrorPosition3DList( " << sComponentName <<" ): " << e );
		setError( e.what() );
		return 0;
	}

	// Create proxy object and return it
	SimpleApplicationPullSinkErrorPositionList3D * pSinkPriv = new SimpleApplicationPullSinkErrorPositionList3DPrivate( pSink );

	return pSinkPriv;
}


void SimpleFacade::setDataflowObserver( SimpleDataflowObserver* pObserver ) throw()
{
	if ( !m_pPrivate->m_pSimpleObserver )
		m_pPrivate->addDataflowObserver( m_pPrivate );
	m_pPrivate->m_pSimpleObserver = pObserver;
}

	
void SimpleFacade::removeDataflowObserver() throw()
{
	m_pPrivate->removeDataflowObserver( m_pPrivate );
	m_pPrivate->m_pSimpleObserver = 0;
}


const char* SimpleFacade::getLastError() throw()
{
	if ( m_pPrivate )
		return m_sError;
	else
		return "Error constructing SimpleFacade -- see logging for details";
}


void SimpleFacade::setError( const char* sMsg ) throw()
{
	if ( m_sError )
		free( m_sError );
	m_sError = strdup( sMsg );
}

void SimpleFacade::killEverything(){
	m_pPrivate->killEverything();
}

} } // namespace Ubitrack::Facade

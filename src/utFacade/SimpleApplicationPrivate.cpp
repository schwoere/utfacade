#include "SimpleApplicationPrivate.h"
#include <log4cpp/Category.hh>
#include <string.h>

static log4cpp::Category& logger( log4cpp::Category::getInstance( "Ubitrack.Facade.SimpleApplication" ) );

namespace Ubitrack { namespace Facade {
	
bool SimpleApplicationPullSinkMatrix3x3Private::getMatrix3x3( SimpleMatrix3x3 & matrix, unsigned long long int timestamp )
{
	if ( pSink == NULL )
		return false;
	
	try {
		LOG4CPP_INFO( logger, "Trying to pull pose in SimpleApplicationPullSinkMatrix3x3Private::getMatrix3x3 for timestamp: " << timestamp );
		// Retrieve measurement for current timestamp
		Ubitrack::Measurement::Matrix3x3 p = pSink->get(timestamp);
		LOG4CPP_INFO( logger, "Sucessfully pulled pose in SimpleApplicationPullSinkMatrix3x3Private::getMatrix3x3: " << p );
		// Convert measurement
		matrix.values[0] = (*p)(0,0);
		matrix.values[1] = (*p)(0,1);
		matrix.values[2] = (*p)(0,2);
		matrix.values[3] = (*p)(1,0);
		matrix.values[4] = (*p)(1,1);
		matrix.values[5] = (*p)(1,2);
		matrix.values[6] = (*p)(2,0);
		matrix.values[7] = (*p)(2,1);
		matrix.values[8] = (*p)(2,2);				
		matrix.timestamp = p.time();

	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleApplicationPullSinkMatrix3x3Private::getMatrix3x3(): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

bool SimpleApplicationPullSinkMatrix4x4Private::getMatrix4x4( SimpleMatrix4x4 & matrix, unsigned long long int timestamp )
{
	if ( pSink == NULL )
		return false;
	
	try {
		LOG4CPP_INFO( logger, "Trying to pull pose in SimpleApplicationPullSinkMatrix4x4Private::getMatrix4x4 for timestamp: " << timestamp );
		// Retrieve measurement for current timestamp
		Ubitrack::Measurement::Matrix4x4 p = pSink->get(timestamp);
		LOG4CPP_INFO( logger, "Sucessfully pulled pose in SimpleApplicationPullSinkMatrix4x4Private::getMatrix4x4: " << p );
		// Convert measurement
		memcpy(matrix.values, p->content(), 16*sizeof(double));		
		matrix.timestamp = p.time();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleApplicationPullSinkMatrix4x4Private::getMatrix4x4(): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

bool SimpleApplicationPullSinkPosition3DPrivate::getPosition3D(  SimplePosition3D & pos, unsigned long long int timestamp )
{
	if ( pSink == NULL )
		return false;
	
	try {
		LOG4CPP_INFO( logger, "Trying to pull pose in SimpleApplicationPullSinkPosition3DPrivate::getPosition3D for timestamp: " << timestamp );
		// Retrieve measurement for current timestamp
		Ubitrack::Measurement::Position p = pSink->get(timestamp);
		LOG4CPP_INFO( logger, "Sucessfully pulled pose in SimpleApplicationPullSinkPosition3DPrivate::getPosition3D: " << p );
		// Convert measurement
		pos.x = (*p)[0];
		pos.y = (*p)[1];
		pos.z = (*p)[2];
		pos.timestamp = p.time();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleApplicationPullSinkPosition3DPrivate::getPosition3D(): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

bool SimpleApplicationPullSinkErrorPosition3DPrivate::getErrorPosition3D( SimpleErrorPosition3D & pos, unsigned long long int timestamp )
{
	if ( pSink == NULL )
		return false;
	
	try {
		LOG4CPP_INFO( logger, "Trying to pull pose in SimpleApplicationPullSinkErrorPosition3DPrivate::getErrorPosition3D for timestamp: " << timestamp );
		// Retrieve measurement for current timestamp
		Ubitrack::Measurement::ErrorPosition p = pSink->get(timestamp);
		LOG4CPP_INFO( logger, "Sucessfully pulled pose in SimpleApplicationPullSinkErrorPosition3DPrivate::getErrorPosition3D: " << p );
		// Convert measurement		
	
		Math::Vector< double, 3 >  v3 = p->value;
		pos.x = v3[0];
		pos.y = v3[1];
		pos.z = v3[2];
		memcpy(pos.covariance, p->covariance.content(), sizeof(double)*9);	
			
		pos.timestamp = p.time();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleApplicationPullSinkErrorPosition3DPrivate::getErrorPosition3D(): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

bool SimpleApplicationPullSinkPositionList3DPrivate::getPositionList3D(  SimplePositionList3D & pos, unsigned long long int timestamp )
{
	if ( pSink == NULL )
		return false;
	
	try {
		LOG4CPP_INFO( logger, "Trying to pull pose in SimpleApplicationPullSinkPositionList3DPrivate::getPositionList3D for timestamp: " << timestamp );
		// Retrieve measurement for current timestamp
		Ubitrack::Measurement::PositionList p = pSink->get(timestamp);
		//LOG4CPP_INFO( logger, "Sucessfully pulled pose in SimpleApplicationPullSinkPositionList3DPrivate::getPositionList3D: " << p );
		// Convert measurement
		const std::size_t count ( p->size() );
		
		for( std::size_t i( 0 ); i<count; i++ )
		{
			SimplePosition3DValue value;
			Math::Vector< double, 3 >  v3 = (*p)[i];
			value.x = v3[0];
			value.y = v3[1];
			value.z = v3[2];
			pos.values.push_back(value);
		}
		
		pos.timestamp = p.time();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleApplicationPullSinkPositionList3DPrivate::getPositionList3D(): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

bool SimpleApplicationPullSinkErrorPositionList3DPrivate::getErrorPositionList3D( SimpleErrorPositionList3D & pos, unsigned long long int timestamp )
{
	if ( pSink == NULL )
		return false;
	
	try {
		LOG4CPP_INFO( logger, "Trying to pull pose in SimpleApplicationPullSinkErrorPositionList3DPrivate::getErrorPositionList3D for timestamp: " << timestamp );
		// Retrieve measurement for current timestamp
		Ubitrack::Measurement::ErrorPositionList p = pSink->get(timestamp);
		//LOG4CPP_INFO( logger, "Sucessfully pulled pose in SimpleApplicationPullSinkErrorPositionList3DPrivate::getErrorPositionList3D: " << p );
		// Convert measurement
		const std::size_t count ( p->size() );
		
		for( std::size_t i( 0 ); i<count; i++ )
		{
			SimpleErrorPosition3DValue value;
			Math::Vector< double, 3 >  v3 = (*p)[i].value;
			value.x = v3[0];
			value.y = v3[1];
			value.z = v3[2];
			
			memcpy(value.covariance, (*p)[i].covariance.content(), sizeof(double)*9)		;
			
			pos.values.push_back(value);
		}
			
		pos.timestamp = p.time();
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleApplicationPullSinkErrorPositionList3DPrivate::getErrorPositionList3D(): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

/** retrieves a pose for the given timestamp */
bool SimpleApplicationPullSinkErrorPosePrivate::getPose( SimpleErrorPose & p, unsigned long long int timestamp )
{
	if ( pSink == NULL )
		return false;
	
	try {
		LOG4CPP_INFO( logger, "Trying to pull pose in SimpleApplicationPullSinkPosePrivate::getPose() for timestamp: " << timestamp );
		// Retrieve measurement for current timestamp
		Ubitrack::Measurement::ErrorPose measurement = pSink->get(timestamp);
		LOG4CPP_INFO( logger, "Sucessfully pulled pose in SimpleApplicationPullSinkPosePrivate::getPose(): " << measurement );
		// Convert measurement
				
			
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
		
	}
	catch ( const Ubitrack::Util::Exception& e )
	{
		LOG4CPP_ERROR( logger, "Caught exception in SimpleApplicationPullSinkPosePrivate::getPose(): " << e );
		setError( e.what() );
		return false;
	}

	return true;
}

}}
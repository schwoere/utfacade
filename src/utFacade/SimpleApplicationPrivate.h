#ifndef __UBITRACK_FACADE_SIMPLEAPPLICATION_PRIVATE_H_INCLUDED__
#define __UBITRACK_FACADE_SIMPLEAPPLICATION_PRIVATE_H_INCLUDED__


#include "../utComponents/ApplicationPullSink.h"
#include "SimpleDatatypes.h"

namespace Ubitrack { namespace Facade {


class SimpleApplicationPullSinkMatrix3x3Private 
	: public SimpleApplicationPullSinkMatrix3x3 {

private:
	/** dataflow component this proxy object represents */
	Ubitrack::Components::ApplicationPullSinkMatrix3x3 * pSink;

	/** content of the last error message */
	char* m_sError;

	/** sets the internal error string */
	void setError( const char* sMsg ) throw()
	{
		if ( m_sError )
			free( m_sError );
		m_sError = strdup( sMsg );
	}
	
	


public:
	SimpleApplicationPullSinkMatrix3x3Private( Ubitrack::Components::ApplicationPullSinkMatrix3x3 * pSink ): m_sError( 0 )
	{
		this->pSink = pSink;
	}
	
	~SimpleApplicationPullSinkMatrix3x3Private()
	{
		if ( m_sError )
			free( m_sError );
	}
	
	virtual bool getMatrix3x3( SimpleMatrix3x3 & matrix, unsigned long long int timestamp );
};

class SimpleApplicationPullSinkMatrix4x4Private 
	: public SimpleApplicationPullSinkMatrix4x4 {

private:
	/** dataflow component this proxy object represents */
	Ubitrack::Components::ApplicationPullSinkMatrix4x4 * pSink;

	/** content of the last error message */
	char* m_sError;

	/** sets the internal error string */
	void setError( const char* sMsg ) throw()
	{
		if ( m_sError )
			free( m_sError );
		m_sError = strdup( sMsg );
	}
	
	


public:
	SimpleApplicationPullSinkMatrix4x4Private( Ubitrack::Components::ApplicationPullSinkMatrix4x4 * pSink ): m_sError( 0 )
	{
		this->pSink = pSink;
	}
	
	~SimpleApplicationPullSinkMatrix4x4Private()
	{
		if ( m_sError )
			free( m_sError );
	}
	
	virtual bool getMatrix4x4( SimpleMatrix4x4 & matrix, unsigned long long int timestamp );
};

class SimpleApplicationPullSinkPosition3DPrivate 
	: public SimpleApplicationPullSinkPosition3D {

private:
	/** dataflow component this proxy object represents */
	Ubitrack::Components::ApplicationPullSinkPosition * pSink;

	/** content of the last error message */
	char* m_sError;

	/** sets the internal error string */
	void setError( const char* sMsg ) throw()
	{
		if ( m_sError )
			free( m_sError );
		m_sError = strdup( sMsg );
	}


public:
	SimpleApplicationPullSinkPosition3DPrivate( Ubitrack::Components::ApplicationPullSinkPosition * pSink ): m_sError( 0 )
	{
		this->pSink = pSink;
	}
	
	~SimpleApplicationPullSinkPosition3DPrivate()
	{
		if ( m_sError )
			free( m_sError );
	}
	
	virtual bool getPosition3D( SimplePosition3D & pos, unsigned long long int timestamp );
};

class SimpleApplicationPullSinkErrorPosition3DPrivate 
	: public SimpleApplicationPullSinkErrorPosition3D {

private:
	/** dataflow component this proxy object represents */
	Ubitrack::Components::ApplicationPullSinkErrorPosition * pSink;

	/** content of the last error message */
	char* m_sError;

	/** sets the internal error string */
	void setError( const char* sMsg ) throw()
	{
		if ( m_sError )
			free( m_sError );
		m_sError = strdup( sMsg );
	}


public:
	SimpleApplicationPullSinkErrorPosition3DPrivate( Ubitrack::Components::ApplicationPullSinkErrorPosition * pSink ): m_sError( 0 )
	{
		this->pSink = pSink;
	}
	
	~SimpleApplicationPullSinkErrorPosition3DPrivate()
	{
		if ( m_sError )
			free( m_sError );
	}
	
	virtual bool getErrorPosition3D( SimpleErrorPosition3D & pos, unsigned long long int timestamp );
};

class SimpleApplicationPullSinkPositionList3DPrivate 
	: public SimpleApplicationPullSinkPositionList3D {

private:
	/** dataflow component this proxy object represents */
	Ubitrack::Components::ApplicationPullSinkPositionList * pSink;

	/** content of the last error message */
	char* m_sError;

	/** sets the internal error string */
	void setError( const char* sMsg ) throw()
	{
		if ( m_sError )
			free( m_sError );
		m_sError = strdup( sMsg );
	}


public:
	SimpleApplicationPullSinkPositionList3DPrivate( Ubitrack::Components::ApplicationPullSinkPositionList * pSink ): m_sError( 0 )
	{
		this->pSink = pSink;
	}
	
	~SimpleApplicationPullSinkPositionList3DPrivate()
	{
		if ( m_sError )
			free( m_sError );
	}
	
	virtual bool getPositionList3D( SimplePositionList3D & pos, unsigned long long int timestamp );
};

class SimpleApplicationPullSinkErrorPositionList3DPrivate 
	: public SimpleApplicationPullSinkErrorPositionList3D {

private:
	/** dataflow component this proxy object represents */
	Ubitrack::Components::ApplicationPullSinkErrorPositionList * pSink;

	/** content of the last error message */
	char* m_sError;

	/** sets the internal error string */
	void setError( const char* sMsg ) throw()
	{
		if ( m_sError )
			free( m_sError );
		m_sError = strdup( sMsg );
	}


public:
	SimpleApplicationPullSinkErrorPositionList3DPrivate( Ubitrack::Components::ApplicationPullSinkErrorPositionList * pSink ): m_sError( 0 )
	{
		this->pSink = pSink;
	}
	
	~SimpleApplicationPullSinkErrorPositionList3DPrivate()
	{
		if ( m_sError )
			free( m_sError );
	}
	
	virtual bool getErrorPositionList3D( SimpleErrorPositionList3D & pos, unsigned long long int timestamp );
};


class SimpleApplicationPullSinkErrorPosePrivate 
	: public SimpleApplicationPullSinkErrorPose {

private:
	/** dataflow component this proxy object represents */
	Ubitrack::Components::ApplicationPullSinkErrorPose * pSink;

	/** content of the last error message */
	char* m_sError;

	/** sets the internal error string */
	void setError( const char* sMsg ) throw()
	{
		if ( m_sError )
			free( m_sError );
		m_sError = strdup( sMsg );
	}
	


public:
	SimpleApplicationPullSinkErrorPosePrivate( Ubitrack::Components::ApplicationPullSinkErrorPose * pSink ): m_sError( 0 )
	{
		this->pSink = pSink;
	}
	
	~SimpleApplicationPullSinkErrorPosePrivate()
	{
		if ( m_sError )
		free( m_sError );
	}
	
	virtual bool getPose( SimpleErrorPose & pose, unsigned long long int timestamp );
};



}
}

#endif

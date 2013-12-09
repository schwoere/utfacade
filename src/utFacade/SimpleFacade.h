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
 * Defines a simple interface to the ubitrack dataflow network.
 *
 * @author Daniel Pustka <daniel.pustka@in.tum.de>
 */
#ifndef __UBITRACK_FACADE_SIMPLEFACADE_H_INCLUDED__
#define __UBITRACK_FACADE_SIMPLEFACADE_H_INCLUDED__
#include <utFacade/utFacade.h>

#include <utFacade/Config.h>

#include "SimpleDatatypes.h"
// Do not add additional includes here

namespace Ubitrack { namespace Facade {

// forward decls
class SimpleFacadePrivate;

/**
 * Provides a much simplified interface to the ubitrack library in a facade software design 
 * pattern fashion. 
 *
 * The goal is to reduce dependencies on compiler-specific implementation details and to allow 
 * straightforward porting to other programing languages. 
 *
 * This means:
 * - no Boost
 * - no templates
 * - no STL, not anymore (by PaF), lists are now supported
 * - no pointer ownership changes
 * - no exceptions
 *
 * These restrictions also extend to included files!!!
 *
 * Unfortunately, only Pose events are supported.
 */
class UTFACADE_EXPORT SimpleFacade
{
public:
	/**
	 * Initializes the Ubitrack library.
	 *
	 * @param sComponentPath Path to component directory. Uses default directory if none is specified
	 */
	SimpleFacade( const char* sComponentPath = 0 ) throw();
	
	/** destroys the data flow */
	~SimpleFacade();

	static unsigned long long int now();
	
	/** 
	 * Loads and instantiates a dataflow network from an XML file
	 *
	 * @param sDfSrg filename of dataflow description
	 * @return true if successful
	 */
	bool loadDataflow( const char* sDfSrg ) throw();
	
	/** 
	 * Loads and instantiates a dataflow network from a string
	 *
	 * @param sDataflow string containing the actual XML dataflow
	 * @return true if successful
	 */
	bool loadDataflowString( const char* sDataflow ) throw();
	
	/** removes all dataflow component instances */
	void clearDataflow() throw();
	

	/** starts components and the event queue */
	void startDataflow() throw();
	
	/** stops components and the event queue */
	void stopDataflow() throw();


	/** 
	 * connect to a ubitrack server.
	 * @param sAddress format: <hostname> [":" <port>]
	 */
	void connectToServer( const char* sAddress ) throw();

	/**
	 * sends the contents of a file to a connected ubitrack server.
	 */
	void sendUtqlToServer( const char* sUtqlFile ) throw();
	
	/**
	 * sends a string to a connected ubitrack server.
	 */
	void sendUtqlToServerString( const char* buffer ) throw();	
	
	
	/**
	 * Sets a callback on an ApplicationPushSinkPose
	 *
	 * @param sComponentName edge name of the ApplicationPushSinkPose
	 * @param pCallback the interface to call when an event is received
	 * @return true if successful
	 */
	bool setPoseCallback( const char* sComponentName, SimplePoseReceiver* pCallback ) throw();

	/**
	 * Sets a callback on an ApplicationPushSinkErrorPose
	 *
	 * @param sComponentName edge name of the ApplicationPushSinkPose
	 * @param pCallback the interface to call when an event is received
	 * @return true if successful
	 */
	bool setErrorPoseCallback( const char* sComponentName, SimpleErrorPoseReceiver* pCallback ) throw();

	
	/**
	 * Sets a callback on an ApplicationPushSinkMatrix3x4
	 *
	 * @param sComponentName edge name of the ApplicationPushSinkMatrix3x4
	 * @param pCallback the interface to call when an event is received
	 * @return true if successful
	 */
	bool setMatrix3x4Callback( const char* sCallbackName, SimpleMatrix3x4Receiver* pCallback ) throw();
	
	/**
	 * Sets a callback on an ApplicationPushSinkMatrix4x4
	 *
	 * @param sComponentName edge name of the ApplicationPushSinkMatrix4x4
	 * @param pCallback the interface to call when an event is received
	 * @return true if successful
	 */
	bool setMatrix4x4Callback( const char* sCallbackName, SimpleMatrix4x4Receiver* pCallback ) throw();

	/**
	 * Sets a callback on an ApplicationPushSinkDistance
	 *
	 * @param sComponentName edge name of the ApplicationPushSinkDistance
	 * @param pCallback the interface to call when an event is received
	 * @return true if successful
	 */
	bool setDistanceCallback( const char* sComponentName, SimpleDistanceReceiver* pCallback ) throw();
	
	/**
	 * Sets a string callback on an ApplicationPushSink
	 *
	 * @param sComponentName edge name of the ApplicationPushSink
	 * @param pCallback the interface to call when an event is received
	 * @return true if successful
	 */
	bool setStringCallback( const char* sComponentName, SimpleStringReceiver* pCallback ) throw();

	/**
	 * Sets a position 2d callback on an ApplicationPushSink
	 *
	 * @param sComponentName edge name of the ApplicationPushSink
	 * @param pCallback the interface to call when an event is received
	 * @return true if successful
	 */
	bool setPosition2DCallback( const char* sComponentName, SimplePosition2DReceiver* pCallback ) throw();
	
	bool setPosition2DListCallback( const char* sComponentName, SimplePosition2DListReceiver* pCallback ) throw();
	
#ifdef HAVE_OPENCV
	/**
	 * Sets a callback on an ApplicationPushSinkVision
	 *
	 * @param sComponentName edge name of the ApplicationPushSinkVision
	 * @param pCallback the interface to call when an event is received
	 * @return true if successful
	 */
	bool setImageCallback( const char* sComponentName, SimpleImageReceiver* pCallback ) throw();
#endif

	bool set3DPositionCallback( const char* sComponentName, SimplePosition3DReceiver* pCallback ) throw();
	bool set3DErrorPositionCallback( const char* sComponentName, SimpleErrorPosition3DReceiver* pCallback ) throw();
	
	bool set3DPositionListCallback( const char* sComponentName, SimplePositionList3DReceiver* pCallback ) throw();
	bool set3DErrorPositionListCallback( const char* sComponentName, SimpleErrorPositionList3DReceiver* pCallback ) throw();
	
	/**
	 * Gets a pointer to a SimplePosition2DReceiver interface on a ApplicationPushSourcePosition2.
	 * The returned object will be deleted by Ubitrack.
	 *
	 * @param sComponentName name of the ApplicationPushSourcePosition2 component.
	 * @return NULL if component not found
	 */
	SimplePosition2DReceiver* getPushSourcePosition2D( const char* sComponentName ) throw();
	SimplePosition3DReceiver* getPushSourcePosition3D( const char* sComponentName ) throw();
	
	/**
	 * Gets a pointer to a SimplePoseReceiver interface on a ApplicationPushSourcePose.
	 * The returned object will be deleted by Ubitrack.
	 *
	 * @param sComponentName name of the ApplicationPushSourcePose component.
	 * @return NULL if component not found
	 */
	SimplePoseReceiver* getPushSourcePose( const char* sComponentName ) throw();

	

	/**
	 * Gets a pointer to a SimpleButtonReceiver interface on an ApplicationPushSourceButton.
	 * The returned object will be deleted by Ubitrack.
	 *
	 * @param sComponentName name of the ApplicationPushSourceButton component
	 * @return NULL if component not found
	 */
	SimpleButtonReceiver* getPushSourceButton( const char* sComponentName ) throw();

	/**
	 * Gets a pointer to a SimpleStringReceiver interface on an ApplicationPushSource.
	 * The returned object will be deleted by Ubitrack.
	 *
	 * @param sComponentName name of the ApplicationPushSource component
	 * @return NULL if component not found
	 */
	SimpleStringReceiver* getPushSourceString( const char* sComponentName ) throw();


	SimplePositionList3DReceiver* getPushSourcePositionList3D( const char* sComponentName ) throw();
	/**
	 * Gets a pointer to a SimpleApplicationPullSinkPose object which can be used for
	 * retrieving pose data via its getSimplePose() method.
	 *
	 * The returned object will NOT be freed by Ubitrack and has to be deleted by the caller. 
	 * It will render useless, however, when stopping the dataflow.
	 *
	 * @param sComponentName name of the ApplicationPullSinkPose component for
	 * which a proxy shall be returned
	 * @return NULL if component not found
	 */
	SimpleApplicationPullSinkPose * getSimplePullSinkPose( const char* sComponentName ) throw();
	
	SimpleApplicationPullSinkErrorPose * getSimplePullSinkErrorPose( const char* sComponentName ) throw();
	
	SimpleApplicationPullSinkMatrix3x3 * getPullSinkMatrix3x3( const char* sComponentName ) throw();
	
	SimpleApplicationPullSinkMatrix4x4 * getPullSinkMatrix4x4( const char* sComponentName ) throw();
	
	SimpleApplicationPullSinkPosition3D * getPullSinkPosition3D( const char* sComponentName ) throw();
	SimpleApplicationPullSinkErrorPosition3D * getPullSinkErrorPosition3D( const char* sComponentName ) throw();
	SimpleApplicationPullSinkPositionList3D * getPullSinkPosition3DList( const char* sComponentName ) throw();
	SimpleApplicationPullSinkErrorPositionList3D * getPullSinkErrorPosition3DList( const char* sComponentName ) throw();

	/**
	 * Get notifications when new dataflow components are created or deleted
	 *
	 * @param pObserver pointer to a SimpleDataflowObserver
	 */
	void setDataflowObserver( SimpleDataflowObserver* pObserver ) throw();
	
	/**
	 * remove notifications when new dataflow components are created or deleted
	 */
	void removeDataflowObserver() throw();
	
	/** returns the description of the last error or 0 if there was no error so far. */	
	const char* getLastError() throw();
	
	void killEverything();

protected:
	/** 
	 * Pointer to private data.
	 *
	 * The actual content is not declared here in order to avoid exposing the bad things (boost, 
	 * stl, etc.) to the application 
	 */
	SimpleFacadePrivate* m_pPrivate;
	
	/** content of the last error message */
	char* m_sError;
	
	/** sets the internal error string */
	void setError( const char* sMsg ) throw();
	
};


 } } // namespace Ubitrack::Facade

#endif

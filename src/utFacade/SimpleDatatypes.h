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
 * Simple datatypes for the \c SimpleFacade
 *
 * @author Daniel Pustka <daniel.pustka@in.tum.de>
 */
#ifndef __UBITRACK_FACADE_SIMPLEDATATYPES_H_INCLUDED__
#define __UBITRACK_FACADE_SIMPLEDATATYPES_H_INCLUDED__

#include "SimpleVectorTypes.h"



namespace Ubitrack { namespace Facade {

/**
 * A simplified version of the Measurement::Pose type without boost.
 */
struct SimplePose
{
	/** position as a 3-vector */
	double tx, ty, tz;

	/** orientation as a (x, y, z, w) quaternion */
	double rx, ry, rz, rw;

	/** timestamp in nanoseconds since unix epoch */
	unsigned long long int timestamp;
};

/**
 * A simplified version of the Measurement::ErrorPose type without boost.
 */
struct SimpleErrorPose
{
	/** position as a 3-vector */
	double tx, ty, tz;

	/** orientation as a (x, y, z, w) quaternion */
	double rx, ry, rz, rw;

	/** covariance of the pose */
	double co11, co12, co13, co14, co15, co16;
	double co21, co22, co23, co24, co25, co26;
	double co31, co32, co33, co34, co35, co36;
	double co41, co42, co43, co44, co45, co46;
	double co51, co52, co53, co54, co55, co56;
	double co61, co62, co63, co64, co65, co66;
	
	/** timestamp in nanoseconds since unix epoch */
	unsigned long long int timestamp;
};

/**
 * A simplified version of the Measurement::Position2D type without boost.
 */
struct SimplePosition2D
{
	/** position as a 2-vector */
	double x, y;

	/** timestamp in nanoseconds since unix epoch */
	unsigned long long int timestamp;
};

/**
 * A simplified version of the Measurement::Position3D type without boost.
 */
struct SimplePosition3D
{
	/** position as a 3-vector */
	double x, y, z;

	/** timestamp in nanoseconds since unix epoch */
	unsigned long long int timestamp;
};

struct SimpleErrorPosition3D
{
	double x, y, z;
	double covariance[9];

	/** timestamp in nanoseconds since unix epoch */
	unsigned long long int timestamp;
};

/**
 * A simplified version of the Measurement::Position2D type without boost.
 */
struct SimplePosition2DList
{
	/** position as a 2-vector */
	std::vector<SimplePosition2DValue> values;

	/** timestamp in nanoseconds since unix epoch */
	unsigned long long int timestamp;
};

/**
 * A simplified version of the Measurement::Position3D type without boost.
 */
struct SimplePositionList3D
{
	/** position as a 3-vector */
	std::vector<SimplePosition3DValue> values;

	/** timestamp in nanoseconds since unix epoch */
	unsigned long long int timestamp;
};

struct SimpleErrorPositionList3D
{
	/** position as a 3-vector */
	std::vector<SimpleErrorPosition3DValue> values;

	/** timestamp in nanoseconds since unix epoch */
	unsigned long long int timestamp;
};


/**
 * A simplified version of the Measurement::Matrix3x4 type without boost.
 */
struct SimpleMatrix3x4
{
	/** Matrix elements */
	double e11, e12, e13, e14, e21, e22, e23, e24, e31, e32, e33, e34;

	/** timestamp in nanoseconds since unix epoch */
	unsigned long long int timestamp;
};


/**
 * A simplified version of the Measurement::Matrix3x3 type without boost.
 */
struct SimpleMatrix3x3
{
	/** Matrix elements */
	double values[9];

	/** timestamp in nanoseconds since unix epoch */
	unsigned long long int timestamp;
};

/**
 * A simplified version of the Measurement::Matrix3x3 type without boost.
 */
struct SimpleMatrix4x4
{
	/** Matrix elements */
	double values[16];

	/** timestamp in nanoseconds since unix epoch */
	unsigned long long int timestamp;
};

/**
 * A simplified version of the Measurement::Distance type without boost.
 */
struct SimpleDistance
{
	/** distance */
	double d;

	/** timestamp in nanoseconds since unix epoch */
	unsigned long long int timestamp;
};


/**
 * A simple callback interface to transport SimplePoses
 */
class SimplePoseReceiver
{
public:
	/** receives a SimplePose */
	virtual void receivePose( const SimplePose& pose ) throw() = 0;
	
	/** virtual destructor */
	virtual ~SimplePoseReceiver()
	{}
};

/**
 * A simple callback interface to transport SimpleErrorPoses
 */
class SimpleErrorPoseReceiver
{
public:
	/** receives a SimplePose */
	virtual void receiveErrorPose( const SimpleErrorPose& pose ) throw() = 0;
	
	/** virtual destructor */
	virtual ~SimpleErrorPoseReceiver()
	{}
};

/**
 * A simple callback interface to transport SimplePosition2D
 */
class SimplePosition2DReceiver
{
public:
	/** receives a SimplePosition2D */
	virtual void receivePosition2D( const SimplePosition2D& position2d ) throw() = 0;

	/** virtual destructor */
	virtual ~SimplePosition2DReceiver()
	{}
};


class SimplePosition2DListReceiver
{
public:
	/** receives a SimplePosition2D */
	virtual void receivePosition2DList( const SimplePosition2DList& position2dList ) throw() = 0;

	/** virtual destructor */
	virtual ~SimplePosition2DListReceiver()
	{}
};

class SimplePosition3DReceiver
{
public:
	/** receives a SimplePosition2D */
	virtual void receivePosition3D( const SimplePosition3D& position3d ) throw() = 0;

	/** virtual destructor */
	virtual ~SimplePosition3DReceiver()
	{}
};

class SimpleErrorPosition3DReceiver
{
public:
	/** receives a SimplePosition2D */
	virtual void receiveErrorPosition3D( const SimpleErrorPosition3D& position3d ) throw() = 0;

	/** virtual destructor */
	virtual ~SimpleErrorPosition3DReceiver()
	{}
};

class SimplePositionList3DReceiver
{
public:
	/** receives a SimplePosition2D */
	virtual void receivePositionList3D( const SimplePositionList3D& position3dList ) throw() = 0;

	/** virtual destructor */
	virtual ~SimplePositionList3DReceiver()
	{}
};

class SimpleErrorPositionList3DReceiver
{
public:
	/** receives a SimplePosition2D */
	virtual void receiveErrorPositionList3D( const SimpleErrorPositionList3D& errorPosition3dList ) throw() = 0;

	/** virtual destructor */
	virtual ~SimpleErrorPositionList3DReceiver()
	{}
};

/**
 * A simple callback interface to transport SimpleMatrix3x4
 */
class SimpleMatrix3x4Receiver
{
public:
	/** receives a SimpleMatrix3x4 */
	virtual void receiveMatrix3x4( const SimpleMatrix3x4& matrix ) throw() = 0;
	
	/** virtual destructor */
	virtual ~SimpleMatrix3x4Receiver()
	{}
};

class SimpleMatrix3x3Receiver
{
public:
	/** receives a SimpleMatrix3x3 */
	virtual void receiveMatrix3x3( const SimpleMatrix3x3& matrix ) throw() = 0;
	
	/** virtual destructor */
	virtual ~SimpleMatrix3x3Receiver()
	{}
};

class SimpleMatrix4x4Receiver
{
public:
	/** receives a SimpleMatrix4x4 */
	virtual void receiveMatrix4x4( const SimpleMatrix4x4& matrix ) throw() = 0;
	
	/** virtual destructor */
	virtual ~SimpleMatrix4x4Receiver()
	{}
};


/**
 * A simple callback interface to transport SimpleInts
 */
class SimpleDistanceReceiver
{
public:
	/** receives a SimpleDistance */
	virtual void receiveDistance( const SimpleDistance& dist ) throw() = 0;
	
	/** virtual destructor */
	virtual ~SimpleDistanceReceiver()
	{}
};


/**
 * A simplified version of the Measurement::Button type without boost.
 */
struct SimpleButton
{
	/** button event as scalar */
	int event;

	/** timestamp in nanoseconds since unix epoch */
	unsigned long long int timestamp;
};


/**
 * A simple callback interface to transport SimpleButton
 */
class SimpleButtonReceiver
{
public:
	/** receives a SimpleButton */
	virtual void receiveButton( const SimpleButton& button ) throw() = 0;
	
	/** virtual destructor */
	virtual ~SimpleButtonReceiver()
	{}
};


/**
 * A simplified version of the Measurement::ImageMeasurement type without boost.
 */
struct SimpleImage
{
	/** image raw data */
	int width;
	int height;
	int imageSize;
	int widthStep;
	int depth;
	int nChannels;
	unsigned char * imageData;

	/** timestamp in nanoseconds since unix epoch */
	unsigned long long int timestamp;
};


/**
 * A simple callback interface to transport SimpleImage
 */
class SimpleImageReceiver
{
public:
	/** receives a SimpleImage */
	virtual void receiveImage( const SimpleImage& image ) throw() = 0;
	
	/** virtual destructor */
	virtual ~SimpleImageReceiver()
	{}
};


/**
 * A simple callback interface to transport strings.
 * Can be used for language-independent transport of more complex data types.
 */
class SimpleStringReceiver
{
public:
	/** receives a SimpleButton */
	virtual void receiveString( const char* ) throw() = 0;
	
	/** virtual destructor */
	virtual ~SimpleStringReceiver()
	{}
};


/**
 * A simple data flow observer
 */
class SimpleDataflowObserver
{
public:
	/** called when a component is added */
	virtual void notifyAddComponent( const char* sPatternName, const char* sComponentName ) throw() = 0;

	/** called when a component is removed */
	virtual void notifyDeleteComponent( const char* sPatternName, const char* sComponentName ) throw() = 0;
	
	/** virtual destructor */
	virtual ~SimpleDataflowObserver()
	{}
};


/**
 * A simple proxy object for an ApplicationPullSinkPose component
 */
class SimpleApplicationPullSinkPose {
protected:
	SimpleApplicationPullSinkPose()
	{}

public:
	/** virtual destructor */
	virtual ~SimpleApplicationPullSinkPose()
	{}

	/**
	 * Retrieves current pose data with the given timestamp
	 *
	 * @param pose Pose data is returned in this object on success
	 * @param timestamp Timestamp for which measurement shall be retrieved
	 * @return true if successfull
	 */
	virtual bool getPose( SimplePose & pose, unsigned long long int timestamp ) = 0;
};

class SimpleApplicationPullSinkErrorPose {
protected:
	SimpleApplicationPullSinkErrorPose()
	{}

public:
	/** virtual destructor */
	virtual ~SimpleApplicationPullSinkErrorPose()
	{}

	/**
	 * Retrieves current pose data with the given timestamp
	 *
	 * @param pose Pose data is returned in this object on success
	 * @param timestamp Timestamp for which measurement shall be retrieved
	 * @return true if successfull
	 */
	virtual bool getPose( SimpleErrorPose & pose, unsigned long long int timestamp ) = 0;
};

class SimpleApplicationPullSinkMatrix3x3 {
protected:
	SimpleApplicationPullSinkMatrix3x3()
	{}

public:
	/** virtual destructor */
	virtual ~SimpleApplicationPullSinkMatrix3x3()
	{}

	virtual bool getMatrix3x3( SimpleMatrix3x3 & matrix, unsigned long long int timestamp ) = 0;
};

class SimpleApplicationPullSinkMatrix4x4 {
protected:
	SimpleApplicationPullSinkMatrix4x4()
	{}

public:
	/** virtual destructor */
	virtual ~SimpleApplicationPullSinkMatrix4x4()
	{}

	virtual bool getMatrix4x4( SimpleMatrix4x4 & matrix, unsigned long long int timestamp ) = 0;
};

class SimpleApplicationPullSinkPosition3D {
protected:
	SimpleApplicationPullSinkPosition3D()
	{}

public:
	/** virtual destructor */
	virtual ~SimpleApplicationPullSinkPosition3D()
	{}
	
	virtual bool getPosition3D( SimplePosition3D & pos, unsigned long long int timestamp ) = 0;
};

class SimpleApplicationPullSinkErrorPosition3D {
protected:
	SimpleApplicationPullSinkErrorPosition3D()
	{}

public:
	/** virtual destructor */
	virtual ~SimpleApplicationPullSinkErrorPosition3D()
	{}

	virtual bool getErrorPosition3D( SimpleErrorPosition3D & pos, unsigned long long int timestamp ) = 0;
};

class SimpleApplicationPullSinkPositionList3D {
protected:
	SimpleApplicationPullSinkPositionList3D()
	{}

public:
	/** virtual destructor */
	virtual ~SimpleApplicationPullSinkPositionList3D()
	{}
	
	virtual bool getPositionList3D( SimplePositionList3D & pos, unsigned long long int timestamp ) = 0;
};

class SimpleApplicationPullSinkErrorPositionList3D {
protected:
	SimpleApplicationPullSinkErrorPositionList3D()
	{}

public:
	/** virtual destructor */
	virtual ~SimpleApplicationPullSinkErrorPositionList3D()
	{}

	virtual bool getErrorPositionList3D( SimpleErrorPositionList3D & pos, unsigned long long int timestamp ) = 0;
};


} } // namespace Ubitrack::Facade

#endif

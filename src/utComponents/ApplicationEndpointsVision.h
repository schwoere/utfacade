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
 * @author Benjamin Becker <benjamin.becker@eads.net>
 */

#include <utComponents/ApplicationPushSink.h>
#include <utComponents/ApplicationPullSink.h>
#include <utComponents/ApplicationPushSource.h>
#include <utVision/Image.h>


namespace Ubitrack { namespace Components {

typedef ApplicationPushSink< Measurement::ImageMeasurement > ApplicationPushSinkVisionImage;
typedef ApplicationPullSink< Measurement::ImageMeasurement > ApplicationPullSinkVisionImage;
typedef ApplicationPushSource< Measurement::ImageMeasurement > ApplicationPushSourceVisionImage;

} } // namespace Ubitrack::Components



namespace Ubitrack {  namespace Dataflow {
/**
 * \internal
 * Defines how to extract the priority out of a data type. 
 * Specialized for image measurements to reduce the maximum queue length.
 */
template<>
struct EventTypeTraits< Measurement::ImageMeasurement >
{
	unsigned long long getPriority( const Measurement::ImageMeasurement& m ) const
	{ return m.time(); }

	/** the maximum queue length for images is 1! */
	int getMaxQueueLength() const
	{ return 1; }
};

} }


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

#include <utDataflow/ComponentFactory.h>
#include "ApplicationPullSink.h"

namespace Ubitrack { namespace Components {

UBITRACK_REGISTER_COMPONENT( ComponentFactory* const cf ) {
	cf->registerComponent< ApplicationPullSinkPose > ( "ApplicationPullSinkPose" );
	cf->registerComponent< ApplicationPullSinkErrorPose > ( "ApplicationPullSinkErrorPose" );
	cf->registerComponent< ApplicationPullSinkPosition > ( "ApplicationPullSinkPosition" );
	cf->registerComponent< ApplicationPullSinkErrorPosition > ( "ApplicationPullSinkErrorPosition" );
	cf->registerComponent< ApplicationPullSinkRotation > ( "ApplicationPullSinkRotation" );
	cf->registerComponent< ApplicationPullSinkPositionList > ( "ApplicationPullSinkPositionList" );
	cf->registerComponent< ApplicationPullSinkErrorPositionList > ( "ApplicationPullSinkErrorPositionList" );
	cf->registerComponent< ApplicationPullSinkPositionList2 > ( "ApplicationPullSinkPosition2DList" );
	cf->registerComponent< ApplicationPullSinkSkalar > ( "ApplicationPullSinkSkalar" );
	cf->registerComponent< ApplicationPullSinkMatrix3x3 > ( "ApplicationPullSinkMatrix3x3" );
	
}

} } // namespace Ubitrack::Components

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

#include "ApplicationPushSource.h"
#include <utDataflow/ComponentFactory.h>

namespace Ubitrack { namespace Components {

using namespace Ubitrack::Dataflow;

UBITRACK_REGISTER_COMPONENT( ComponentFactory* const cf ) {
	cf->registerComponent< ApplicationPushSourceButton > ( "ApplicationPushSourceButton" );
	cf->registerComponent< ApplicationPushSourceRotation > ( "ApplicationPushSourceRotation" );
	cf->registerComponent< ApplicationPushSourcePosition > ( "ApplicationPushSourcePosition" );
	cf->registerComponent< ApplicationPushSourcePositionList > ( "ApplicationPushSourcePositionList" );
	cf->registerComponent< ApplicationPushSourcePosition2D > ( "ApplicationPushSourcePosition2" );	
	cf->registerComponent< ApplicationPushSourcePosition2DList > ( "ApplicationPushSourcePositionList2" );
	cf->registerComponent< ApplicationPushSourcePose > ( "ApplicationPushSourcePose" );
	cf->registerComponent< ApplicationPushSourcePoseList > ( "ApplicationPushSourcePoseList" );
	cf->registerComponent< ApplicationPushSourcePoseList > ( "ApplicationPushSourceMatrix3x3" );
	cf->registerComponent< ApplicationPushSourcePoseList > ( "ApplicationPushSourceVector4" );
}

} } // namespace Ubitrack::Components

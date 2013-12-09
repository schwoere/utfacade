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
 * Defines an observer interface for the ubitrack dataflow network.
 *
 * @author Daniel Pustka <daniel.pustka@in.tum.de>
 */
#ifndef __UBITRACK_FACADE_DATAFLOWOBSERVER_H_INCLUDED__
#define __UBITRACK_FACADE_DATAFLOWOBSERVER_H_INCLUDED__

#include <string>
#include <utGraph/UTQLSubgraph.h>

namespace Ubitrack { namespace Facade {

/**
 * Implement this interface if you want to be notified of changes in the data flow network
 */
class DataflowObserver
{
public:
	/**
	 * The facade calls this method whenever a new component is added to the data flow network.
	 *
	 * @param sPatternName name of the pattern, can be used to identify queries
	 * @param sComponentName id of the pattern, can be used to identify the data flow component
	 * @param pattern reference to the full UTQL subgraph if more information is needed
	 */
	virtual void notifyAddComponent( const std::string& sPatternName, const std::string& sComponentName, 
		const Graph::UTQLSubgraph& pattern ) = 0;

	/**
	 * The facade calls this method whenever a component is removed from the data flow network.
	 *
	 * @param sPatternName name of the pattern, can be used to identify queries
	 * @param sComponentName id of the pattern, can be used to identify the data flow component
	 */
	virtual void notifyDeleteComponent( const std::string& sPatternName, const std::string& sComponentName ) = 0;

	/** virtual destructor */
	virtual ~DataflowObserver()
	{}	
};

} } // Ubitrack::Facade

#endif


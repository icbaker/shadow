/*
 * The Shadow Simulator
 *
 * Copyright (c) 2010-2012 Rob Jansen <jansen@cs.umn.edu>
 *
 * This file is part of Shadow.
 *
 * Shadow is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Shadow is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Shadow.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SHD_CREATE_NODES_H_
#define SHD_CREATE_NODES_H_

#include "shadow.h"

typedef struct _CreateNodesAction CreateNodesAction;

CreateNodesAction* createnodes_new(GString* name, GString* cluster,
		guint64 bandwidthdown, guint64 bandwidthup, guint64 quantity, guint64 cpuFrequency,
		guint64 heartbeatIntervalSeconds, GString* heartbeatLogLevelString,
		GString* logLevelString, GString* logPcapString, GString* pcapDirString);
void createnodes_addApplication(CreateNodesAction* action, GString* pluginName,
		GString* arguments, guint64 launchtime);
void createnodes_run(CreateNodesAction* action);
void createnodes_free(CreateNodesAction* action);

#endif /* SHD_CREATE_NODES_H_ */

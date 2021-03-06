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

#include "shadow.h"

struct _PacketArrivedEvent {
	Event super;
	Packet* packet;
	MAGIC_DECLARE;
};

EventFunctionTable packetarrived_functions = {
	(EventRunFunc) packetarrived_run,
	(EventFreeFunc) packetarrived_free,
	MAGIC_VALUE
};

PacketArrivedEvent* packetarrived_new(Packet* packet) {
	PacketArrivedEvent* event = g_new0(PacketArrivedEvent, 1);
	MAGIC_INIT(event);

	shadowevent_init(&(event->super), &packetarrived_functions);

	packet_ref(packet);
	event->packet = packet;

	return event;
}

void packetarrived_run(PacketArrivedEvent* event, Node* node) {
	MAGIC_ASSERT(event);

	debug("event started");

	in_addr_t ip = packet_getDestinationIP(event->packet);
	NetworkInterface* interface = node_lookupInterface(node, ip);
	networkinterface_packetArrived(interface, event->packet);

	debug("event finished");
}

void packetarrived_free(PacketArrivedEvent* event) {
	MAGIC_ASSERT(event);

	packet_unref(event->packet);

	MAGIC_CLEAR(event);
	g_free(event);
}

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

struct _Network {
	GQuark id;
	GMutex* lock;
	GHashTable *linksByCluster;
	GHashTable *linksByNode;

	guint64 bandwidthdown;
	guint64 bandwidthup;
	gdouble packetloss;
	MAGIC_DECLARE;
};

Network* network_new(GQuark id, guint64 bandwidthdown, guint64 bandwidthup, gdouble packetloss) {
	Network* network = g_new0(Network, 1);
	MAGIC_INIT(network);

	network->id = id;
	network->bandwidthdown = bandwidthdown;
	network->bandwidthup = bandwidthup;
	network->packetloss = packetloss;

	network->linksByCluster = g_hash_table_new_full(g_int_hash, g_int_equal, NULL, NULL);
	network->linksByNode = g_hash_table_new_full(g_int_hash, g_int_equal, NULL, NULL);

	network->lock = g_mutex_new();

	return network;
}

void network_free(gpointer data) {
	Network* network = data;
	MAGIC_ASSERT(network);

	g_mutex_lock(network->lock);

	g_hash_table_destroy(network->linksByCluster);
	g_hash_table_destroy(network->linksByNode);

	g_mutex_unlock(network->lock);
	g_mutex_free(network->lock);

	MAGIC_CLEAR(network);
	g_free(network);
}

GQuark* network_getIDReference(Network* network) {
	MAGIC_ASSERT(network);
	return &(network->id);
}

guint64 network_getBandwidthUp(Network* network) {
	MAGIC_ASSERT(network);
	return network->bandwidthup;
}

guint64 network_getBandwidthDown(Network* network) {
	MAGIC_ASSERT(network);
	return network->bandwidthdown;
}

gint network_compare(gconstpointer a, gconstpointer b, gpointer user_data) {
	const Network* na = a;
	const Network* nb = b;
	MAGIC_ASSERT(na);
	MAGIC_ASSERT(nb);
	return na->id > nb->id ? +1 : na->id == nb->id ? 0 : -1;
}

gboolean network_isEqual(Network* a, Network* b) {
	if(a == NULL && b == NULL) {
		return TRUE;
	} else if(a == NULL || b == NULL) {
		return FALSE;
	} else {
		return network_compare(a, b, NULL) == 0;
	}
}

void network_addLink(Network* network, gpointer link) {
	MAGIC_ASSERT(network);

	Network* destination = link_getDestinationNetwork((Link*)link);
	GList *links = g_hash_table_lookup(network->linksByCluster, &(destination->id));
	links = g_list_append(links, link);
	g_hash_table_replace(network->linksByCluster, &(destination->id), links);
}

Link* network_getLink(Network *network, in_addr_t sourceIP, in_addr_t destinationIP) {
	MAGIC_ASSERT(network);

	/* FIXME this is not thread-safe!
	 * we should rewrite this functionality so that all of the hashtables are
	 * precomputed before we start processing nodes to avoid thread collisions.
	 */
	g_mutex_lock(network->lock);

	gint *key;

	/* check to see if we already have hash table of links for source */
	GHashTable *nodeLinks = g_hash_table_lookup(network->linksByNode, &sourceIP);
	if(!nodeLinks) {
		nodeLinks = g_hash_table_new_full(g_int_hash, g_int_equal, NULL, NULL);
		key = g_new0(gint, 1);
		*key = sourceIP;
		g_hash_table_insert(network->linksByNode, key, nodeLinks);
	}

	/* lookup link to destination IP */
	Link *link = g_hash_table_lookup(nodeLinks, &destinationIP);
	if(!link) {
		Internetwork* internet = worker_getInternet();
		Network *destinationNetwork = internetwork_lookupNetwork(internet, destinationIP);

		/* get list of possible links to destination network */
		GList *links = g_hash_table_lookup(network->linksByCluster, &(destinationNetwork->id));
		if(!links) {
			g_mutex_unlock(network->lock);
			return NULL;
		}

		/* randomly select link to assign between the nodes */
		Random* random = node_getRandom(worker_getPrivate()->cached_node);
		gdouble randomDouble = random_nextDouble(random);
		guint length = g_list_length(links);

		guint n = (guint)(((gdouble)length - 1) * randomDouble);
		g_assert((n >= 0) && (n < length));

		/* get random link from list */
		link = g_list_nth_data(links, n);

		guint64 latency, jitter;
		guint64 min,q1,mean,q3,max;

		latency = link_getLatency(link);
		jitter = link_getJitter(link);
		link_getLatencyMetrics(link, &min, &q1, &mean, &q3, &max);
		struct in_addr addr;
		gchar *srcIP = g_strdup(inet_ntoa((struct in_addr){sourceIP}));
		gchar *dstIP = g_strdup(inet_ntoa((struct in_addr){destinationIP}));
		message("link for connection [%s] %s -> %s [%s] chosen: latency=%d jitter=%d metrics=%d %d %d %d %d",
				g_quark_to_string(network->id), srcIP, dstIP, g_quark_to_string(destinationNetwork->id),
				latency, jitter, min, q1, mean, q3, max);
		g_free(srcIP);
		g_free(dstIP);

		/* insert link into source network */
		key = g_new0(gint, 1);
		*key = destinationIP;

		g_hash_table_insert(nodeLinks, key, link);

		/* insert link into destination network */
		nodeLinks = g_hash_table_lookup(destinationNetwork->linksByNode, &destinationIP);
		if(!nodeLinks) {
			nodeLinks = g_hash_table_new_full(g_int_hash, g_int_equal, NULL, NULL);
			key = g_new0(gint, 1);
			*key = destinationIP;
			g_hash_table_insert(destinationNetwork->linksByNode, key, nodeLinks);
		}

		key = g_new0(gint, 1);
		*key = sourceIP;
		g_hash_table_insert(nodeLinks, key, link);
	}

	g_mutex_unlock(network->lock);

	return link;
}

gdouble network_getLinkReliability(in_addr_t sourceIP, in_addr_t destinationIP) {
	Internetwork* internet = worker_getInternet();
	Network *sourceNetwork = internetwork_lookupNetwork(internet, sourceIP);
	Network *destinationNetwork = internetwork_lookupNetwork(internet, destinationIP);

	Link *link = network_getLink(sourceNetwork, sourceIP, destinationIP);
	if(link) {
		/* there are three chances to drop a packet here:
		 * p1 : loss rate from source-node to the source-cluster
		 * p2 : loss rate on the link between source-cluster and destination-cluster
		 * p3 : loss rate from destination-cluster to destination-node
		 *
		 * The reliability is then the combination of the probability
		 * that its not dropped in each case:
		 * P = ((1-p1)(1-p2)(1-p3))
		 */
		gdouble p1 = sourceNetwork->packetloss;
		gdouble p2 = link_getPacketLoss(link);
		gdouble p3 = destinationNetwork->packetloss;
		gdouble P = (1.0-p1) * (1.0-p2) * (1.0-p3);
		return P;
	} else {
		critical("unable to find link between networks '%s' and '%s'. Check XML file for errors.",
				g_quark_to_string(sourceNetwork->id), g_quark_to_string(destinationNetwork->id));
		return G_MINDOUBLE;
	}
}

gdouble network_getLinkLatency(in_addr_t sourceIP, in_addr_t destinationIP, gdouble percentile) {
	Internetwork* internet = worker_getInternet();
	Network *sourceNetwork = internetwork_lookupNetwork(internet, sourceIP);
	Network *destinationNetwork = internetwork_lookupNetwork(internet, destinationIP);

	Link *link = network_getLink(sourceNetwork, sourceIP, destinationIP);

	if(link) {
		return link_computeDelay(link, percentile);
	} else {
		critical("unable to find link between networks '%s' and '%s'. Check XML file for errors.",
				g_quark_to_string(sourceNetwork->id), g_quark_to_string(destinationNetwork->id));
		return G_MAXDOUBLE;
	}
}

gdouble network_sampleLinkLatency(in_addr_t sourceIP, in_addr_t destinationIP) {
	Random* random = node_getRandom(worker_getPrivate()->cached_node);
	gdouble percentile = random_nextDouble(random);
	return network_getLinkLatency(sourceIP, destinationIP, percentile);
}

void network_scheduleRetransmit(Network* network, Packet* packet) {
	MAGIC_ASSERT(network);

	// FIXME make sure loopback packets dont get here!!
	// FIXME network_isEqual is wrong!

	/* source should retransmit. use latency to approximate RTT for 'retransmit timer' */
	Internetwork* internet = worker_getInternet();

	in_addr_t sourceIP = packet_getSourceIP(packet);
	Network* sourceNetwork = internetwork_lookupNetwork(internet, sourceIP);
	in_addr_t destinationIP = packet_getDestinationIP(packet);
	Network* destinationNetwork = internetwork_lookupNetwork(internet, destinationIP);

	gdouble latency = 0;
	if(network_isEqual(network, sourceNetwork)) {
		/* RTT is two link latencies */
		latency += network_sampleLinkLatency(sourceIP, destinationIP);
		latency += network_sampleLinkLatency(destinationIP, sourceIP);
	} else {
		/* latency to destination already incurred, RTT is latency back to source */
		latency += network_sampleLinkLatency(destinationIP, sourceIP);
	}

	SimulationTime delay = (SimulationTime) floor(latency * SIMTIME_ONE_MILLISECOND);
	PacketDroppedEvent* event = packetdropped_new(packet);
	worker_scheduleEvent((Event*)event, delay, (GQuark)sourceIP);
}

void network_schedulePacket(Network* sourceNetwork, Packet* packet) {
	MAGIC_ASSERT(sourceNetwork);

	Internetwork* internet = worker_getInternet();
	in_addr_t sourceIP = packet_getSourceIP(packet);
	in_addr_t destinationIP = packet_getDestinationIP(packet);

	/* first thing to check is if network reliability forces us to 'drop'
	 * the packet. if so, get out of dodge doing as little as possible.
	 */
	gdouble reliability = network_getLinkReliability(sourceIP, destinationIP);
	Random* random = node_getRandom(worker_getPrivate()->cached_node);
	gdouble chance = random_nextDouble(random);
	if(chance > reliability){
		/* sender side is scheduling packets, but we are simulating
		 * the packet being dropped between sender and receiver, so
		 * it will need to be retransmitted */
		network_scheduleRetransmit(sourceNetwork, packet);
	} else {
		/* packet will make it through, find latency */
		gdouble latency = network_sampleLinkLatency(sourceIP, destinationIP);
		SimulationTime delay = (SimulationTime) floor(latency * SIMTIME_ONE_MILLISECOND);

		PacketArrivedEvent* event = packetarrived_new(packet);
		worker_scheduleEvent((Event*)event, delay, (GQuark)destinationIP);
	}
}

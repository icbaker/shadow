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

static gchar* _logging_getLogLevelString(GLogLevelFlags log_level) {
	gchar* levels;
	switch (log_level) {
		case G_LOG_LEVEL_ERROR: {
			levels = "error";
			break;
		}
		case G_LOG_LEVEL_CRITICAL: {
			levels = "critical";
			break;
		}

		case G_LOG_LEVEL_WARNING: {
			levels = "warning";
			break;
		}

		case G_LOG_LEVEL_MESSAGE: {
			levels = "message";
			break;
		}

		case G_LOG_LEVEL_INFO: {
			levels = "info";
			break;
		}

		case G_LOG_LEVEL_DEBUG: {
			levels = "debug";
			break;
		}

		default: {
			levels = "default";
			break;
		}
	}
	return levels;
}

static const gchar* _logging_getLogDomainString(const gchar *log_domain) {
	const gchar* domains = log_domain != NULL ? log_domain : "shadow";
	return domains;
}

static gboolean _logging_messageIsFiltered(const gchar *msgLogDomain, GLogLevelFlags msgLogLevel) {
	Worker* w = worker_getPrivate();

	/* check the local node log level first */
	gboolean isNodeLevelSet = FALSE;
	if(w->cached_node) {
		GLogLevelFlags nodeLevel = node_getLogLevel(w->cached_node);
		if(nodeLevel) {
			isNodeLevelSet = TRUE;
			if(msgLogLevel > nodeLevel) {
				return TRUE;
			}
		}
	}

	/* only check the global config if the node didnt have a local setting */
	if(!isNodeLevelSet && w->cached_engine) {
		Configuration* c = engine_getConfig(w->cached_engine);
		if(c && (msgLogLevel > configuration_getLogLevel(c))) {
			return TRUE;
		}
	}

	return FALSE;
}

/* this func is called whenever g_logv is called, not just in our log code */
void logging_handleLog(const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data) {
	/* GLogLevelFlags* configuredLogLevel = user_data; */

	/* check again if the message should be filtered */
	if(_logging_messageIsFiltered(log_domain, log_level)) {
		return;
	}

	gulong hours, minutes, seconds, microseconds;
	gulong elapsed = (gulong) g_timer_elapsed(engine_getRunTimer(shadow_engine), &microseconds);
	hours = elapsed / 3600;
	elapsed %= 3600;
	minutes = elapsed / 60;
	seconds = elapsed % 60;

	g_print("%lu:%lu:%lu:%06lu %s\n", hours, minutes, seconds, microseconds, message);

	if(log_level & G_LOG_LEVEL_ERROR) {
		g_print("\t**aborting**\n");
	}
}

void logging_logv(const gchar *msgLogDomain, GLogLevelFlags msgLogLevel,
		const gchar* functionName, const gchar *format, va_list vargs) {
	/* this is called by worker threads, so we have access to worker */
	Worker* w = worker_getPrivate();

	/* see if we can avoid some work because the message is filtered anyway */
	if(_logging_messageIsFiltered(msgLogDomain, msgLogLevel)) {
		return;
	}

	/* format the simulation time if we are running an event */
	GString* clockStringBuffer = g_string_new("");
	if(w->clock_now != SIMTIME_INVALID) {
		SimulationTime hours, minutes, seconds, remainder;
		remainder = w->clock_now;

		hours = remainder / SIMTIME_ONE_HOUR;
		remainder %= SIMTIME_ONE_HOUR;
		minutes = remainder / SIMTIME_ONE_MINUTE;
		remainder %= SIMTIME_ONE_MINUTE;
		seconds = remainder / SIMTIME_ONE_SECOND;
		remainder %= SIMTIME_ONE_SECOND;

		g_string_printf(clockStringBuffer, "%lu:%lu:%lu:%09lu", hours, minutes, seconds, remainder);
	} else {
		g_string_printf(clockStringBuffer, "n/a");
	}

	/* we'll need to free clockString later */
	gchar* clockString = g_string_free(clockStringBuffer, FALSE);

	/* node identifier, if we are running a node
	 * dont free this since we dont own the ip address string */
	GString* nodeStringBuffer = g_string_new("");
	if(w->cached_node) {
		g_string_printf(nodeStringBuffer, "%s-%s", node_getName(w->cached_node), node_getDefaultIPName(w->cached_node));
	} else {
		g_string_printf(nodeStringBuffer, "n/a");
	}
	gchar* nodeString = g_string_free(nodeStringBuffer, FALSE);

	/* the function name - no need to free this */
	const gchar* functionString = !functionName ? "n/a" : functionName;

	GString* newLogFormatBuffer = g_string_new(NULL);
	g_string_printf(newLogFormatBuffer, "[thread-%i] %s [%s-%s] [%s] [%s] %s",
			w->thread_id,
			clockString,
			_logging_getLogDomainString(msgLogDomain),
			_logging_getLogLevelString(msgLogLevel),
			nodeString,
			functionString,
			format
			);

	/* get the new format out of our string buffer and log it */
	gchar* newLogFormat = g_string_free(newLogFormatBuffer, FALSE);
	g_logv(msgLogDomain, msgLogLevel, newLogFormat, vargs);

	/* cleanup */
	g_free(newLogFormat);
	g_free(clockString);
	g_free(nodeString);
}

void logging_log(const gchar *log_domain, GLogLevelFlags log_level, const gchar* functionName, const gchar *format, ...) {
	va_list vargs;
	va_start(vargs, format);

	logging_logv(log_domain, log_level, functionName, format, vargs);

	va_end(vargs);
}

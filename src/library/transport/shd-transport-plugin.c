#include "shd-transport.h"

/* my global structure to hold all variable, node-specific application state.
 * the name must not collide with other loaded modules globals. */
Transport transportstate;

/* function table for Shadow so it knows how to call us */
PluginFunctionTable transport_pluginFunctions = {
    &transportplugin_new, &transportplugin_free, &transportplugin_ready,
};

void __shadow_plugin_init__(ShadowlibFunctionTable* shadowlibFuncs) {
    g_assert(shadowlibFuncs);

    /* start out with cleared state */
    memset(&transportstate, 0, sizeof(Transport));

    /* save the functions Shadow makes available to us */
    transportstate.shadowlibFuncs = *shadowlibFuncs;

    /*
     * tell shadow which of our functions it can use to notify our plugin,
     * and allow it to track our state for each instance of this plugin
     *
     * we 'register' our function table, and 1 variable.
     */
    gboolean success = transportstate.shadowlibFuncs.registerPlugin(&transport_pluginFunctions, 1, sizeof(Transport), &transportstate);

    /* we log through Shadow by using the log function it supplied to us */
    if(success) {
        transportstate.shadowlibFuncs.log(G_LOG_LEVEL_MESSAGE, __FUNCTION__,
                "successfully registered transport plug-in state");
    } else {
        transportstate.shadowlibFuncs.log(G_LOG_LEVEL_CRITICAL, __FUNCTION__,
                "error registering transport plug-in state");
    }
}

void transportplugin_new(int argc, char* argv[]) {
    transportstate.shadowlibFuncs.log(G_LOG_LEVEL_DEBUG, __FUNCTION__,
            "transportplugin_new called");

    const char* USAGE = "Transport USAGE: 'utp client serverIP', 'utp server', 'utp loopback', 'utp socketpair',"
            "** clients and servers must be paired together, but loopback, socketpair,"
            "and pipe modes stand on their own.";

    /* 0 is the plugin name, 1 is the protocol */
    if(argc < 2) {
        transportstate.shadowlibFuncs.log(G_LOG_LEVEL_CRITICAL, __FUNCTION__, "%s", USAGE);
        return;
    }

    char* protocol = argv[1];

    gboolean isError = TRUE;

    /* check for the protocol option and create the correct application state */
    if(g_strncasecmp(protocol, "utp", 3) == 0)
    {
        transportstate.protocol = TRANSPORTP_UTP;
        transportstate.tutp = transportutp_new(transportstate.shadowlibFuncs.log, argc - 2, &argv[2]);
        isError = (transportstate.tutp == NULL) ? TRUE : FALSE;
    }

    if(isError) {
        /* unknown argument for protocol, log usage information through Shadow */
        transportstate.shadowlibFuncs.log(G_LOG_LEVEL_CRITICAL, __FUNCTION__, "%s", USAGE);
    }
}

void transportplugin_free() {
    transportstate.shadowlibFuncs.log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "echoplugin_free called");

    /* call the correct version depending on what we are running */
    switch(transportstate.protocol) {
        case TRANSPORTP_UTP: {
            transportutp_free(transportstate.tutp);
            break;
        }

        default: {
            transportstate.shadowlibFuncs.log(G_LOG_LEVEL_CRITICAL, __FUNCTION__,
                    "unknown protocol in transportplugin_free");
            break;
        }
    }
}

void transportplugin_ready() {
    transportstate.shadowlibFuncs.log(G_LOG_LEVEL_DEBUG, __FUNCTION__, "echoplugin_ready called");

    /* call the correct version depending on what we are running */
    switch(transportstate.protocol) {
        case TRANSPORTP_UTP: {
            transportutp_ready(transportstate.tutp);
            break;
        }

        default: {
            transportstate.shadowlibFuncs.log(G_LOG_LEVEL_CRITICAL, __FUNCTION__,
                    "unknown protocol in transportplugin_ready");
            break;
        }
    }
}

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
        echostate.shadowlibFuncs.log(G_LOG_LEVEL_CRITICAL, __FUNCTION__,
                "error registering transport plug-in state");
    }
}

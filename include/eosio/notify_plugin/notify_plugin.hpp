/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#pragma once
#include <appbase/application.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>

namespace eosio {

using namespace appbase;
using notify_plugin_impl_ptr = std::shared_ptr<class notify_plugin_impl>;

/**
 *  This is a template plugin, intended to serve as a starting point for making new plugins
 */
class notify_plugin : public appbase::plugin<notify_plugin> {
public:
    notify_plugin();
   virtual ~notify_plugin();
 
   APPBASE_PLUGIN_REQUIRES((chain_plugin))
   virtual void set_program_options(options_description&, options_description& cfg) override;
 
   void plugin_initialize(const variables_map& options);
   void plugin_startup();
   void plugin_shutdown();

private:
    notify_plugin_impl_ptr my;
};

}

/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <eosio/notify_plugin/notify_plugin.hpp>

#include <eosio/chain/controller.hpp>
#include <eosio/chain/trace.hpp>
#include <eosio/chain_plugin/chain_plugin.hpp>

#include <fc/io/json.hpp>
#include <fc/log/logger.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/signals2/connection.hpp>
#include <eosio/chain/types.hpp>


namespace eosio {
    using namespace fc;
   static appbase::abstract_plugin& _notify_plugin = app().register_plugin<notify_plugin>();

    struct transfer_args {
        account_name  from;
        account_name  to;
        asset         quantity;
        string        memo;
    };
//FC_REFLECT(transfer_args, (from)(to)(quantity)(memo))

//    void runCommand(const std::string& strCommand)
//    {
//        int nErr = ::system(strCommand.c_str());
//        if (nErr)
//            wlog("runCommand error: system(%s) returned %d\n", strCommand, nErr);
//    }
class notify_plugin_impl {
   public:
    std::vector<account_name> filter_accounts;
    string shell_cmd;

    void notify_tx(const chain::transaction_trace_ptr& txp) {
        bool need_notify = false;
        for(auto& at :txp->action_traces){
            account_name to = at.receipt.receiver;
            auto it = find(this->filter_accounts.begin(), this->filter_accounts.end(), to);
            if (it != this->filter_accounts.end()) {
                need_notify = true;
                break;
            }
            for(auto& act : at.act.authorization) {
               account_name from = act.actor;
                auto it = find(this->filter_accounts.begin(), this->filter_accounts.end(), from);
                if (it != this->filter_accounts.end()) {
                    need_notify = true;
                    break;
                }
            }
            if (need_notify) {
                break;
            }
        }
        if (need_notify) {
            auto id = txp->id;
            //send notify
            boost::replace_all(shell_cmd, "%s", id.str());
            boost::thread t([&](string cmd) {
                    int nErr = ::system(cmd.c_str());
                    if (nErr) {
//                        elog("runCommand error: system(%s) returned %d\n", cmd, nErr);
                        elog("runCommand error: system(${cmd}) returned ${code}", ("cmd",cmd)("code",nErr));
                    }
            }, shell_cmd); // thread runs free
        }

    }
    void reset() {
        this->filter_accounts.clear();
        this->shell_cmd = nullptr;
    }
};

notify_plugin::notify_plugin():my(new notify_plugin_impl()){}
notify_plugin::~notify_plugin(){}

void notify_plugin::set_program_options(options_description& cli, options_description& cfg) {
   cfg.add_options()
         ("tx-notify-filter-account-name", bpo::value<vector<string> >()->composing(),
          "transaction filter by account name,eg: tx-notify-filter-account-name = eosio_account")
         ;
    cfg.add_options()("tx-notify-shell-cmd", bpo::value<std::string>(), "curl http://domain/tx/%s");
}

void notify_plugin::plugin_initialize(const variables_map& options) {
   try {

      if( options.count( "notify-filter-account-name" )) {
         // Handle the option
          std::vector<std::string> fo = options.at( "notify-filter-account-name" ).as< std::vector<std::string>>();
          for (std::string& f: fo) {
            my->filter_accounts.push_back(account_name(f));
          }
      }


      my->shell_cmd = options.at("tx-notify-shell-cmd").as<string>();


       chain_plugin* chain_plug = app().find_plugin<chain_plugin>();
       auto& chain = chain_plug->chain();
       chain.applied_transaction.connect([&](const chain::transaction_trace_ptr& ptr){
           my->notify_tx(ptr);
       });
//       chain.accepted_transaction.connect( [&]( const chain::transaction_metadata_ptr& t ) {
//
//           my->notify_tx( t );
//       } );
   }
   FC_LOG_AND_RETHROW()
}

void notify_plugin::plugin_startup() {
   // Make the magic happen
}

void notify_plugin::plugin_shutdown() {
   // OK, that's enough magic
}

}

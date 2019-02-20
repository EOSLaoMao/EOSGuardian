#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <eosio.system/eosio.system.hpp>
#include <eosio.token/eosio.token.hpp>
#include "../include/eosguardian/eosguardian.hpp"
#include "validation.cpp"
#include "utils.cpp"

using namespace validation;
using namespace utils;

class [[eosio::contract]] eosguardian : contract {
public:
    using contract::contract;

    [[eosio::action]]
    void setsettings(asset cap_total, asset cap_tx, uint64_t duration) {

        require_auth(_self);

        eosio_assert(cap_total.symbol == EOS_SYMBOL, "only support EOS as cap");
        eosio_assert(cap_tx.symbol == EOS_SYMBOL, "only support EOS as cap");

        settings_table s(_self, _self.value);
        auto itr = s.find(_self.value);
        
        if(itr == s.end()) {
            s.emplace(_self, [&](auto &i) {
                i.account = _self;
                i.cap_total = cap_total;
                i.cap_tx = cap_tx;
                i.duration = duration;
                i.created_at = now();
                i.updated_at = now();
            });
        } else {
            s.modify(itr, _self, [&](auto &i) {
                i.cap_total = cap_total;
                i.cap_tx = cap_tx;
                i.duration = duration;
                i.updated_at = now();
            });
        }
    }

    [[eosio::action]]
    void setwhitelist(name account, asset cap_total, asset cap_tx, uint64_t duration) {

        require_auth(_self);

        eosio_assert(cap_total.symbol == EOS_SYMBOL, "only support EOS as cap");
        eosio_assert(cap_tx.symbol == EOS_SYMBOL, "only support EOS as cap");

        whitelist_table w(_self, _self.value);
        auto itr = w.find(account.value);

        if(itr == w.end()) {
            w.emplace(_self, [&](auto &i) {
                i.account = account;
                i.cap_total = cap_total;
                i.cap_tx = cap_tx;
                i.duration = duration;
                i.created_at = now();
                i.updated_at = now();
            });
        } else {
            w.modify(itr, _self, [&](auto &i) {
                i.cap_total = cap_total;
                i.cap_tx = cap_tx;
                i.duration = duration;
                i.updated_at = now();
            });
        }
    }

    [[eosio::action]]
    void delwhitelist(name account){

        require_auth(_self);

        whitelist_table w(_self, _self.value);
        auto itr = w.find(account.value);
        eosio_assert(itr != w.end(), "account not found in whitelist");
        w.erase(itr);
    }

    [[eosio::action]]
    void setblacklist(name account){

        require_auth(_self);

        blacklist_table b(_self, _self.value);
        auto itr = b.find(account.value);
        eosio_assert(itr == b.end(), "account already exist in blacklist");
        
        b.emplace(_self, [&](auto &i) {
            i.account = account;
            i.created_at = now();
        });
    }

    [[eosio::action]]
    void delblacklist(name account){

        require_auth(_self);

        blacklist_table b(_self, _self.value);
        auto itr = b.find(account.value);
        eosio_assert(itr != b.end(), "account not found in blacklist");
        b.erase(itr);
    }

    void safedelegate(name to, asset net_weight, asset cpu_weight) {

        require_auth(_self);

        eosio_assert(net_weight.symbol == EOS_SYMBOL, "only support EOS");
        eosio_assert(cpu_weight.symbol == EOS_SYMBOL, "only support EOS");
    }

    void safetransfer(name to, asset quantity, string memo){

        require_auth(_self);

        eosio_assert(quantity.symbol == EOS_SYMBOL, "only support EOS");

        validate_blacklist(_self, to);
        validate_transfer(_self, to, quantity);

        add_txrecord(_self, to, quantity, memo);
    }
};

extern "C" {
    void apply(uint64_t receiver, uint64_t code, uint64_t action) {
        print(name(receiver));
        print(name(code));
        print(name(action));

        if (code == name("ifttt").value) {
            if (action == name("safetransfer").value) {
                print("prepare to execute safetransfer");
                eosio::execute_action(name(code), name(action), &eosguardian::safetransfer);
            } else if (action == name("safedelegate").value) {
                print("prepare to execute safedelegate");
                eosio::execute_action(name(code), name(action), &eosguardian::safedelegate);
            }
            
        } else if (receiver == code) {
            switch(action) {
                EOSIO_DISPATCH_HELPER(eosguardian,
                    (setsettings)
                    (setwhitelist)
                    (delwhitelist)
                    (setblacklist)
                    (delblacklist)
                )
            };
        }
    }
}

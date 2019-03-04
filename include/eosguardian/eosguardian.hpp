/**
 *  @file bankofstaked.hpp
 */
#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/multi_index.hpp>

#define EOS_SYMBOL symbol("EOS", 4)

using namespace eosio;

namespace guardian
{
    static const uint64_t USER_STATUS_EXPIRED = 0;
    static const uint64_t USER_STATUS_EFFECTIVE = 1;
    //users table
    struct [[eosio::table, eosio::contract("eosguardian")]] users {
        name account;
        uint64_t duration; // subscription duration in minutes
        uint64_t created_at; // unix time, in seconds
        uint64_t updated_at; // unix time, in seconds

        uint64_t primary_key() const { return account.value; }
        EOSLIB_SERIALIZE(users, (account)(duration)(created_at)(updated_at));
    };
    typedef multi_index<"users"_n, users> users_table;

    struct [[eosio::table, eosio::contract("eosguardian")]] settings {
        name account;
        asset cap_total{0, EOS_SYMBOL}; // max cap in given duration
        asset cap_tx{0, EOS_SYMBOL}; // max cap per transfer
        uint64_t duration; // cap_total duration in minutes
        uint64_t created_at; // unix time, in seconds
        uint64_t updated_at; // unix time, in seconds

        uint64_t primary_key() const { return account.value; }
        EOSLIB_SERIALIZE(settings, (account)(cap_total)(cap_tx)(duration)(created_at)(updated_at));
    };
    typedef multi_index<"settings"_n, settings> settings_table;

    struct [[eosio::table, eosio::contract("eosguardian")]] blacklist {
        name account;
        uint64_t created_at; // unix time, in seconds

        uint64_t primary_key() const { return account.value; }
        EOSLIB_SERIALIZE(blacklist, (account)(created_at));
    };
    typedef multi_index<"blacklist"_n, blacklist> blacklist_table;

    struct [[eosio::table, eosio::contract("eosguardian")]] whitelist {
        name account;
        asset cap_total{0, EOS_SYMBOL}; // max cap in given duration
        asset cap_tx{0, EOS_SYMBOL}; // max cap per transfer
        uint64_t duration; // cap_total duration in minutes
        uint64_t updated_at; // unix time, in seconds
        uint64_t created_at; // unix time, in seconds

        uint64_t primary_key() const { return account.value; }
        EOSLIB_SERIALIZE(whitelist, (account)(cap_total)(cap_tx)(duration)(updated_at)(created_at));
    };
    typedef multi_index<"whitelist"_n, whitelist> whitelist_table;

    struct [[eosio::table, eosio::contract("eosguardian")]] txrecord {
        uint64_t id; // primary key
        name to;
        asset quantity{0, EOS_SYMBOL}; // transfer quantity
        uint64_t created_at; // unix time, in seconds

        uint64_t primary_key() const { return id; }
        uint64_t get_to() const { return to.value; }
        EOSLIB_SERIALIZE(txrecord, (id)(to)(quantity)(created_at));
    };
    typedef multi_index<"txrecord"_n, txrecord, indexed_by<"to"_n, const_mem_fun<txrecord, uint64_t, &txrecord::get_to>>> txrecord_table;

}// namespace guardian

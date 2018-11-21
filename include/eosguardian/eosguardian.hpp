/**
 *  @file bankofstaked.hpp
 */
#include <eosiolib/asset.hpp>
#include <eosiolib/eosio.hpp>
#include <eosiolib/multi_index.hpp>

#define EOS_SYMBOL S(4, EOS)

using namespace eosio;

namespace guardian
{

    // @abi table settings i64
    struct settings
    {
        account_name account;
        asset cap_total; // max cap in given duration
        asset cap_tx; // max cap per transfer
        uint64_t duration; // cap_total duration
        uint64_t created_at; // unix time, in seconds
        uint64_t updated_at; // unix time, in seconds

        account_name primary_key() const { return account; }
        EOSLIB_SERIALIZE(settings, (account)(cap_total)(cap_tx)(duration)(created_at)(updated_at));
    };
    typedef multi_index<N(settings), settings> settings_table;

    // @abi table blacklist i64
    struct blacklist
    {
        account_name account;
        uint64_t created_at; // unix time, in seconds

        account_name primary_key() const { return account; }
        EOSLIB_SERIALIZE(blacklist, (account)(created_at));
    };
    typedef multi_index<N(blacklist), blacklist> blacklist_table;

    // @abi table whitelist i64
    struct whitelist
    {
        account_name account;
        asset cap_total; // max cap in given duration
        asset cap_tx; // max cap per transfer
        uint64_t duration; // cap_total duration
        uint64_t updated_at; // unix time, in seconds
        uint64_t created_at; // unix time, in seconds

        account_name primary_key() const { return account; }
        EOSLIB_SERIALIZE(whitelist, (account)(cap_total)(cap_tx)(duration)(updated_at)(created_at));
    };
    typedef multi_index<N(whitelist), whitelist> whitelist_table;

    // @abi table txrecord i64
    struct txrecord
    {
        uint64_t id; // primary key
        account_name to;
        asset quantity; // transfer quantity
        uint64_t created_at; // unix time, in seconds

        uint64_t primary_key() const { return id; }
        account_name get_to() const { return to; }
        EOSLIB_SERIALIZE(txrecord, (id)(to)(quantity)(created_at));
    };
    typedef multi_index<N(txrecord), txrecord,
                    indexed_by<N(to), const_mem_fun<txrecord, account_name, &txrecord::get_to>>> txrecord_table;
            
}// namespace guardian

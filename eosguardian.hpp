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
  asset cap; // max cap in given duration
  uint64_t duration; // cap duration
  uint64_t created_at; // unix time, in seconds
  uint64_t updated_at; // unix time, in seconds

  account_name primary_key() const { return account; }
  EOSLIB_SERIALIZE(settings, (account)(cap)(duration)(created_at)(updated_at));
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
  asset cap; // max cap in given duration
  uint64_t duration; // cap duration
  uint64_t updated_at; // unix time, in seconds
  uint64_t created_at; // unix time, in seconds

  account_name primary_key() const { return account; }
  EOSLIB_SERIALIZE(whitelist, (account)(cap)(duration)(updated_at)(created_at));
};
typedef multi_index<N(whitelist), whitelist> whitelist_table;

// @abi table transrecord i64
struct transrecord
{
  account_name to;
  asset amount; // transfer amount
  uint64_t created_at; // unix time, in seconds

  account_name primary_key() const { return to; }
  EOSLIB_SERIALIZE(transrecord, (to)(amount)(created_at));
};
typedef multi_index<N(transrecord), transrecord> transrecord_table;



}// namespace guardian



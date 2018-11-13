using namespace eosio;
using namespace eosiosystem;
using namespace guardian;
using std::string;

namespace validation {
    // validate blacklist
    void validate_blacklist(account_name code_account, account_name to) {
        blacklist_table b(code_account, code_account);
        auto itr = b.find(to);
        eosio_assert(itr == b.end(), "account in blacklist");
    }

    // get total transfer record
    asset get_cap_used(account_name code_account, account_name to) {
        txrecord_table t(code_account, code_account);
        auto idx = t.get_index<N(to)>();
        asset used;

        auto first = idx.lower_bound(to);
        auto last = idx.upper_bound(to);
        auto now = current_time();
        while(first != last && first != idx.end())
        {
          used += first->amount;
          first++;
        }
        return used;
    }
    // validate transfer
    void validate_transfer(account_name code_account, account_name to, asset amount) {
        whitelist_table w(code_account, code_account);
        asset cap_total;
        asset cap_tx;
        auto itr = w.find(to);
        if(itr != w.end()) {
            cap_total = itr->cap_total;
            cap_tx = itr->cap_tx;
        } else {
            settings_table s(code_account, code_account);
            auto it = s.find(code_account);
            cap_total = it->cap_total;
            cap_tx = it->cap_tx;
        }
        eosio_assert(amount <= cap_tx, "cap_tx exceeded!");
        asset cap_used = get_cap_used(code_account, to);
        eosio_assert(cap_used <= cap_total, "cap_total exceeded!");
    }
}

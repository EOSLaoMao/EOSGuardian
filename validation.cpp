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
    // get transfer cap
    asset get_cap(account_name code_account, account_name to) {
        whitelist_table w(code_account, code_account);
        asset cap;
        auto itr = w.find(to);
        if(itr != w.end()) {
            cap = itr->cap;
        } else {
            settings_table s(code_account, code_account);
            auto it = s.find(to);
            cap = it->cap;
        }
        return cap;
    }
    // validate transfer
    void validate_transfer(account_name code, account_name to, asset amount) {
        asset cap = get_cap(code, to);
    }
}

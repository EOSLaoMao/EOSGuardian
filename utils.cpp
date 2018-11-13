using namespace eosio;
using namespace eosiosystem;
using namespace guardian;
using std::string;

namespace utils {
    void add_txrecord(account_name code, account_name to, asset amount, string memo) {
        txrecord_table t(code, code);
        t.emplace(code, [&](auto &i) {
            i.id = t.available_primary_key();
            i.to = to;
            i.amount = amount;
            }
        );
    }
}

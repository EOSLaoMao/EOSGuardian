using namespace eosio;
using namespace eosiosystem;
using namespace guardian;
using std::string;

namespace utils {
    void add_txrecord(name user, name to, asset quantity, string memo) {
        txrecord_table t(CODE, user.value);
        t.emplace(CODE, [&](auto &i) {
            i.id = t.available_primary_key();
            i.to = to;
            i.quantity = quantity;
            i.created_at = now();
        });
    }
}

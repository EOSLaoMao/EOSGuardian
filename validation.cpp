using namespace eosio;
using namespace eosiosystem;
using namespace guardian;
using std::string;

namespace validation {
    // validate blacklist
    void validate_blacklist(account_name code, account_name to) {
        blacklist_table b(code, code);
        auto itr = b.find(to);
        eosio_assert(itr == b.end(), "account in blacklist");
    }

    // get durtation
    uint64_t get_cap_duration(account_name code, account_name to) {
        whitelist_table w(code, code);
        uint64_t duration;
        auto itr = w.find(to);
        if(itr != w.end()) {
            duration = itr->duration;
        } else {
            settings_table s(code, code);
            auto it = s.find(code);
            duration = it->duration;
        }
        return duration;
    }

    // delete expired records
    void delete_records(account_name code, const std::vector<uint64_t>& ids=std::vector<uint64_t>()) {
        txrecord_table t(code, code);
        for(int i=0; i<ids.size(); i++)
        {
            uint64_t id = ids[i];
            print(" | id:", id);
            auto itr = t.find(id);
            if(itr != t.end()) {
                t.erase(itr);
            }
        }
    }

    // get total transfer record
    asset get_cap_used(account_name code, account_name to, asset quantity) {
        txrecord_table t(code, code);
        auto idx = t.get_index<N(to)>();
        asset used;
        std::vector<uint64_t> to_delete_ids;
        auto itr = t.get(0);
        auto first = idx.lower_bound(to);
        auto last = idx.upper_bound(to);
        auto n = now();
        uint64_t duration = get_cap_duration(code, to);
        while(first != last && first != idx.end())
        {
          if((duration*3600 + first->created_at) > n) {
              // not expired
              used += first->quantity;
          } else {
              // expired record should be deleted
              to_delete_ids.emplace_back(first->id);
          }
          first++;
        }
        // add quantity to used
        used += quantity;

        // delete expired orders
        delete_records(code, to_delete_ids);

        return used;
    }

    // validate transfer
    void validate_transfer(account_name code, account_name to, asset quantity) {
        whitelist_table w(code, code);
        asset cap_total;
        asset cap_tx;
        auto itr = w.find(to);
        if(itr != w.end()) {
            cap_total = itr->cap_total;
            cap_tx = itr->cap_tx;
        } else {
            settings_table s(code, code);
            auto it = s.find(code);
            cap_total = it->cap_total;
            cap_tx = it->cap_tx;
        }
        eosio_assert(quantity <= cap_tx, "cap_tx exceeded!");
        asset cap_used = get_cap_used(code, to, quantity);
        print("cap_used:", cap_used.amount);
        eosio_assert(cap_used <= cap_total, "cap_total exceeded!");
    }
}

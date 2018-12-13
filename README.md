# EOS Guardian / EOS 安全卫士

<img src="./logo.svg" height=100 width=100>

<a href="https://github.com/EOSLaoMao/EOSGuardian/blob/master/README-CN.md">中文版 README</a>

**EOS Guardian** is an EOS smart contract aiming to provide safer fund management with proper permission settings.

**EOS Guradian** is one of the many projects built by EOSLaoMao team.

VOTE EOSLaoMao: `eoslaomaocom`

## Background

**EOS Guardian** originated from another smart contract [SafeDelegatebw](https://github.com/EOSLaoMao/SafeDelegatebw) which was built by EOSLaoMao team for [Bank of Staked](https://github.com/EOSLaoMao/BankofStaked-CE) creditors.

SafeDelegatebw is a contract that simply wraps up a new `delegatebw` action calling the system contract's `delegatebw` action with the 5th parameter `--transfer` hardcoded to always be `false`.

In this way, instead of granting the system contract's `delegatebw` action permission to Bank of Staked (which is risky, because system contract's action could be called with `--transfer` specified to `true`, which will cause ownership change of your funds), creditors can deploy `SafeDelegatebw` and grant customized `delegatebw` action provided by the `SafeDelegatebw` contract.

The rules we followed buiding SafeDelegatebw are simple:

1. wrap up customized actions to lower the risks of actions provided by system contracts.
1. use lower-level permission to control (according to the [Principle of Least Priviledge](https://en.wikipedia.org/wiki/Principle_of_least_privilege)) these customized actions instead of using `active` or `owner` keys.

Then we examined means of applying these principles to a broader range of user cases that could benefit from this approach.

This is how EOS Guardian came about.

## Key Functions

There are two key functions provided by EOS Guardian for now:

### 1. safedelegate

This function is similar to the SafeDelegatebw smart contract. It hardcodes the `--transfer` parameter to always be `false`, so that all delegate transactions triggered via `safedelegate` are guaranteed to have ZERO risk of losing any funds.

### 2. safetransfer

This function is a little bit complex.

Imagine your EOS account was hacked with your `active` or `owner` key compromized. Currently by design, `active` or `owner` key could be used to do anyting including transfering all your EOS tokens out instantly.

Furthermore, we noticed that there is a list of EOS accounts that are suspicious, and should be handled with care... or not at all. To this end, we've added additional controls to prevent transfers to dangerous account.

As such, here are the security-related design principles applied in `safetransfer` for EOS Guardian:

1. use of customized lower risk permissions, rather than `active` or `owner`;
1. use adjustable time based caps for your transfers;
1. use configurable blacklist and whitelist mechanisms to check all transfers prior to proceeding.

By using the EOS permissions system, we achieved these goals in EOS Guardian, which contains 3 major parts:

#### 1. Global Cap Control

EOS Guardian provides a global cap controls:

1. cummulative limit amount that can be transfered out (`cap_total`) over several transfers within given a period (`duration` in minutes);
1. hard cap for any single transfer (`cap_tx`).

Here is the format for the global cap settings:

```
cap_total:    x EOS      // max amount of EOS you can transfer out in duration minutes
cap_tx:       y EOS      // max amount of EOS you can transfer out in one transfer
duration:     z          // cap_total duration, in minutes.
```

You can update these settings via the `setsettings` action using your `active` key:

```
cleos push action YOUR_ACCOUNT setsettings '{"cap_toal": "100.0000 EOS", "cap_tx": "1.0000 EOS", "duration": 60}' -p YOUR_ACCOUNT@active
```

After this is setup, you can use `safetransfer` to transfer EOS out using a lower risk permission `safeperm`:

```
cleos push action YOUR_ACCOUNT safertansfer '{"to": "SOME_ACCOUNT", "quantity": "1.0000 EOS", "memo": "test"}' -p YOUR_ACCOUNT@safeperm
```

Any `safetransfer` transaction will do a cap check before it triggers the `transfer` action in the system contract. If any check fails, no funds will be transfered out.

#### 2. Whitelist

Global cap control is a default check for all `safetransfer` transactions. If you want to set caps for specific accounts, you will find whitelist feature handy.

The fields of whitelist table are similar to global cap control. Here is how you can add account (e.g. account B) into your whitelist with different caps and duration using your `active` permission:

```
cleos push action YOUR_ACCOUNT setwhitelist '{"account": "B", "cap_toal": "200.0000 EOS", "cap_tx": "2.0000 EOS", "duration": 360}' -p YOUR_ACCOUNT@active
```

After adding account B to the whitelist, any `safetransfer` to account B will check `caps_total` and `cap_tx` configured in whitelist, instead of checking global cap setting (i.e. the whitelist overrides the global cap settings).

Additionally, you can remove accounts from your `whitelist` table using `delwhitelist` action:

```
cleos push action YOUR_ACCOUNT delwhitelist '{"account": "B"}' -p YOUR_ACCOUNT@active
```

#### 3. Blacklist

There are certain accounts you may not want to interact with; for example: accounts in ECAF orders or accounts you find suspicious.

Simply add these accounts to the `blacklist` table, any `safetransfer` will go through this table and make sure the recipient account is not in it (blacklist overrides both the whitelist and the global settings).

Here is how you can add and remove accounts from the blacklist:

```
cleos push action YOUR_ACCOUNT addblacklist '{"account": "HACKER_ACCOUNT"}' -p YOUR_ACCOUNT@active

cleos push action YOUR_ACCOUNT delblacklist '{"account": "HACKER_ACCOUNT"}' -p YOUR_ACCOUNT@active

```

## How to Deploy

### 1. Building the EOS Guardian Contract

```
git clone https://github.com/EOSLaoMao/EOSGuardian.git eosguardian
cd eosguardian
./build.sh
```

### 2. Deploying to Your Account

First, please ensure your account has sufficient RAM to deploy:

```
cleos set contract YOUR_ACCOUNT ../safeguardian/

```

### 3. Setup permissions

There is a script `scripts/perm.sh` to setup permissions. It will add two permissions under `active` permission.

1. `guardianperm`: only be used by the EOS Guardian contract, which just been deployed under your account in the previous step. It only contains one code permission: `YOUR_ACCOUNT@eosio.code`.
1. `safeperm`: used to fire `safetransfer` and `safedelegate`, it contains code permission as well as a key for your daily basis usage.

Prepare a new key pair for `safeperm` and setup like this:

```
./scripts/perm.sh YOUR_ACCOUNT SAFEPERM_PUBKEY
```

After setup, the permission structure of your account should look like this:

```
permissions:
     owner     1:    1 OWNER_PUBKEY
        active     1:    1 ACTIVE_PUBKEY
           safeperm     1:    1 SAFEPERM_PUBKEY, 1 eosguardian1@eosio.code
           guardianperm     1:    1 eosguardian1@eosio.code

```

### Add Caps/Whitelist/Blacklist

Add global caps:

```
cleos push action YOUR_ACCOUNT setsettings '{"cap_toal": "100.0000 EOS", "cap_tx": "1.0000 EOS", "duration": 60}' -p YOUR_ACCOUNT@active
```

Add/delete Whitelist accounts:

```
cleos push action YOUR_ACCOUNT setwhitelist '{"account": "B", "cap_toal": "200.0000 EOS", "cap_tx": "2.0000 EOS", "duration": 360}' -p YOUR_ACCOUNT@active

cleos push action YOUR_ACCOUNT delwhitelist '{"account": "B"}' -p YOUR_ACCOUNT@active

```

Add/delete Blacklist accounts:

```
cleos push action YOUR_ACCOUNT addblacklist '{"account": "HACKER_ACCOUNT"}' -p YOUR_ACCOUNT@active

cleos push action YOUR_ACCOUNT delblacklist '{"account": "HACKER_ACCOUNT"}' -p YOUR_ACCOUNT@active

```

### Use safetransfer and safedelegate using safeperm

```
cleos push action YOUR_ACCOUNT safertansfer '{"to": "SOME_ACCOUNT", "quantity": "1.0000 EOS", "memo": "test"}' -p YOUR_ACCOUNT@safeperm

cleos push action YOUR_ACCOUNT safedelegate '{"to": "SOME_ACCOUNT", "net_weight": "1.0000 EOS", "cpu_weight": "1.0000 EOS"}' -p YOUR_ACCOUNT@safeperm

```

We recommend that you only use `safeperm` to do transfer and delegate, and use active perm to do adjustments.

# Conclusion

By deploying **EOS Guardian**, user can use lower permission to do transfer with a cap which will lower their risk of fund losses.

We are looking forward to corporate with any wallets/tools to protect EOS accounts using **EOS Guardian**.

If You have any questions, please join our telegram: https://t.me/EOSGuardian

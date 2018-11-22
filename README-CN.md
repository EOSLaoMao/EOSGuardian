# EOS 安全卫士

<a href="https://github.com/EOSLaoMao/EOSGuardian/blob/master/README.md">ENGLISH README</a>

**EOS 安全卫士** 是一个旨在通过 EOS 的权限设置来保护账户资金安全的智能合约。

**EOS 安全卫士** 由 EOSLaoMao 团队开发。欢迎大家给 EOSLaoMao BP 节点投票：`eoslaomaocom`

## 起源

**EOS 安全卫士** 源于 EOSLaoMao 团队的另一个名为 [SafeDelegatebw](https://github.com/EOSLaoMao/SafeDelegatebw) 的智能合约，该合约用于协助 [Bank of Staked](https://github.com/EOSLaoMao/BankofStaked-CE) 项目的贷出人账户（creditor）.

SafeDelegatebw 将系统合约的 `delegatebw` 接口进行了重新封装，将该接口的第五个参数 --transfer 写死为 false。（这个参数置为 true 的时候，表示给对方抵押的同时，也会把这笔 EOS 的所有权转移给对方，是一个高风险操作）

这样一来，贷出人账户（creditor）只需要在自己账户上部署 SafeDelegatebw 合约并授权该合约的 delegatebw 接口（而不是系统合约的 delegatebw 接口）给 Bank of Staked，即可完成安全的抵押授权，消除资金转移的风险。

在开发 SafeDelegatebw 的过程中，我的遵循的规则很简单：

1. 重新封装系统合约提供的接口，在新接口中降低资金转移风险。
2. 使用自定义的低权限进行新接口的调用，而不是直接使用 active 或 owner 权限。

SafeDelegatebw 开发完成之后，我们一直在思考，上述的规则是不是可以有更广泛的应用。

正是这些思考催生了 EOS 安全卫士这一项目。

## EOS 安全卫士的功能

目前 EOS 安全卫士提供如下两个功能：

### safedelegate 安全抵押功能

该功能和上述的 SafeDelegatebw 合约类似，通过写死系统合约接口的 --transfer 参数为 false 将抵押过程中资金损失的风险降为0。

### safetransfer 安全转账功能

安全转账功能的开发背景：

EOS 默认的 active 和 owner 权限结构是有一定的资金风险的。假如某个账户的 active key 被盗，黑客可以瞬间转移所有的代币。

同时我们也注意到，用于是要避免与某些账户发生任何交易的，比如被 ECAF 加到黑名单的那批账户。有些用户因为与这些账户交易而遇到了麻烦。

上述的问题最终让我们确立了设计安全转账功能的 3 个基本功能：

1. 避免在日常交易中使用账户的 active 或者 owner 权限，转而使用风险更低的自定义权限。

2. 提供可调整的单笔转账上限控制，提供可调整的基于时间的转账总额上限控制。

3. 提供自定义黑名单/白名单账户功能，实现接受账户级别的转账限制。

By using EOS permission system, we achieved these goals in EOS Guardian which contains 3 major parts:

结合 EOS 的权限系统，EOS 安全卫士的安全转账功能实现了上述 3 个目标。

#### 1. 全局转账总额控制


通过全局转账总额功能，你可以设置一定时间范围内（duration）的转账总额上限（cap_total），你也可以设置单笔转账的上限（cap_tx）

下面是全局控制参数的数据：

```
cap_toal:     x EOS      // duration 时间内的转账总额上限
cap_tx:       y EOS      // 单笔转账的上限
duration:     z          // cap_total 总额的时间周期，单位为分钟
```

你可以通过账户的 active 权限通过 `setsettings` 接口进行上述参数的设置和调整：

```
cleos push action YOUR_ACCOUNT setsettings '{"cap_toal": "100.0000 EOS", "cap_tx": "1.0000 EOS", "duration": 60}' -p YOUR_ACCOUNT@active
```

设置完毕之后，你可以使用低级别的 `safeperm` 权限调用 `safetransfer` 功能进行安全转账（safeperm 权限的设置见后文）。

```
cleos push action YOUR_ACCOUNT safertansfer '{"to": "SOME_ACCOUNT", "quantity": "1.0000 EOS", "memo": "test"}' -p YOUR_ACCOUNT@safeperm
```

任何 `safetransfer` 触发的安全转账交易，都会先做单笔上限和总额上限的检查，检查通过之后，才会最终触发转账操作。检查失败，交易会自动失败，从而确保用户资金安全。


#### 2. 白名单功能

除了全局转账控制，EOS 安全卫士还提供白名单功能实现账户级别的转账控制。

白名单功能的参数跟全局转账控制的参数类似，你可以使用 active 权限添加白名单账户。下面的例子，是将账户 B 加入到白名单，并独立设置 duration，cap_total，cap_tx 参数。

```
cleos push action YOUR_ACCOUNT setwhitelist '{"account": "B", "cap_toal": "200.0000 EOS", "cap_tx": "2.0000 EOS", "duration": 360}' -p YOUR_ACCOUNT@active
```

将账户 B 加入到白名单之后，任何通过 `safetransfer` 安全转账功能给 B 账户的转账操作，都将执行白名单中配置的额度检查，而不再执行全局转账控制（白名单配置高于全局配置）。

当然，你也可以通过 `delwhitelist` 接口将特定账户从白名单中删除。

```
cleos push action YOUR_ACCOUNT delwhitelist '{"account": "B"}' -p YOUR_ACCOUNT@active
```

#### 3. 黑名单功能

有些 EOS 账户是最好不与之发生任何交易的，比如 ECAF 加入黑名单中的部分账户，或者任何用户觉得可疑的账户。基于上述考虑，EOS 安全卫士提供了黑名单功能。

在你将特定账户加入到黑名单之后，任何通过 `safetransfer` 触发的向这些账户发出的转账交易都会自动失败。

你可以通过 `addblacklist` 和 `delblacklist` 接口将账户加入或移除黑名单：

```
cleos push action YOUR_ACCOUNT addblacklist '{"account": "HACKER_ACCOUNT"}' -p YOUR_ACCOUNT@active

cleos push action YOUR_ACCOUNT delblacklist '{"account": "HACKER_ACCOUNT"}' -p YOUR_ACCOUNT@active

```


## 如何部署 EOS 安全卫士

### 1. 编译 EOS 安全卫士

```
git clone https://github.com/EOSLaoMao/EOSGuardian.git eosguardian
cd eosguardian
./build.sh
```

### 2. 部署到你的账户

确保账户有足够的 RAM 用于部署：

```
cleos set contract YOUR_ACCOUNT ../safeguardian/

```

### 3. 设置权限

接下来需要使用 `scripts/perm.sh` 脚本进行权限设置，该脚本将在 active 权限下面新增两个自定义权限。

第一个权限，`guardianperm`，该权限用于该账户上的 EOS 安全卫士自身的调用，并且需要被授权系统合约的转账权限。该权限只包含一个 code permission。

第二个权限，`safeperm`，该权限将用于用户调用安全抵押（safedelegate）和安全转账（safetransfer）接口。该权限由 code permission 和一个新的公钥构成，该公钥用于用户的日常使用。

准备好一个用户 safeperm 的新公钥，然后执行权限设置脚本：


```
./scripts/perm.sh YOUR_ACCOUNT SAFEPERM_PUBKEY
```

设置完成之后，账户的权限结构将变为：

```
permissions:
     owner     1:    1 OWNER_PUBKEY
        active     1:    1 ACTIVE_PUBKEY
           safeperm     1:    1 SAFEPERM_PUBKEY, 1 eosguardian1@eosio.code
           guardianperm     1:    1 eosguardian1@eosio.code

```

### 设置全局额度配置/白名单/黑名单

配置全局额度参数：


```
cleos push action YOUR_ACCOUNT setsettings '{"cap_toal": "100.0000 EOS", "cap_tx": "1.0000 EOS", "duration": 60}' -p YOUR_ACCOUNT@active
```

增加/删除白名单配置：

```
cleos push action YOUR_ACCOUNT setwhitelist '{"account": "B", "cap_toal": "200.0000 EOS", "cap_tx": "2.0000 EOS", "duration": 360}' -p YOUR_ACCOUNT@active

cleos push action YOUR_ACCOUNT delwhitelist '{"account": "B"}' -p YOUR_ACCOUNT@active

```

增加/删除黑名单配置：

```
cleos push action YOUR_ACCOUNT addblacklist '{"account": "HACKER_ACCOUNT"}' -p YOUR_ACCOUNT@active

cleos push action YOUR_ACCOUNT delblacklist '{"account": "HACKER_ACCOUNT"}' -p YOUR_ACCOUNT@active

```

### 使用安全转账（safetransfer）和安全抵押（safedelegate）功能

```
cleos push action YOUR_ACCOUNT safertansfer '{"to": "SOME_ACCOUNT", "quantity": "1.0000 EOS", "memo": "test"}' -p YOUR_ACCOUNT@safeperm

cleos push action YOUR_ACCOUNT safedelegate '{"to": "SOME_ACCOUNT", "net_weight": "1.0000 EOS", "cpu_weight": "1.0000 EOS"}' -p YOUR_ACCOUNT@safeperm

```

我们建议部署了 EOS 安全卫士账户的用户，在今后的日常使用中，仅使用 `safeperm` 这一低级权限进行日常转账操作，同时只在必要的时候使用 active 权限（比如调整上述的配置）


# 结论

通过部署并合理配置 EOS 安全卫士合约，用户可以一定程度上降低资金风险。

我们也非常期待和各类钱包/工具合作，将 EOS 安全卫士推广给更多用户。

如果你有任何问题，欢迎加入 EOS 安全卫士 telegram 群与我们交流：https://t.me/EOSGuardian

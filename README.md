# 万金油客户端

## 命令文法

- stmt          : `stmt         -> block block*  | command (, command)*`
- block         : `block        -> { stmt stmt* }`
- command       : `command      -> id(params)    | id params`
- params_space  : `params_space -> param params  | ϵ`
- params_comma  : `params_comma -> param (, param)* | ϵ`
- param         : `string`
- id            : `[A-Za-z_][A-Za-z0-9_]*`


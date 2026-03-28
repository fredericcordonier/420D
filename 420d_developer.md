
# 420D Developer guide

## Coding naming conventions ##

### Structures ###

Structure names are suffixed with '_t'.

### Identifiers ###

Functions and variables names follow the following pattern:

```
<type>_<visibility>_<variable name>
```

#### Type letter ####

| <!--  -->    |  <!--  -->  |
| ------------ | ----------- |
| structure    |  s |
| integer      | i |
| char         | c |

Pointers are prefixed with the character 'p'. Arrays are prefixed with the character 'a'.


#### Visibility letter ####

| <!--  -->    |  <!--  -->  |
| ------------ | ----------- |
| global | g |
| local  | l |
| static | ``none``  |
| function parameter | x |
| constant function parameter | c |

#### Examples ####

| <!--  -->    |  <!--  -->  |
| ------------ | ----------- |
| i_value | Static integer |
| pi_cx_value | function parameter of type pointer to const int |


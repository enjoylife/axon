* File:  JouleQueue.md
* Version: 1.0
* Purpose: Documentation and architecture overview of JouleQueue.
* Author: Matthew Clemens
* Copyright: Modified BSD, see LICENSE for more details 

JouleQueue (worker queue)
======================
> "derived unit of energy/work."

## Datatypes

```c
jq_t
jq_config_t
jq_stat_t
```

## API
```c
jq_init(jq_t q, jq_config_t config);
jq_nq(jq_t q, void * data);
jq_dq(jq_t q, void * data);
jq_stat(jq_t q, jq_stat_t stat);
jq_link(jq_t q, jq_t q2, jq_config_t config);
jq_config(jq_config_t c, ...);
```



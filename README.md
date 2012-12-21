Axon
====

Implementation of a DHT

Primary Model
============

## Based on Dynamo, Chord, etc
- Given a key, it maps that key to a node.

* Provides a single API call `lookup(axon, key)`
* utilizes consitant hashing to help balance load.
  * when hash table resizing only k/n key need to be remapped.(k=num keys, n= num nodes)
* each node needs routing info for only a small number of nodes
  * O(log n) 
  * lookups with O(log n) messages between them
*assigns each node and key an m bit identifier.
  * nodes id from ip
  * key id from a hash of string key
* scalable key lookup
  * each node maintains a routing table (finger table) with m entries
    * O(log n) are distinct
    * index i is the id of the first node s the succeeds this nod n by at elast 2^(i-1). (0 index)


Needed Vocab
============

* *Identity Circle*: the set of identifiers that are wrapped in the identifer space.
* *m*: the number of bits that make up the identifers for key and node
* *r*: number of succeesors stored at a node
* *node*: the physical machines that the DHT is built upon
* *k*: number of successors that a single nodes keys are replicated to.
* *successor*: th node that is equal to or first following  the hashed values of a key or node
* *N*: total of nodes at a given time
* *fingerTable*: routing table with up to m entries per node 


Axon API
========

## Public Datatypes

axon_t
axon_config_t
axon_stat_t
axon_net_info

## Public Error returns
EA_HARD_FAIL
EA_NET_FAIL
EA_FAIL


### int axon_init(axon_t *obj, axon_config_t *config);
* The axon_init function initializes the pointed to variable obj.
* If the config variable is NULL, obj is created with a default configuration.
* Returns 0 on success, positive error number on failure.

### int axon_config_init(axon_config_t *config);
* The axon_config_init function initializes the pointed to variable config.
* Returns 0 on success, positive error number on failure.

### int axon_join(axon_t *obj, axon_net_info *info);
* TODO
    
### int axon_leave(axon_t *obj, axon_net_info *info);
* TODO

### int axon_destroy(axon_t *obj);
* Returns 0 on success, positive error number on failure.

### int axon_config_destroy(axon_config_t *config);
* Returns 0 on success, positive error number on failure.


### int axon_lookup(axon_t *obj, axon_stat_t *stat, char *key, int length, void *value);
* The axon_lookup function uses the already initialized obj along with the pointed to key to lookup the associated value with said key. 
* The length variable is the length of the key array.
* The void pointer will be  populated with a NULL if the key was not found, or on success, a non-null value which was previously stored.
* Do not use a test of the value variable's nullnes as a primary means of checking for succes.
* The pointed to stat variable is a datatype that holds information gathered as a consequence of this function call.
* If the stat variable is NULL no information about this call will be gathered.
* EA_HARD_FAIL signifies that their was a error even before the network was traversed.
* On return the stat variable will be populated with query stats and information, as long as the return value is not EA_HARD_FAIL.   
* Returns 0 on success, positive error number on failure.

### int axon_distrbute(axon_t *obj, axon_stat_t *stat, char* key, int length, void *value);
* The axon_distrbute function stores the key, value  pair within this axon's network scope.
* The length variable is the length of the key array.
* The pointed to stat variable is a datatype that holds information gathered as a consequence of this function call.
* If the stat variable is NULL no information about this call will be gathered.
* EA_HARD_FAIL signifies that their was a error even before the network was traversed.
* On return the stat variable will be populated with query stats and information, as long as the return value is not EA_HARD_FAIL.   
* Returns 0 on success, positive error number on failure.


Private Axon functions
======================

### int _stabilize

### int _notify

### int _fix_fingers

### int _check_pred

### int _check_suc

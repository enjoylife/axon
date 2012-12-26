Axon
====

"An axon (from Greek, axis) also known as a nerve fiber; is a long, slender projection of a nerve cell.
The function of the axon is to transmit information to different neurons, muscles and glands."

An implementation of a DHT (Distributed Hash Table)

Primary Model
============

* Given a key, it maps that key to a node.
* A single pair of important API calls. IE; `lookup(key)`, `distribute(key, data)` 
* assigns each node and key an fixed length bit identifier.
  * node's id is derived from  it's ip.
  * key id from a hash of application defined string key.

* utilizes consitant hashing to help balance load.
  * when hash table needs resizing, only k/n key need to be remapped.(k=num keys, n= num nodes)

* each node needs routing info for only a small number of nodes
  * O(log n) 
  * lookups with O(log n) messages between them

* scalable key lookup
  * each node maintains a routing table (finger table) with m entries
  * O(log n) are distinct
  * index i is the id of the first node s the succeeds this nod n by at elast 2^(i-1). (0 index)

Inspiration
===========

## Code
* nginx
* memcached
* redis
* nbds
* Unix Network Programming
* pthreads programming

## Philosophy / Design
* The Architecture of Open Source Applications
* On the Impact of the Computer on Society

## Papers
* Chord, Dynamo, 
* A performance vs. cost framework for evaluating DHT design tradeoffs under churn


Needed Vocab
============

* *Identity Circle*: the set of identifiers that are wrapped in the identifer space.
* *m*: the number of bits that make up the identifers for key and node
* *r*: number of successors stored at a node
* *node*: the physical machines that the DHT is built upon
* *k*: number of successors that a single nodes keys are replicated to.
* *successor*: th node that is equal to or first following  the hashed values of a key or node
* *N*: total of nodes at a given time
* *fingerTable*: routing table with up to m entries per node 


Global Picture
==============

The axon api is the networking and communications layer between nodes on a distributed system. The design and archecture for axon is based upon a few key parts. First its importent to know that it is a single process that contains multiple threads running non blocking functions or distpatching work to others. Some of the dispatches are to other axon running computers, or they could be to this computers idleing threads looking for work.

The main thread is responsible for inital configuration and monitering of itself, as well as accepting and handing off of incomping new connections to the thread workers.

Thread workers handle request processing related to a wide array of tasks. Incoming tasks needed to be done may be simple in-memory key checks, while others might be long, large file transfers. This hetrogenous group of tasks requires different approachs which ultimitly end up being processed by the same functions. The disparity comes from time constraints, ex tcp timeouts vs a log(n) in memory key look up. One has a millisecond granularity while the latter has clock cycle precesion. The timeing of these functions needs to be adjusted accordingly. But why should we be timing these anyway? Statistics of your process is invaluable information. Improving, recording, and monitering provides a means to regulate. or even better a angle to pitch to investors. These stats being recored need to be consistant across every function. 
A more through look at the types of processing done by the worker threads revels specific tasks in certain areas. 

    * Routing of packets to and from our machine. 

This requires a standard of some kind in the comunication across nodes. A agreed upon format is read and checked per request, with the various cases of message types being jumped to per the type of message. Each recieved transfer of bytes is inspected, followed by any other messaging processing function needing to be run. From there it is passed to a type of task that,

* Gets or Sets the key value pairs that are inserted and deleted from the node. 

Depending on the truthyness of the key being present, the thread may have to preform  network io or file io.File io blocking is avoided by useing the aio api. Asyncronous File Input / Output. Fast key request are served from in-memory keys, while frequently and recently pull values are stored in the remaining allocated memory.

* Network Maintance internally and between nodes.

Periodic checks and fixes are needed on the network. With nodes sending periodic heartbeats, as well as avoiding timeouts. Fixes to the internal routing need to be applied following other changes. Also node arrivals and depautures from the network cluster. Network io is non blocking by using notification and polling api's ie, epoll kqueue, poll, etc.

Processing hooks for a application using this library are placeable in a priority worker queue.


## Where does Axon fit?
__Top Layer__
* Applications, users, and so on.
* Semantic Layer  
* Network Layer  - *Axon*
* Database Layer 
* OS, Hardware, and so on.
__Bottom Layer__


### OSI Model
Level 7, (application layer).

Axon API
========

## Datatypes
axon_t
{
}

axon_config_t
{
}

axon_stat_t
{
}

axon_net_t
{
}

axon_unit_t 
{
* The k_length variable is the length of the key char array.
* The v_length variable is the length of the buffer of data.
    char * key;
    int k_length;
    void * value;
    int v_length;

    //int init_time;
    //int version;
}

axon_net_ops
{
}
    
    


## Error Constants
A_HARD_FAIL
A_NET_FAIL
A_FAIL


## Enums
A_PUT
A_GET
A_ANY



## Inits / Destroys

### int axon_init(axon_t *obj, axon_config_t *config);
* The axon_init function initializes the pointed to variable obj.
* If the config variable is NULL, obj is created with a default configuration.
* Returns 0 on success, positive error number on failure.

### int axon_destroy(axon_t *obj);
* Returns 0 on success, positive error number on failure.

### int axon_config_init(axon_config_t *config);
* The axon_config_init function initializes the pointed to variable config.
* Sets all config members to their respective default values
* Returns 0 on success, positive error number on failure.

### int axon_config_destroy(axon_config_t *config);
* Returns 0 on success, positive error number on failure.



## Config Modification 

### int axon_hash(axon_t *obj, (*hash_func)(char * str, int length),int action );
* Sets the hash function to use for the key 
* Does not modify the hash for network id routes, only keys
* If action is A_PUT  the previous hash function is overridden with the function pointed to by hash_func. 
* If action is A_GET, on success the currently used hash function will be pointed to by hash_func, regardless of what hash_func was pointing to prior to the function call. 



## Network State

### int axon_join(axon_t *obj, axon_net_t *info);
* TODO
    
### int axon_leave(axon_t *obj, axon_net_t *info);
* TODO

### int axon_stabilize();
* TODO

### int axon_notify();
* TODO

### int axon_fix_fingers();
* TODO

### int axon_check_predecessor();
* TODO

### int axon_check_successor();
* TODO



## Key Value Manipulation

### int axon_lookup(axon_t *obj, axon_stat_t *stat, axon_unit_t unit, axon_net_ops *options);
* The axon_lookup function uses the already initialized obj along with the pointed to unit which contatins the key, value pair. 

* The void pointer will be  populated with a NULL if the key was not found, or on success, a non-null value which was previously stored.
* Do not use a test of the value variable's nullnes as the primary means of checking for succes.

* The pointed to stat variable is a datatype that holds information gathered as a consequence of this function call.
* If the stat variable is NULL at invocation no information about this call will be gathered.
* On return the stat variable will be populated with query stats and information, as long as the return value is not A_HARD_FAIL.   

* A_HARD_FAIL signifies that their was a error even before the network was traversed.
* Returns 0 on success, positive error number on failure.

### int axon_distribute(axon_t *obj, axon_stat_t *stat, axon_unit_t unit, axon_net_ops *options);
* The axon_distrbute function stores the key, value  pair within this axon's network scope.

* The pointed to stat variable is a datatype that holds information gathered as a consequence of this function call.
* If the stat variable is NULL no information about this call will be gathered.
* A_HARD_FAIL signifies that their was a error even before the network was traversed.
* On return the stat variable will be populated with query stats and information, as long as the return value is not A_HARD_FAIL.   
* Returns 0 on success, positive error number on failure.

### int axon_handle_lookup();
* TODO


### int axon_handle_distribute();
* TODO


Private Axon functions
======================
* TODO

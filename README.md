axon
====

Implementation of a DHT

Primary Model
============

## Chord
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
===========

* *Identity Circle*: the set of identifiers that are wrapped in the identifer space.
* *m*: the number of bits that make up the identifers for key and node
* *r*: number of succeesors stored at a node
* *node*: the physical machines that the DHT is built upon
* *k*: number of successors that a single nodes keys are replicated to.
* *successor*: th node that is equal to or first following  the hashed values of a key or node
* *N*: total of nodes at a given time
* *fingerTable*: routing table with up to m entries per node 


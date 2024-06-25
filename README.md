# kvd

KvD is an in-memory storage to coordinate services on cluster nodes over HTTP.

## Use case

[uidgen](https://github.com/gcca/doapi) is an ID-generator service. KvD is used to assign on runtime the cluster and node id-values

## Examples

Key-value operations:

```sh
$ printf 'GET key\nPUT key value\nGET key' | http :8000/kvd
{"results":["","$ok","value"]}
```

Counter operations:

```sh
$ printf 'COUNT key\nCOUNT key' | http :8000/kvd
{"results":["0","1"]}
```

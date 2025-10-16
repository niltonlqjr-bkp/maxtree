
# maxtree
Parallel Maxtree implementation in a new account (Old one was disabled).
[Old repository](https://github.com/niltonlqjr/maxtree) 

=======
### Maxtree parallel implementations

#### Prerequisites
- libvips [libvips.org](https://www.libvips.org/)
- C++11 Compiler (tested on g++ 13.3.0)

#### Docker ready to use

- Docker on [github](https://github.com/niltonlqjr/docker-libvips)

#### Build

- Build all implementations (parallel and sequential)
```console
$ make
```

- Build only parallel
```console
$ cd parallel
$ make
```

- Build only sequential
```console
$ cd sequential/flood
$ make
```

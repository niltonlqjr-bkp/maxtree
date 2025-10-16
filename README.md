
# maxtree
maxtree in backup account backup (new repository)
Old [account](https://github.com/niltonlqjr) was disabled
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

# Modern C++ usage practice, the final lab

## Problem statement

Develop a simple web service allowing for the storage of JSON documents.

The service should handle the requests as follows:
- `[POST] serviceurl` - add a document; an identifier (`id`) is returned
- `[PUT] serviceurl/<id>` - update a document with the specified `id`
- `[DELETE] serviceurl/<id>` - delete the document with the specified `id`
- `[GET] serviceurl/<id>` - get a document with the specified `id`
- `[GET] serviceurl` - get all stored documents
- `[GET] serviceurl?key=<key>[&value=<value>]` - get all documents that have the specified `key` and `value` (if used)

## Building, installation, and usage

### Dependencies

The following is required:
- C++17 or higher
- CMake 3.0 or higher (to build)
- [`fastcgi++`](https://github.com/eddic/fastcgipp) 3.0 or higher
- [`lighttpd`](https://www.lighttpd.net/) 1.4.63 or higher

### Building

To build FastCGI script use the shell commands:
```
$ mkdir build
$ cd build
$ cmake ..
$ cmake --build . --target fcgi 
```

`fastcgi++` should be built and installed before building the script.

### Installation

To install the script and prepare server directories use the `init.sh` script (after building); before using the server you should log out and log in again:
```
$ ./init.sh
```

To uninstall the script and destroy server directories use the `destroy.sh` script:
```
$ ./destroy.sh
```

### Usage

To run the server use `lighttpd`:
```
$ lighttpd -D -f lighttpd.conf
```

## Author

Daniil Lebedev aka [@alphaver](https://github.com/alphaver), student (4th year, group 13), FAMCS, BSU

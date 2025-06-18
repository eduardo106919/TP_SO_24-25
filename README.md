# Operating Systems Project

Practical Assignment for the **Operating Systems** course in the 2024/2025 academic year.

This service allows **indexing** and **searching** of text documents stored locally on a computer. The **server** program is responsible for registering **metadata** about each document (e.g., unique identifier, title, year, author, location), and it also supports a set of **queries** related to this metadata and the content of the documents.

Users must use a **client** program to interact with the service. This **interaction** will allow users to add or remove a document from the service’s index and to perform **searches** on the indexed documents.


## Usage

Run `make` to compile the program.

The **server** must be initialize before the **client**, otherwise the program will not run properly.

### Server

To **start** the server, run:
```bash
./bin/dserver document_folder cache_size [-g] [cache_type]
```
- `document_folder`: folder where the documents to be indexed are located
- `cache_size`: maximum number of entries to be kept in memory
- `-g`: turns off debugging messages (optional)
- `cache_type`: selects the eviction policy to use in the cache (optional)

The valid values for `cache_type` are:
- `FIFO`: First In First Out Eviction Policy
- `RAND`: Random Eviction Policy
- `LRU`: Least Recently Used Eviction Policy

**Note**: if the user does not specify an eviction policy, the cache is **not used**, so the program only works with **disk management**.

### Client

To **index** a document, run:
```bash
./bin/dclient -a "title" "authors" "year" "path"
```
- `title`: title of the document
- `authors`: author(s) of the document (e.g., separated by a semicolon (‘;’) when there are several authors)
- `year`: year of the document
- `path`: relative path of the document, i.e., from the base directory configured for the service

To **remove** an indexed document, run:
```bash
./bin/dclient -d "key"
```
- `key`: document identifier

To **consult** the indexed metadata, run:
```bash
./bin/dclient -c "key"
```
- `key`: document identifier

Count the **number of lines** of a given document that contain a given **keyword**:
```bash
./bin/dclient -l "key" "keyword"
```
- `key`: document identifier
- `keyword`: word to look for

Return a **list** of document identifiers that contain a given **keyword**, using N **processes**:
```bash
./bin/dclient -s "keyword" [nr_processes]
```
- `keyword`: word to look for
- `nr_processes`: number of processes to use (optional)

**Note**: if `nr_processes` is not specified, the default value is 1.

To **shut down** the server, run:
```bash
./bin/dclient -f
```

## Testing



## Others

To generate **documentation**, you can run:
```
make docs
```
This command opens a Firefox tab, so be aware.


To **clean the binary files** that persist after the server is shut down, run:
```
make trash
```


# ChainDB

![ChainDB](https://i.imgur.com/iQIpccw.png)

## Description
This is a secure and reliable blockchain database.

Each database contains its own public and private key.
All blocks are signed with the private key (ECDSA/secp256k1).

## Instructions
### Build (debug mode)
`$ make build`

### Build (release mode)
`$ make release`

### Testing
`$ make test`

### Run
`$ make run`

### CLI Usage
Go to shell:

`$ make shell`

Go to the build directory:

`$ cd build`

Create the chain:

`$ ./cli/cli --create-chain true`

Add new block:

`$ ./cli/cli --add-block true --data '{data: \"test\"}'`

Get blocks:

`$ ./cli/cli --get-blocks true` or `$ ./cli/cli --get-block true --block-id 1`

Get info:

`$ ./cli/cli --get-info true`

Get keys:

`$ ./cli/cli --get-keys true`

Verify the chain:

`$ ./cli/cli --verify-chain true --chain-id 1`

Remove the chain:

`$ ./cli/cli --remove-chain true --chain-id 1`

Look for more here: `./cli/cli -h`

### Run in GDB
`$ make gdb`

### Run in Valgrind
`$ make valgrind`

### Run in Callgrind
`$ make callgrind`

### Check code (cppcheck)
`$ make cppcheck`

## Author
Stanislav Yakush (st.yakush@yandex.ru)

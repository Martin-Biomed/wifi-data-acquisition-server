
# NOT USED: Protobuf lacks adequate documentation and support in Objective C (Windows)

This project uses Protocol Buffers (protobuf) to define messages that will be sent to/from the C and Java components
of the project. The protobuf protocol was selected because it enables language-agnostic communications. 

The protobuf protocol defines the format of the messages, but an additional communication method is needed to actually
transfer data between the apps (based on different languages).

## Working with Protocol Buffers

### The (protobuf-c) Library

The (protobuf-c) library lets developers encode/decode Protocol Buffer messages in C.

### Defining the structure of the data (.proto files)

The first step when working with protocol buffers is to define the structure for the data you 
want to serialize in a proto file, which uses the (.proto) extension.

For more detail on how to construct a (.proto) file, see: https://protobuf.dev/getting-started/cpptutorial/

### The Protoc Compiler

The protoc compiler generates data access definitions based on the (.proto) definitions. The compiler can generate
data access classes in multiple programming languages.

The compiled files create the messaging definitions that can then be populated from inside the C code (using the protobuf-c library).

## Setup Steps

1. This project uses the (protobuf-c) library, installed using vcpkg: **.\vcpkg\vcpkg install [package name]:x64-windows**
2. Downloading the appropriate VSCode Extension: On the VSCode "Extensions" manager, select (vscode-proto3)
3. Downloaded the Protoc compiler (ZIP file) from: https://protobuf.dev/getting-started/cpptutorial/
    - Create a folder in the VSCode project called "protoc".
    - Copy the Extracted contents from (protoc-25.2-win64.zip) to the VSCode project "protoc" folder.
    - **Optional:** Add it to path. For this project I have chosen not to add the (protoc) compiler at the OS-level path.

## Compilation Steps (Protoc)

The Protoc command that was downloaded for this project can be invoked from folder: protoc/protoc-25.2-win64/bin/protoc.exe

To use the Protoc compiler, we have to invoke the relative location of the command.
- In the terminal, type: *.\protoc\protoc-25.2-win64\bin\protoc*

To compile based on your (.proto) files, use: 
*.\protoc\protoc-25.2-win64\bin\protoc --proto_path=.\main\ --objc_out=.\build\  .\main\return-msgs.proto*
- This will produce the "ReturnMsgs" files in the project "build" folder.
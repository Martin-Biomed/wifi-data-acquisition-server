
## What is Flatbuffers

Flatbuffers are a Google message format in the same vein as Protocol Buffers or JSON (but are designed to be much faster). 
They are used to create language-agnostic structures that can be used for messaging between applications.

### Schema File
You must create a schema file that defines the data structures that we may want to transfer over serial connections.
- To create schema files, you can use the VSCode FlatBuffers extension for syntax help.

### The FlatBuffer Compiler (flatc)
The FlatBuffers compiler requires the corresponding 

## Setup Steps

1. Copy the flatbuffers source code into your VSCode project.
    - Navigate to the terminal in VSCode and use cmd: **git clone https://github.com/google/flatbuffers.git**
    - Change into the flatbuffers directory: **cd .\flatbuffers**


2. Install FlatBuffers in your project. The PC that was used for producing this code had installed Visual Studio 16 (2019).
    - Run command: **cmake -G "Visual Studio 16 2019"**

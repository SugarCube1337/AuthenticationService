# AuthenticationService (cse-programming-languages Project)
## Overview

This project, **AuthenticationService**, focuses on implementing a REST API server with a custom minimal framework for issuing and validating JWT tokens. The server facilitates secure access to resources within a computational network, with specific details about the network and resources left undefined.

## Features
- **Custom REST API Framework:** The server utilizes a minimalistic, self-developed framework for processing requests, including parsing GET and PUT requests.
- **JWT Tokens:** Implements JWT tokens with hmac-sha256 for secure token signing and validation.
- **Network Communication:** Utilizes TCP for communication with TLS encryption using the OpenSSL library.
- **Database:** MongoDB stores information about users and services. The choice of MongoDB is driven by the non-normalized nature of user data.
- **Administration Interface:** An interactive command-line interface enables system administrators to interact with the MongoDB database.
- **Multithreading:** The program employs multithreading, with separate threads for network communication and command-line interface operations. The main thread controls the lifecycle of the worker threads and can terminate them gracefully using signals (Ctrl+C).
- **Non-blocking Client Operation:** Achieved through the use of the `select` mechanism for user input and `epoll` for tracking network connections.
- **Dockerization:** The MongoDB database can be deployed using Docker, with a corresponding Docker Compose file provided in the deployment folder.

## Getting Started
1. Run the initialization script for WSL: `bash wsl_init.sh`.
2. Navigate to the build directory: `cd /mnt/c/Users/danil/CLionProjects/AuthenticationService/cmake-build-debug/`.
3. Execute the AuthenticationService: `./AuthenticationService`.
4. For MongoDB setup, use the following Docker Compose commands:
    - Start: `docker-compose -f ./mongo.yml up -d`.
    - Stop and remove: `docker-compose -f ./mongo.yml down`.

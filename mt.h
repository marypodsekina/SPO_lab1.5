#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include <bson/bson.h>
#include <signal.h>

#define PORT 5555
#define MAX_MSG_LENGTH 1024
#define END_STRING "chau"
#define JAVA_CALL "/usr/lib/jvm/java-16-openjdk-amd64/bin/java -classpath /mnt/c/Users/serge/IdeaProjects/spo-parser/src/main/java/lib/gson-2.8.7.jar:/mnt/c/Users/serge/IdeaProjects/spo-parser/src/main/java/classes CustomParser in.txt"
#define HOST "127.0.0.1"

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL SO_NOSIGPIPE
#endif

#define perro(x) {fprintf(stderr, "%s:%d: %s: %s\n", __FILE__, __LINE__, x, strerror(errno));exit(1);}

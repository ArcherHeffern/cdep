#include <stdio.h>
#include <stdbool.h>

#ifndef __DEPLOYEE__
#define __DEPLOYEE__ 1


typedef struct {
    char *name;
    char *ip;
    char *username;
    char *password;
} Remote; 

typedef struct {
    int capacity;
    int size;
    Remote **remotes;
} Remotes;

typedef struct {
    char *name;
    char *executable_location;
    char *start_command;
    char *remote_name;
} Service;

typedef struct {
    int capacity;
    int size;
    Service **services;
} Services;

enum Section {
    SERVICE,
    REMOTE,
    NONE,
};

Remotes* remotes_init(int capacity);
void remotes_destroy(Remotes *remotes);
Services* services_init(int capacity);
void services_destroy(Services *services);
// Returns 0 if remote with same name already exists
bool remotes_insert(Remotes *remotes, Remote *remote);
bool services_insert(Services *services, Service *service);

Remote* remote_init(char *name, char *ip, char *username, char *password);
void parse_remote(FILE *file, char *line);
void remote_print(Remote *remote);
void remote_destroy(Remote *remote);


Service* service_init(char *name, char *executable_location, char *start_command, char *remote_name);
void parse_service(FILE *file, char *line);
void service_print(Service *service);
void service_destroy(Service *service);


void parse_header(char *line);
void parse_section(FILE *file, char *line);


char* get_resource_name(char *line);
int get_str(FILE *file, char** out);

#endif 

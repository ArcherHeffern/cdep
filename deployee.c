#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdlib.h>
#include <unistd.h>
#include "deployee.h"
#include "strutil.h"

#define REMOTE_CAPACITY 5
#define SERVICE_CAPACITY 5

Remotes *remotes;
Services *services; 

enum Section section = NONE;
char *service_header = "__services__";
char *remote_header  = "__remotes__";


int main(int argc, char** argv) {
    remotes = remotes_init(8);
    services = services_init(8);

    char *file_name;
    FILE *file;
    ssize_t num_read;
    char* line = NULL;
    size_t n = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", basename(argv[0]));
        exit(1);
    }
    file_name = argv[1];
    if (access(file_name, R_OK) == -1) {
        perror("Access");
        exit(1);
    }
    file = fopen(file_name, "r");
    if (file == NULL) {
        perror("fopen");
        exit(1);
    }
    while ((num_read = getline(&line, &n, file)) != -1) {
        num_read = strip(&line, num_read);
        if (num_read == 0) {
            continue;
        }
        switch (line[0]) {
            case '_':
                parse_header(line);
                break;
            case '[':
                parse_section(file, line);
                break;
            default:
                printf("Unrecognized option: %s\n", line);
            }
        if (line) {
        }
    }
    remotes_destroy(remotes);
    services_destroy(services);
    free(line);
    fclose(file);

}

void parse_header(char *line) {
	if (strcmp(line, service_header) == 0) {
		section = SERVICE;
	} else if (strcmp(line, remote_header) == 0) {
		section = REMOTE;
	} else {
		fprintf(stderr, "Bad Header: \"%s\" not recognized\n", line);
		exit(1);
	}
}

void parse_section(FILE *file, char *line) {
	if (section == SERVICE) {
		parse_service(file, line);
	} else if (section == REMOTE) {
		parse_remote(file, line);
	} else {
		fprintf(stderr, "Define a section header before declaring a resource\n");
	}	
}

int get_str(FILE *file, char** out) {
    size_t n = 0;
    int n_read;
    char *line;

    n_read = getline(&line, &n, file);
    n_read = strip(&line, n_read);
    if (n_read == 0) {
        return 0;
    }
    *out = malloc(n_read * sizeof(char) + 1);
    strcpy(*out, line);
    return n_read;
}

char* get_resource_name(char *line) {
    char *name;
    int name_len;

    name_len = strlen(line);
    if (line[name_len-1] != ']') {
        fprintf(stderr, "Malformed resource header name: Should be contained in Braces. Got %s\n", line);
    }
    line++;
    line[name_len - 2] = 0;
    name = malloc((name_len - 1));
    strcpy(name, line);
    return name;
}

void parse_service(FILE *file, char *line) {
    char *name;
    char *executable_location;
    char *start_command;
    char *remote_name;
    Service *service;

    name = get_resource_name(line);

    if (get_str(file, &executable_location) <= 0) {
        fprintf(stderr, "Error getting executable location in %s\n", name);
    }
    if (get_str(file, &start_command) <= 0) {
        fprintf(stderr, "Error getting start command in %s\n", name);
    }
    if (get_str(file, &remote_name) <= 0) {
        fprintf(stderr, "Error getting remote name in %s\n", name);
    }
    service = service_init(name, executable_location, start_command, remote_name);
    if (!services_insert(services, service)) {
        fprintf(stderr, "[%s] is duplicated\n", name);
        exit(1);
    }
    service_print(service);
}

void parse_remote(FILE *file, char *line) {
	char *name;
    char *ip;
    int port;
    char *username;
    char *password;

    name = get_resource_name(line);

    if (get_str(file, &ip) <= 0) {
        fprintf(stderr, "Error getting ip in %s\n", name);
        exit(1);
    }
    if (get_str(file, &username) <= 0) {
        fprintf(stderr, "Error getting username in %s\n", name);
        exit(1);
    }
    if (get_str(file, &password) <= 0) {
        fprintf(stderr, "Error getting password in %s\n", name);
        exit(1);
    }

    Remote* remote = remote_init(name, ip, username, password);
    if (!remotes_insert(remotes, remote)) {
        fprintf(stderr, "[%s] is duplicated\n", name);
        exit(1);
    }
    remote_print(remote);
}

///////////////////////////////////////
// REMOTES and SERVICES: COLLECTIONS //
///////////////////////////////////////

Remotes* remotes_init(int capacity) {
    Remotes *remotes = malloc(sizeof(Remotes));
    remotes->capacity = capacity;
    remotes->size = 0;
    remotes->remotes = malloc(sizeof(Remote*));
    return remotes;
}
void remotes_destroy(Remotes *remotes) {
    int i;
    for (i = 0; i < remotes->size; i++) {
        free(remotes->remotes[i]);
    }
    free(remotes->remotes);
    free(remotes);
}
Services* services_init(int capacity) {
    Services *services = malloc(sizeof(Services));
    services->size = 0;
    services->capacity = capacity;
    services->services = malloc(sizeof(Service*));
    return services;
}
void services_destroy(Services *services) {
    int i;
    for (i = 0; i < services->size; i++) {
        free(services->services[i]);
    }
    free(services);
}

bool remotes_insert(Remotes *remotes, Remote *remote) {
    int i;
    for (i = 0; i < remotes->size; i++) {
        if (strcmp(remotes->remotes[i]->name, remote->name) == 0) {
            return false;
        }
    }
    if (remotes->size == remotes->capacity) {
        remotes->capacity *= 2;
        remotes->remotes = realloc(remotes->remotes, sizeof(Remote*) * remotes->capacity);
    }
    remotes->remotes[remotes->size++] = remote;
    return true;
}
bool services_insert(Services *services, Service *service) {
    int i;
    for (i = 0; i < services->size; i++) {
        if (strcmp(services->services[i]->name, service->name) == 0) {
            return false;
        }
    }
    if (services->size == services->capacity) {
        services->capacity *= 2;
        services->services = realloc(services->services, sizeof(Service*) * services->capacity);
    }
    services->services[services->size++] = service;
    return true;
}

//////////////////////////////////
// REMOTE and SERVICE FUNCTIONS //
//////////////////////////////////

Remote* remote_init(char *name, char *ip, char *username, char *password) {
	Remote *remote = malloc(sizeof(Remote));
	remote->name = name;
	remote->ip = ip;
	remote->username = username;
	remote->password = password;
	return remote;
}

void remote_print(Remote *remote) {
	printf("name: %s\n", remote->name);
	printf("ip: %s\n", remote->ip);
	printf("username: %s\n", remote->username);
	printf("password: %s\n", remote->password);
}

void remote_destroy(Remote *remote) {
    free(remote->ip);
    free(remote->name);
    free(remote->password);
    free(remote->username);
	free(remote);
}

Service* service_init(char *name, char *executable_location, char *start_command, char *remote_name) {
	Service *service = malloc(sizeof(Service));
	service->name = name;
	service->executable_location = executable_location;
	service->start_command = start_command;
	service->remote_name = remote_name;
}

void service_print(Service *service) {
    printf("name: %s\n", service->name);
    printf("executable location: %s\n", service->executable_location);
    printf("start command: %s\n", service->start_command);
    printf("remote name: %s\n", service->remote_name);
}

void service_destroy(Service *service) {
    free(service->name);
    free(service->executable_location);
    free(service->start_command);
    free(service->remote_name);
	free(service);
}

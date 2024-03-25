#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

int nrDec(int x) {
    int s = 0;
    while(x > 0) {
        s++;
        x/=10;
    }
    return s;
}

int main(int argc, char *argv[])
{
    char *host="34.254.242.81";
    int port = 8080;
    
    char *message;
    char *response;
    char *jsonStr;
    int sockfd;

    char input[BUFLEN];
    char *cookie = calloc(BUFLEN, sizeof(char));
    char *jwt = calloc(BUFLEN, sizeof(char));
    
    while(1) {
        fgets(input, BUFLEN, stdin);
        strcpy(input + strlen(input) - 1, input + strlen(input));
        if(strcmp(input, "register") == 0) {
            
            // REGISTER

            // FORM THE JSON

            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            printf("username = ");
            fgets(input, BUFLEN, stdin);
            strcpy(input + strlen(input) - 1, input + strlen(input));
            json_object_set_string(root_object, "username", input);

            printf("password = ");
            fgets(input, BUFLEN, stdin);
            strcpy(input + strlen(input) - 1, input + strlen(input));
            json_object_set_string(root_object, "password", input);
            
            if(strchr(json_object_get_string(root_object, "username"), ' ') || 
               strchr(json_object_get_string(root_object, "password"), ' ')) {
                printf("\nCan't use spaces in your registration\n\n");
                json_value_free(root_value);
                continue;
            }

            jsonStr = json_serialize_to_string(root_value);

            // SEND INFO TO SERVER
            sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request(host, "/api/v1/tema/auth/register", "application/json", 
                                        &jsonStr, 1, NULL, 0, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // PRINT RESPONSE

            if(strstr(response, "201 Created")) {
                printf("\nUser registered\n\n");
            } else {
                printf("\nRegistration failed\nUsername is used\n\n");
            }

            // free
            json_value_free(root_value);
            json_free_serialized_string(jsonStr);
            close_connection(sockfd);
            free(message);
            free(response);
        } else if(strcmp(input, "login") == 0) {
            
            // LOGIN
            
            // FORM THE JSON

            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);
            printf("username = ");
            fgets(input, BUFLEN, stdin);
            strcpy(input + strlen(input) - 1, input + strlen(input));
            json_object_set_string(root_object, "username", input);
            
            printf("password = ");
            fgets(input, BUFLEN, stdin);
            strcpy(input + strlen(input) - 1, input + strlen(input));
            json_object_set_string(root_object, "password", input);

            if(strchr(json_object_get_string(root_object, "username"), ' ') || 
               strchr(json_object_get_string(root_object, "password"), ' ')) {
                printf("\nCan't use spaces in your registration\n\n");
                json_value_free(root_value);
                continue;
            }

            jsonStr = json_serialize_to_string(root_value);

            // SEND INFO TO SERVER
            sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request(host, "/api/v1/tema/auth/login", "application/json", 
                                        &jsonStr, 1, NULL, 0, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);
            
            // SAVE COOKIE

            char *p = strstr(response, "Cookie");
            if(p!=NULL) {
                char *p1 = strtok(p+8, "\r\n");
                strcpy(cookie, p1);
            }

            // PRINT INFO

            if(strstr(response, "200 OK")) {
                printf("\nLogin succesfuly\n\n");
                memset(jwt, 0, BUFLEN);
            } else {
                if(strstr(response, "No account with this username")){
                    printf("\nNo account with this name\n\n");
                } else {
                    printf("\nCredentials are wrong\n\n");
                }
            }

            // FREE

            json_value_free(root_value);
            json_free_serialized_string(jsonStr);
            close_connection(sockfd);
            free(message);
            free(response);
            
        } else if(strcmp(input, "enter_library") == 0) {
            
            // ACCESS LIBRARY
            
            // CHECK IF LOGGED IN

            if(cookie[0]==0) {
                printf("\nNot logged in\n\n");
                continue;
            }

            // SEND INFO TO SERVER

            sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(host, "/api/v1/tema/library/access", NULL, &cookie, 1, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            char *p = strstr(response, "token");
            
            // SAVE TOKEN

            if(p != NULL) {
                char *p1 = strtok(p+8, "\"");
                strcpy(jwt, p1);
                printf("\nLibrary accesed\n\n");
            } else {
                printf("\nFailed to acces the library\n\n");
            }

            //free
            free(message);
            free(response);
            close_connection(sockfd);
        } else if(strcmp(input, "get_books") == 0) {
            
            //GET ALL BOOKS
            
            //CHECK IF LOGGED IN AND IF WE HAVE THE ACCES TO THE LIBRARY

            if(cookie[0]==0) {
                printf("\nNot logged in\n\n");
                continue;
            }
            if(jwt[0]==0) {
                printf("\nYou don't have acces to the library\n\n");
                continue;
            }

            // SEND INFO TO SERVER

            sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(host, "/api/v1/tema/library/books", NULL, &cookie, 1, jwt);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // PRINT RESPONSE

            char *p = strstr(response, "[");
            JSON_Value *root_value = json_parse_string(p);
            char* preetyJson = json_serialize_to_string_pretty(root_value);
            if(strstr(preetyJson, "[]")) {
                printf("\nThe library is empty\n\n");
            } else {
                printf("\n%s\n\n", preetyJson);
            }
            // FREE

            free(message);
            free(response);
            close_connection(sockfd);
        } else if(strcmp(input, "get_book") == 0) {
            
            // GET BOOK
            
            // GET ID

            printf("id = ");
            char idString[10];
            int id = 0;
            fgets(idString, 10, stdin);

            // CHECK IF LOGGED IN AND IF WE HAVE ACCES TO THE LIBRARY
            if(cookie[0]==0) {
                printf("Not logged in\n");
                continue;
            }
            if(jwt[0]==0) {
                printf("You don't have acces to the library\n");
                continue;
            }

            strcpy(idString + strlen(idString) - 1, idString + strlen(idString));
            id = atoi(idString);
            if(id <= 0 || nrDec(id) != strlen(idString)) {
                printf("Invalid id\n");
                continue;
            }
            char *url = calloc(BUFLEN, sizeof(char));
            sprintf(url, "/api/v1/tema/library/books/%d", id);

            // SEND INFO TO SERVER

            sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
            message = compute_get_request(host, url, NULL, &cookie, 1, jwt);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // PRINT RESPONSE

            if(strstr(response, "error")) {
                printf("\nNo book was found!\n\n");
                continue;
            }

            char *p = strstr(response, "{");
            JSON_Value *root_value = json_parse_string(p);
            char* preetyJson = json_serialize_to_string_pretty(root_value);
            printf("\n%s\n\n", preetyJson);

            // FREE

            free(url);
            free(message);
            free(response);
            close_connection(sockfd);
        } else if(strcmp(input, "add_book") == 0) {

            //ADD BOOK
            
            // CHECK IF LOGGED IN AND IF WE HAVE ACCES TO THE LIBRARY

            if(cookie[0]==0) {
                printf("\nNot logged in\n\n");
                continue;
            }
            if(jwt[0]==0) {
                printf("\nYou don't have acces to the library\n\n");
                continue;
            }
            
            // FORM INPUT

            JSON_Value *root_value = json_value_init_object();
            JSON_Object *root_object = json_value_get_object(root_value);

            int ok = 1; // used to see if the input is ok

            printf("title = ");
            fgets(input, BUFLEN, stdin);
            strcpy(input + strlen(input) - 1, input + strlen(input));
            if(strlen(input) < 1) {
                ok = 0;
            } else {
                json_object_set_string(root_object, "title", input);
            }
            
            printf("author = ");
            fgets(input, BUFLEN, stdin);
            strcpy(input + strlen(input) - 1, input + strlen(input));
            if(strlen(input) < 1) {
                ok = 0;
            } else {
                json_object_set_string(root_object, "author", input);
            }
            
            printf("genre = ");
            fgets(input, BUFLEN, stdin);
            strcpy(input + strlen(input) - 1, input + strlen(input));
            if(strlen(input) < 1) {
                ok = 0;
            } else {
                json_object_set_string(root_object, "genre", input);
            }
            
            int pageCount, comparison;
            printf("page_count = ");
            fgets(input, BUFLEN, stdin);
            strcpy(input + strlen(input) - 1, input + strlen(input));
            if(strlen(input) < 1) {
                ok = 0;
            } else {
                pageCount = atoi(input);
                comparison = strlen(input);
                if(pageCount <= 0 || nrDec(pageCount) != comparison) {
                    ok = 0;
                } else {
                    json_object_set_number(root_object, "page_count", pageCount);
                }
            }
            
            printf("publisher = ");
            fgets(input, BUFLEN, stdin);
            strcpy(input + strlen(input) - 1, input + strlen(input));
            if(strlen(input) < 1) {
                ok = 0;
            } else {
                json_object_set_string(root_object, "publisher", input);
            }
            
            if(ok == 0) {
                json_value_free(root_value);
                printf("\nInvalid format\n\n");
                continue;
            }

            jsonStr = json_serialize_to_string(root_value);

            // SEND INFO TO SERVER
            sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
            message = compute_post_request(host, "/api/v1/tema/library/books", "application/json", 
                                        &jsonStr, 1, NULL, 0, jwt);            
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // PRINT RESPONSE

            if(strstr(response, "200 OK")) {
                printf("\nBook succesfuly added\n\n");
            } else {
                printf("\nFailed to add the book\n\n");
            }

            // FREE

            free(message);
            free(response);
            close_connection(sockfd);
        } else if(strcmp(input, "delete_book") == 0) {
            
            // REMOVE BOOK

            // CHECK IF LOGGED IN AND IF WE HAVE ACCES TO THE LIBRARY

            if(cookie[0]==0) {
                printf("Not logged in\n");
                continue;
            }
            if(jwt[0]==0) {
                printf("You don't have acces to the library\n");
                continue;
            }

            // FORM INPUT

            printf("id = ");
            char idString[10];
            int id = 0;
            fgets(idString, 10, stdin);
            strcpy(idString + strlen(idString) - 1, idString + strlen(idString));
            id = atoi(idString);
            if(id <= 0 || nrDec(id) != strlen(idString)) {
                printf("Invalid id\n");
                continue;
            }
            char *url = calloc(BUFLEN, sizeof(char));
            sprintf(url, "/api/v1/tema/library/books/%d", id);

            // SEND INFO TO SERVER

            sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);
            message = compute_delete_request(host, url, NULL, 0, jwt);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // PRINT RESPONSE

            if(strstr(response, "404 Not Found")) {
                printf("\nNo book was found with this id\n\n");
            } else {
                printf("\nSuccesfully removed the book\n\n");
            }

            // FREE

            free(url);
            free(message);
            free(response);
            close_connection(sockfd);
        } else if(strcmp(input, "logout") == 0) {

            // LOGOUT

            // CHECK IF LOGGED IN

            if(cookie[0] == 0 || cookie == NULL) {
                printf("\nNot logged in\n\n");
                continue;
            }
            
            sockfd = open_connection(host, port, AF_INET, SOCK_STREAM, 0);

            // SEND INFO TO SERVER

            message = compute_get_request(host, "/api/v1/tema/auth/logout", NULL, &cookie, 1, NULL);
            send_to_server(sockfd, message);
            response = receive_from_server(sockfd);

            // PRINT RESPONSE

            if(strstr(response, "200 OK")) {
                memset(cookie, 0, BUFLEN);
                printf("\nLogged out\n\n");
            } else {
                printf("\nFailed to log you out\n\n");
            }

            // FREE

            if(jwt[0]) {
                memset(jwt, 0, BUFLEN);
            }
            free(message);
            free(response);
            close_connection(sockfd);
        } else if(strcmp(input, "exit") == 0) {

            // PREPARE FOR EXIT
            // FREE MEMORY
            free(cookie);
            free(jwt);
            break;
        } else {
            printf("wrong command\n");
        }
    }
    
    return 0;
}

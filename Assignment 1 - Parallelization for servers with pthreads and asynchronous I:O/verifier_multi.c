#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <getopt.h>
#include "common.h"

/* Check the common header for the definition of puzzle */

#define URL "http://berkeley.uwaterloo.ca:4590/verify"
#define ROW_LENGTH 20
#define MATRIX_LENGTH 202
#define MAX_WAIT_MSECS 30*1000 /* Wait max. 30 seconds */

const char *ROW_FORMAT = "[%d,%d,%d,%d,%d,%d,%d,%d,%d]";
const char *MATRIX_FORMAT = "{\"content\":[%s, %s, %s, %s, %s, %s, %s, %s, %s]}";

int num_connections = 1;

FILE *inputfile;

/* Create cURL easy handle and configure it */
CURL *create_eh(const int *result_code, const char *json_to_send, const struct curl_slist *headers);

/* Configure headers for the cURL request */
struct curl_slist *config_headers();

/* Transform the puzzle into an appropriate json format for the server */
char *convert_to_json(puzzle *p);

/* cURL read callback */
size_t read_callback(char *buffer, size_t size, size_t nitems, void *userdata);

/* cURL write callback */
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata);

void multi_verify();

int main(int argc, char **argv) {
    /* Parse arguments */
    int c;
    char* filename = NULL;
    while ((c = getopt(argc, argv, "t:i:")) != -1) {
        switch (c) {
            case 't':
                num_connections = strtoul(optarg, NULL, 10);
                if (num_connections == 0) {
                    printf("%s: option requires an argument > 0 -- 't'\n", argv[0]);
                    return EXIT_FAILURE;
                }
                break;
            case 'i':
                filename = optarg;
                break;
            default:
                return -1;
        }
    }

    /* Open file */
    inputfile = fopen(filename, "r");
    if (inputfile == NULL) {
        printf("Unable to open file!\n");
        return EXIT_FAILURE;
    }

    curl_global_init(CURL_GLOBAL_ALL);

    /* Check puzzles */
    multi_verify();

    curl_global_cleanup();
    fclose( inputfile );
    return 0;
}

void multi_verify() {
    CURLM *cm = curl_multi_init();

    char *converted;
    struct curl_slist *headers_list[num_connections];

    int total_puzzles = 0;
    int verified = 0;
    int still_running = 0;

    CURL *eh;

    int results[num_connections];
    int idx = 0;

    puzzle *p;

    // Read in the puzzle one by one
    while ((p = read_next_puzzle(inputfile)) != NULL) {
        total_puzzles ++;

        converted = convert_to_json(p);
        headers_list[idx] = config_headers();

        // Initalize the easy handle and pass in the result pointer and header pointer
        eh = create_eh(&results[idx], converted, headers_list[idx]);
        // Add easy handle to the multi handle
        curl_multi_add_handle( cm, eh );

        // If the number of easy handle reaches num_connections, we dispatch them all at once with curl_multi_perform
        if (idx == num_connections - 1) {
            curl_multi_perform(cm, &still_running);
            do {
                int numfds = 0;
                int res = curl_multi_wait(cm, NULL, 0, MAX_WAIT_MSECS, &numfds);
                if (res != CURLM_OK) {
                    return exit(EXIT_FAILURE);
                }

                curl_multi_perform(cm, &still_running);
            } while (still_running);
            
            // When still_running = 0 and all the request are finished
            // We collect all the result from the result array
            for (int i = 0; i < num_connections; i++) {
                verified += results[i];
                results[i] = 0;
                curl_slist_free_all(headers_list[i]);
            } 

            idx = 0;
        } else {
            idx ++;
        }

        free(p);
    }
    
    // Similary to above. Handling what left in the multi handlers
    if (idx > 0) {
        curl_multi_perform(cm, &still_running);
        do {
            int numfds = 0;
            int res = curl_multi_wait(cm, NULL, 0, MAX_WAIT_MSECS, &numfds);
            if (res != CURLM_OK) {
                return exit(EXIT_FAILURE);
            }

            curl_multi_perform(cm, &still_running);
        } while (still_running);
        
        for (int i = 0; i < idx; i++) {
            verified += results[i];
            results[i] = 0;
            curl_slist_free_all(headers_list[i]);
        } 
    }

    CURLMsg *msg = NULL;
    int msgs_left = 0;
    
    // Check the message, return code and response code for each requests
    while ((msg = curl_multi_info_read(cm, &msgs_left))) {
        if (msg->msg == CURLMSG_DONE) {
            eh = msg->easy_handle;

            CURLcode res = msg->data.result;
            if (res != CURLE_OK) {
                printf("Error occurred in executing the cURL request: %s\n",
                    curl_easy_strerror(res));
                exit(EXIT_FAILURE);
            }

            long response_code;
            curl_easy_getinfo(eh, CURLINFO_RESPONSE_CODE, &response_code);
            if (response_code != 200) {
                printf("Error in HTTP request; HTTP code %lu received.\n", response_code);
            }

            curl_multi_remove_handle(cm, eh);
            curl_easy_cleanup(eh);
        } else {
            printf("Error after curl multi info read(), CURLMsg=%d\n", msg->msg);
        }
    }

    // Print the final result
    printf("%d of %d puzzles passed verification.\n", verified, total_puzzles);

    curl_multi_cleanup(cm);
}

char *convert_to_json(puzzle *p) {
    char *rows[9];
    for (int i = 0; i < 9; i++) {
        rows[i] = malloc(ROW_LENGTH);
        memset(rows[i], 0, ROW_LENGTH);
        int written = sprintf(rows[i], ROW_FORMAT,
                              p->content[i][0], p->content[i][1], p->content[i][2],
                              p->content[i][3], p->content[i][4], p->content[i][5],
                              p->content[i][6], p->content[i][7], p->content[i][8]);
        if (written != ROW_LENGTH - 1) {
            printf("Something went wrong when writing row; expected to write %d but wrote %d...\n",
                    ROW_LENGTH -1, written);
        }
    }
    char *json = malloc(MATRIX_LENGTH);
    memset(json, 0, MATRIX_LENGTH);
    int written = sprintf(json, MATRIX_FORMAT,
                          rows[0], rows[1], rows[2], rows[3], rows[4],
                          rows[5], rows[6], rows[7], rows[8]);
    if (written != MATRIX_LENGTH - 1) {
        printf("Something went wrong when writing matrix; expected to write %d but wrote %d...\n",
               MATRIX_LENGTH -1, written);
    }
    for (int i = 0; i < 9; i++) {
        free(rows[i]);
    }
    return json;
}

size_t read_callback(char *buffer, size_t size, size_t nitems, void *userdata) {
    memcpy(buffer, userdata, MATRIX_LENGTH);
    free(userdata);
    return MATRIX_LENGTH;
}

size_t write_callback(char *ptr, size_t size, size_t nmemb, void  *userdata) {
    printf("Write callback message from server: %s\n", ptr);
    int * p = (int*) userdata;
    *p = atoi(ptr);
    return size * nmemb;
}

struct curl_slist *config_headers() {
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Expect:");
    return headers;
}

CURL *create_eh(const int *result, const char *json_to_send, const struct curl_slist *headers) {
    CURL *eh = curl_easy_init();
    curl_easy_setopt(eh, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(eh, CURLOPT_URL, URL);
    curl_easy_setopt(eh, CURLOPT_POST, 1L);
    curl_easy_setopt(eh, CURLOPT_READFUNCTION, read_callback);
    curl_easy_setopt(eh, CURLOPT_READDATA, json_to_send);
    curl_easy_setopt(eh, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(eh, CURLOPT_WRITEDATA, result);
    curl_easy_setopt(eh, CURLOPT_POSTFIELDSIZE, MATRIX_LENGTH);
    return eh;
}



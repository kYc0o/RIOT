
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>

#include "fw_requests.h"
#include "net/ipv6/addr.h"
#include "clist.h"

clist_node_t art_to_file;
clist_node_t requests_as_server;

static const char* reposotory_base_path = "firmwares";
int sockfd;

/* create artifact list */
static void fill_repository(void)
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char path[255];

    sprintf(path, "%s/repository.repo", reposotory_base_path);

    fp = fopen(path, "r");
    if (fp == NULL) {
        printf("[server] No repository found, exiting...\n");
        exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, fp)) != -1) {
        char* tmp = strstr(line, "\n");
        *tmp = 0;
        tmp = strstr(line, " ");
        *tmp = 0;
        tmp++;

        if (find_by_artifact(&art_to_file, line) == NULL) {
            struct map_entry *entry = (struct map_entry*)malloc(sizeof(struct map_entry));
            sprintf(path, "%s/%s", reposotory_base_path, tmp);
            entry->artifact = strdup(line);
            entry->filename = strdup(path);
            clist_rpush(&art_to_file, (clist_node_t*)entry);
            printf("[server] %s => %s\n", entry->artifact, entry->filename);
        }
    }

    free(line);
    fclose(fp);
}

static void server_on_new_summary(uint16_t session_id, uint16_t nr_chunks);
static void server_on_new_chunk(uint16_t chunk_id, uint16_t len, const uint8_t *data);
static void server_on_ack_artifact_request(void);
static void server_on_artifact_request(const ipv6_addr_t *source_address,
                                       const char *artifact);
static void server_on_chunk_request(uint16_t session_id, uint16_t chunk_id);
static void server_on_repo_addr_request(const ipv6_addr_t *src, ipv6_addr_t *req_src,
                                        char *fw_name);
static void server_on_repo_addr(const ipv6_addr_t *src, char *fw_name);

static const struct request_processing_callback server_request_callbacks = {
        .on_new_summary = server_on_new_summary,
        .on_new_chunk = server_on_new_chunk,
        .on_ack_firmware_request = server_on_ack_artifact_request,
        .on_firmware_request = server_on_artifact_request,
        .on_chunk_request = server_on_chunk_request,
        .on_repo_request = server_on_repo_addr_request,
        .on_repo_addr = server_on_repo_addr
};

static void server_on_new_summary(uint16_t session_id, uint16_t nr_chunks)
{
    (void) session_id;
    (void) nr_chunks;
    printf("[server] ERROR: Everything is wrong %s:%d\n", __FILE__, __LINE__);
}
static void server_on_new_chunk(uint16_t chunk_id, uint16_t len, const uint8_t *data)
{
    (void) chunk_id;
    (void) len;
    (void) data;
    printf("[server] ERROR: Everything is wrong %s:%d\n", __FILE__, __LINE__);
}

static void server_on_ack_artifact_request(void)
{
    printf("[server] ERROR: Everything is wrong %s:%d\n", __FILE__, __LINE__);
}

static int is_artifact_location_known(struct firmware_request *req)
{
    struct map_entry *entry = find_by_artifact(&art_to_file, req->firmware_name);
    return entry != NULL;
}

static void server_on_repo_addr(const ipv6_addr_t *src, char *fw_name)
{
    (void) fw_name;
    char src_addr[IPV6_ADDR_MAX_STR_LEN];
    printf("[server] ERROR: Everything is wrong from %s at %s:%d\n",
            ipv6_addr_to_str(src_addr, src, sizeof(src_addr)),
            __FILE__, __LINE__);
}

static void prepare_request_with_artifact(struct firmware_request *req,
                                          const char *artifact)
{
    struct map_entry *entry = find_by_artifact(&art_to_file, artifact);
    req->fd = open(entry->filename, O_RDONLY);

    struct stat buf;
    stat(entry->filename, &buf);
    uint32_t s = buf.st_size;
    req->current_packet = 0;
    req->nr_packets = s / REQUEST_PACKET_SIZE;
    if (s % REQUEST_PACKET_SIZE) req->nr_packets++;
}

static void server_on_artifact_request(const ipv6_addr_t *source_address,
                                       const char *artifact)
{
    char src_addr[IPV6_ADDR_MAX_STR_LEN];

    (void) ipv6_addr_to_str(src_addr, source_address, sizeof(src_addr));
    printf("[server] Address '%s', Artifact: '%s'\n", src_addr, artifact);

    struct firmware_request *req = find_request_by_source(&requests_as_server,
                                                          src_addr, artifact);

    if (req == NULL) {
        /* a new request */
        req = create_request(src_addr, artifact, SENDING_SUMMARY);
        clist_rpush(&requests_as_server, (clist_node_t*)req);
    }

    /* requesting the same */
    if (req->current_state == SENDING_SUMMARY ||
        req->current_state == WAITING_FOR_LOCATION) {
        /* send summary to the source */
        if (is_artifact_location_known(req)) {
            printf("[server] Sending summary\n");

            /* filling the information about the packet */
            prepare_request_with_artifact(req, artifact);
            struct firmware_packet pkt;
            build_summary_packet(&pkt, req->session_id, req->nr_packets);

            /* send response back */
            struct sockaddr_in6 cliaddr;
            cliaddr.sin6_family = AF_INET6;
            inet_pton(AF_INET6, src_addr, &cliaddr.sin6_addr);
            cliaddr.sin6_port = htons(FW_SERVER_UDP_PORT);
            sendto(sockfd, &pkt, total_len(&pkt), 0, (struct sockaddr *)&cliaddr,
                   sizeof(cliaddr));
        }
        else {
            printf("[server] Wrong <=======\n");
            exit(EXIT_FAILURE);
        }
    }
}
static void server_on_chunk_request(uint16_t session_id, uint16_t chunk_id)
{
    struct firmware_request *req = find_request_by_session(&requests_as_server,
                                                           session_id);

    if (req != NULL && ((req->current_state == SENDING_CHUNKS &&
                         req->current_packet <= chunk_id) ||
                         (req->current_state == SENDING_SUMMARY &&
                          req->current_packet == 0 && chunk_id == 0))) {

        req->current_packet = chunk_id;

        /* modifying the state */
        req->current_state = SENDING_CHUNKS;

        printf("[server] Session Id '%d', Chunk Id: '%d'. %d %d '%s'\n",
                session_id, chunk_id, req->current_state, req->current_packet,
                req->source_address);

        /* read from the file */
        uint8_t buf[REQUEST_PACKET_SIZE];
        lseek(req->fd, chunk_id * REQUEST_PACKET_SIZE, SEEK_SET);
        int n = read(req->fd, buf, REQUEST_PACKET_SIZE);

        /* build the packet */
        struct firmware_packet pkt;
        build_chunk_packet(&pkt, chunk_id, n, buf);

        /* send response back */
        struct sockaddr_in6 cliaddr;
        cliaddr.sin6_family = AF_INET6;
        inet_pton(AF_INET6, req->source_address, &cliaddr.sin6_addr);
        cliaddr.sin6_port = htons(FW_SERVER_UDP_PORT);
        sendto(sockfd, &pkt, total_len(&pkt), 0, (struct sockaddr*)&cliaddr,
               sizeof(cliaddr));
    }
}

static void server_on_repo_addr_request(const ipv6_addr_t *src, ipv6_addr_t *req_src,
                                        char *fw_name)
{
    (void) src;
    char src_addr[IPV6_ADDR_MAX_STR_LEN];
    struct firmware_packet pkt;
    struct sockaddr_in6 cliaddr;

    build_repo_addr(&pkt, fw_name);

    cliaddr.sin6_family = AF_INET6;

    printf("[server] Receiving request from: %s\n", ipv6_addr_to_str(src_addr,
            req_src, sizeof(src_addr)));

    inet_pton(AF_INET6, ipv6_addr_to_str(src_addr, req_src, sizeof(src_addr)),
              &cliaddr.sin6_addr);
    cliaddr.sin6_port = htons(FW_SERVER_UDP_PORT);
    sendto(sockfd, &pkt, total_len(&pkt), 0, (struct sockaddr*)&cliaddr,
           sizeof(cliaddr));
}

int main (int argc, char *argv[])
{
    (void) argc;
    (void) argv;
    int n;
    struct sockaddr_in6 servaddr;
    struct sockaddr_in6 cliaddr;
    socklen_t len;
    uint8_t mesg[1000];
    char address_name[80];
    ipv6_addr_t ip;

    sockfd = socket(AF_INET6, SOCK_DGRAM, 0);
    if (sockfd < 0)  {
        printf("[server] ERROR: no socket available\n");
        return -1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin6_family = AF_INET6;
    servaddr.sin6_addr = in6addr_any; //IN6ADDR_ANY_INIT;//htonl(IN6ADDR_ANY_INIT);
    servaddr.sin6_port = htons(FW_SERVER_UDP_PORT);

    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
        printf("[server] ERROR: no binding available %d:%s\n", errno, strerror(errno));
        return -1;
    }

    printf("[server] Server started with size: %lu\n", sizeof(struct firmware_packet));
    fill_repository();
    /* loop forever */
    for (;;)
    {
        char src_addr[IPV6_ADDR_MAX_STR_LEN];
        len = sizeof(cliaddr);
        n = recvfrom(sockfd, (char*)mesg, 1000, 0, (struct sockaddr *)&cliaddr,
                     &len);
        inet_ntop(AF_INET6, (struct sockaddr *)&cliaddr.sin6_addr, address_name, 80);
        (void) ipv6_addr_to_str(address_name, &ip, sizeof(address_name));
        /*sscanf(address_name, "%02x%02x::%02x%02x", &(ip.u8[0]), &(ip.u8[1]),
               &(ip.u8[14]), &(ip.u8[15]));*/

        printf("[server] Processing message from: %s\n",
                ipv6_addr_to_str(src_addr, &ip, sizeof(src_addr)));

        enum message_processing_error_code err = process_message(&ip, n, mesg,
                                                                 &server_request_callbacks);

        if (err != NONE) {
            printf("[server] Error %d processing message\n", err);
        }
    }

    return 0;
}

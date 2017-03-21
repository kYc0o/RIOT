
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
static int fill_repository()
{
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	char path[255];

	sprintf(path, "%s/repository.repo", reposotory_base_path);

	fp = fopen(path, "r");
	if (fp == NULL)
		exit(EXIT_FAILURE);

	while ((read = getline(&line, &len, fp)) != -1) {
		char* tmp = strstr(line, "\n");
		*tmp = 0;
		tmp = strstr(line, " ");
		*tmp = 0;
		tmp++;

		if (find_by_artifact(art_to_file, line) == NULL) {


			struct map_entry* entry = (struct map_entry*)malloc(sizeof(struct map_entry));
			sprintf(path, "%s/%s", reposotory_base_path, tmp);
			entry->artifact = strdup(line);
			entry->filename = strdup(path);
			list_add(art_to_file, entry);
			printf("%s => %s\n", entry->artifact, entry->filename);
		}
	}

	free(line);

	fclose(fp);
}

static void server_onNewSummary(uint16_t session_id, uint16_t nr_chunks);
static void server_onNewChunk(uint16_t chunk_id, uint16_t len, const uint8_t* data);
static void server_onAckArtifactRequest(void);
static void server_onArtifactRequest(const uip_ipaddr_t* source_address, const char* artifact);
static void server_onChunkRequest(uint16_t session_id, uint16_t chunk_id);
static void server_onRepoAddrRequest(uip_ipaddr_t *src, uip_ipaddr_t *reqSrc, char *duName);
static void server_onRepoAddr(uip_ipaddr_t *src);

static const struct request_processing_callback server_request_callbacks = {
		.on_new_summary = server_onNewSummary,
		.on_new_chunk = server_onNewChunk,
		.on_ack_firmware_request = server_onAckArtifactRequest,
		.on_firmware_request = server_onArtifactRequest,
		.on_chunk_request = server_onChunkRequest,
		.on_repo_request = server_onRepoAddrRequest,
		.on_repo_addr = server_onRepoAddr
};

static void
server_onNewSummary(uint16_t session_id, uint16_t nr_chunks)
{
	printf("ERROR: Everything is wrong %s:%d\n", __FILE__, __LINE__);
}
static void
server_onNewChunk(uint16_t chunk_id, uint16_t len, const uint8_t* data)
{
	printf("ERROR: Everything is wrong %s:%d\n", __FILE__, __LINE__);
}

static void
server_onAckArtifactRequest(void)
{
	printf("ERROR: Everything is wrong %s:%d\n", __FILE__, __LINE__);
}

static int
is_artifact_location_known(struct firmware_request* req)
{
	struct map_entry* entry = find_by_artifact(art_to_file, req->firmware_name);
	return entry != NULL;
}

static void
prepare_request_with_artifact(struct firmware_request* req, const char* artifact)
{
	struct map_entry* entry = find_by_artifact(art_to_file, req->firmware_name);
	req->fd = open(entry->filename, O_RDONLY);

	struct stat buf;
	stat(entry->filename, &buf);
	uint32_t s = buf.st_size;
	req->current_packet = 0;
	req->nr_packets = s / REQUEST_PACKET_SIZE;
	if (s % REQUEST_PACKET_SIZE) req->nr_packets++;

}

static void
server_onArtifactRequest(const uip_ipaddr_t* source_address, const char* artifact)
{
	char *srcAddr = convertAddr(source_address);
	printf("Address '%s', Artifact: '%s'\n", srcAddr, artifact);


	struct firmware_request* req = find_request_by_source(requests_as_server, srcAddr, artifact);

	int is_new = req == NULL;

	if (req == NULL) {
		/* a new request */
		req = create_request(srcAddr, artifact, SENDING_SUMMARY);
		list_add(requests_as_server, req);
	}

	/* requesting the same */
	if (req->current_state == SENDING_SUMMARY || req->current_state == WAITING_FOR_LOCATION) {
		/* send summary to the source */

		if (is_artifact_location_known(req)) {
			printf("\tSending summary\n");
			/* filling the information about the packet */
			prepare_request_with_artifact(req, artifact);
			struct firmware_packet pkt;
			build_summary_packet(&pkt, req->session_id, req->nr_packets);

			/* send response back */
			struct sockaddr_in6 cliaddr;
			cliaddr.sin6_family = AF_INET6;
			inet_pton(AF_INET6, srcAddr, &cliaddr.sin6_addr);
			cliaddr.sin6_port=htons(PORT);
			sendto(sockfd, &pkt, total_len(&pkt), 0, (struct sockaddr *)&cliaddr,sizeof(cliaddr));
		}
		else {

			printf("Wrong <=======\n");

			while(1) {  };
		}
	}
}
static void
server_onChunkRequest(uint16_t session_id, uint16_t chunk_id)
{
	struct firmware_request* req = find_request_by_session(requests_as_server, session_id);
	char _reqSrc[16];

	if (req != NULL && 
			((req->current_state == SENDING_CHUNKS && req->current_packet <= chunk_id) ||
					(req->current_state == SENDING_SUMMARY && req->current_packet == 0 && chunk_id == 0))  ) {

		req->current_packet = chunk_id;

		/* modifying the state */
		req->current_state = SENDING_CHUNKS;

		printf("Session Id '%d', Chunk Id: '%d'. %d %d '%s'\n", session_id, chunk_id, req->current_state, req->current_packet, req->source_address);
		/* read from the file */
		uint8_t buf [REQUEST_PACKET_SIZE];
		lseek(req->fd, chunk_id * REQUEST_PACKET_SIZE, SEEK_SET);
		int n = read(req->fd, buf, REQUEST_PACKET_SIZE);

		/* build the packet */
		struct firmware_packet pkt;
		build_chunk_packet(&pkt, chunk_id, n, buf);

		/* send response back */
		struct sockaddr_in6 cliaddr;
		cliaddr.sin6_family = AF_INET6;
		inet_pton(AF_INET6, req->source_address, &cliaddr.sin6_addr);
		cliaddr.sin6_port=htons(PORT);
		sendto(sockfd, &pkt, total_len(&pkt), 0, (struct sockaddr *)&cliaddr,sizeof(cliaddr));
	}
}

static void
server_onRepoAddrRequest(uip_ipaddr_t *src, uip_ipaddr_t *reqSrc, char *duName)
{
	struct firmware_packet pkt;
	build_repo_addr(&pkt, duName);
	struct sockaddr_in6 cliaddr;
	cliaddr.sin6_family = AF_INET6;
	printf("Receiving request from: %s\n", convertAddr(reqSrc));
	inet_pton(AF_INET6, convertAddr(reqSrc), &cliaddr.sin6_addr);
	cliaddr.sin6_port=htons(PORT);
	sendto(sockfd, &pkt, total_len(&pkt), 0, (struct sockaddr *)&cliaddr,sizeof(cliaddr));
}

static void
server_onRepoAddr(uip_ipaddr_t *src)
{
	printf("ERROR: Everything is wrong from %s at %s:%d\n", convertAddr(src), __FILE__, __LINE__);
}


int
main (int argc, char *argv[])
{
	uip_ipaddr_t ip;
	list_init(art_to_file);
	fill_repository();

	list_init(requests_as_server);

	printf("Server started with size: %d\n", sizeof(struct firmware_packet));

	int n;
	struct sockaddr_in6 servaddr;
	struct sockaddr_in6 cliaddr;
	socklen_t len;
	uint8_t mesg[1000];
	char address_name[80];

	sockfd = socket(AF_INET6,SOCK_DGRAM,0);

	if (sockfd < 0)  {
		printf("ERROR: no socket available\n");
		return -1;
	}

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin6_family = AF_INET6;
	servaddr.sin6_addr=in6addr_any; //IN6ADDR_ANY_INIT;//htonl(IN6ADDR_ANY_INIT);
	servaddr.sin6_port=htons(PORT);
	if (bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) != 0) {
		printf("ERROR: no binding available %d:%s\n", errno, strerror(errno));
		return -1;
	} 

	/* loop forever */
	for (;;)
	{
		len = sizeof(cliaddr);
		n = recvfrom(sockfd,(char*)mesg,1000,0,(struct sockaddr *)&cliaddr,&len);
		inet_ntop(AF_INET6, (struct sockaddr *)&cliaddr.sin6_addr, address_name, 80);
		sscanf(address_name, "%02x%02x::%02x%02x", &(ip.u8[0]), &(ip.u8[1]), &(ip.u8[14]), &(ip.u8[15]));
		printf("Processing message from: %s\n", convertAddr(&ip));
		enum message_processing_error_code err = process_message(&ip, n, mesg, &server_request_callbacks);
		if (err != NONE) {
			printf("Error %d processing message\n", err);
		}
	}

	return 0;
}

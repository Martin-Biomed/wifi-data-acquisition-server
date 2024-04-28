

#include "ping.h"

// Code has been modified from its original version in: https://github.com/nopnop2002/esp-idf-ping/blob/master/main/ping.c

// This value is kept in a static variable so we can access the latest value persistently from outside this module
static char host_ip[HOST_IP_MAX_LEN];
static int icmp_seq_num;
static int ttl_num;
static int elapsed_time_num;

// This value changes to 0 once a (ping_success) or (ping_failure) has finished execution.
// This is being used to synchronise wifi_connect and the ping thread because we don't have direct access to functions in ping/ping_sock.c
static int ping_resolved = -1;


int get_icmp_seq_num(void){
	return icmp_seq_num;
}

int get_ttl_num(void){
	return ttl_num;
}

int get_elapsed_time_num(void){
	return elapsed_time_num;
}

char* get_host_ip(void){
	return host_ip;
}

// This function is used for synchronisation between (ping) and (wifi_connect)
int get_ping_resolved_status(void){
	return ping_resolved;
}

// This function is called when the ping to the selected host has succeeded
void cmd_ping_on_ping_success(esp_ping_handle_t hdl, void *args)
{
		uint8_t ttl;
		uint16_t seqno;
		uint32_t elapsed_time, recv_len;
		ip_addr_t target_addr;
		esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
		esp_ping_get_profile(hdl, ESP_PING_PROF_TTL, &ttl, sizeof(ttl));
		esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
		esp_ping_get_profile(hdl, ESP_PING_PROF_SIZE, &recv_len, sizeof(recv_len));
		esp_ping_get_profile(hdl, ESP_PING_PROF_TIMEGAP, &elapsed_time, sizeof(elapsed_time));

		ESP_LOGI(PING_TAG, "%"PRIu32" bytes from %s icmp_seq=%d ttl=%d time=%"PRIu32" ms",
				recv_len, inet_ntoa(target_addr.u_addr.ip4), seqno, ttl, elapsed_time);
		
		strcpy(host_ip, inet_ntoa(target_addr.u_addr.ip4));
		icmp_seq_num = seqno;
		ttl_num = ttl;
		elapsed_time_num = (int)elapsed_time;
		
		ping_resolved = 0;
		esp_ping_delete_session(hdl);
}

// This function is called when the ping to the selected host has timed out
void cmd_ping_on_ping_timeout(esp_ping_handle_t hdl, void *args)
{
	uint16_t seqno;
	ip_addr_t target_addr;
	esp_ping_get_profile(hdl, ESP_PING_PROF_SEQNO, &seqno, sizeof(seqno));
	esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
	ESP_LOGW(PING_TAG, "From %s icmp_seq=%d timeout", inet_ntoa(target_addr.u_addr.ip4), seqno);

	ping_resolved = 0;
	esp_ping_delete_session(hdl);
}

// This function is called when the ping to the selected host has failed
void cmd_ping_on_ping_end(esp_ping_handle_t hdl, void *args)
{
	ip_addr_t target_addr;
	uint32_t transmitted;
	uint32_t received;
	uint32_t total_time_ms;
	esp_ping_get_profile(hdl, ESP_PING_PROF_REQUEST, &transmitted, sizeof(transmitted));
	esp_ping_get_profile(hdl, ESP_PING_PROF_REPLY, &received, sizeof(received));
	esp_ping_get_profile(hdl, ESP_PING_PROF_IPADDR, &target_addr, sizeof(target_addr));
	esp_ping_get_profile(hdl, ESP_PING_PROF_DURATION, &total_time_ms, sizeof(total_time_ms));
	uint32_t loss = (uint32_t)((1 - ((float)received) / transmitted) * 100);
	if (IP_IS_V4(&target_addr)) {
		ESP_LOGI(PING_TAG, "\n--- %s ping statistics ---", inet_ntoa(*ip_2_ip4(&target_addr)));
	} else {
		ESP_LOGI(PING_TAG, "\n--- %s ping statistics ---", inet6_ntoa(*ip_2_ip6(&target_addr)));
	}
	ESP_LOGI(PING_TAG, "%"PRIu32" packets transmitted, %"PRIu32" received, %"PRIu32"%% packet loss, time %"PRIu32"ms",
			 transmitted, received, loss, total_time_ms);
	
	ping_resolved = 0;

	// delete the ping sessions, so that we clean up all resources and can create a new ping session
	// we don't have to call delete function in the callback, instead we can call delete function from other tasks
	ESP_LOGI(PING_TAG, "Deleting the ping session....");
	esp_ping_delete_session(hdl);
}

/*
interval_ms:ping interval mSec. Default is 1000mSec.
task_prio:ping task priority. Default is 2.
target_host:target host url. if null,target is own gateway.
*/
esp_err_t initialize_ping(uint32_t interval_ms, uint32_t task_prio, char * target_host)
{

	// Re-initialise the value of static variables
	icmp_seq_num = -1;
	ttl_num = -1;
	elapsed_time_num = -1;
	memset(host_ip, 0, sizeof(host_ip));
	ping_resolved = -1;

	esp_ping_config_t ping_config = ESP_PING_DEFAULT_CONFIG();

	if (strlen(target_host) > 0) {
		/* convert URL to IP address */
		ip_addr_t target_addr;
		memset(&target_addr, 0, sizeof(target_addr));
		struct addrinfo hint;
		memset(&hint, 0, sizeof(hint));
		struct addrinfo *res = NULL;
		//int err = getaddrinfo("www.espressif.com", NULL, &hint, &res);
		int err = getaddrinfo(target_host, NULL, &hint, &res);
		if(err != 0 || res == NULL) {
			ESP_LOGE(PING_TAG, "DNS lookup failed err=%d res=%p", err, res);
			return ESP_FAIL;
		} else {
			ESP_LOGI(PING_TAG, "DNS lookup success");
		}

		if (res->ai_family == AF_INET) {
			struct in_addr addr4 = ((struct sockaddr_in *) (res->ai_addr))->sin_addr;
			inet_addr_to_ip4addr(ip_2_ip4(&target_addr), &addr4);
		} else {
			struct in6_addr addr6 = ((struct sockaddr_in6 *) (res->ai_addr))->sin6_addr;
			inet6_addr_to_ip6addr(ip_2_ip6(&target_addr), &addr6);
		}
		freeaddrinfo(res);
		ESP_LOGI(PING_TAG, "target_addr.type=%d", target_addr.type);
		ESP_LOGI(PING_TAG, "target_addr.u_addr.ip4=%s", ip4addr_ntoa(&(target_addr.u_addr.ip4)));
		ping_config.target_addr = target_addr; // target IP address
	} else {
		// ping target is my gateway
		//tcpip_adapter_ip_info_t ip_info;
		//ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &ip_info));
		//ESP_LOGI(PING_TAG, "IP Address: %s", ip4addr_ntoa(&ip_info.ip));
		//ESP_LOGI(PING_TAG, "Subnet mask: %s", ip4addr_ntoa(&ip_info.netmask));
		//ESP_LOGI(PING_TAG, "Gateway:	%s", ip4addr_ntoa(&ip_info.gw));

		// get gateway address in esp_netif_ip_info_t
		esp_netif_ip_info_t ip_info;
		ESP_ERROR_CHECK(esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info));
		ESP_LOGD(PING_TAG, "ip_info.ip=" IPSTR, IP2STR(&ip_info.ip));
		ESP_LOGD(PING_TAG, "ip_info.netmask=" IPSTR, IP2STR(&ip_info.netmask));
		ESP_LOGI(PING_TAG, "ip_info.gw=" IPSTR, IP2STR(&ip_info.gw));

		// convert from esp_ip4_addr_t to string like "192.168.10.1"
		char buf[32];
		sprintf(buf, IPSTR, IP2STR(&ip_info.gw));

		// convert from string to ip_addr_t
		ip_addr_t gateway_addr;
		ip4addr_aton(buf, &gateway_addr.u_addr.ip4);

		// set lwip_ip_addr_type
		gateway_addr.type = IPADDR_TYPE_V4;
		//gateway_addr.u_addr.ip4 = ip_info.gw;
		//gateway_addr = ip_info.gw;
		ESP_LOGI(PING_TAG, "gateway_addr.u_addr.ip4=%s", ip4addr_ntoa(&(gateway_addr.u_addr.ip4)));
		ping_config.target_addr = gateway_addr; // gateway IP address
	}

	//ping_config.count = ESP_PING_COUNT_INFINITE; // ping in infinite mode, esp_ping_stop can stop it
	ping_config.count = PING_COUNT;
	ping_config.interval_ms = interval_ms;
	ping_config.task_prio = PING_TASK_PRIORITY;

	/* set callback functions */
	esp_ping_callbacks_t cbs = {
		.on_ping_success = cmd_ping_on_ping_success,
		.on_ping_timeout = cmd_ping_on_ping_timeout,
		.on_ping_end = cmd_ping_on_ping_end,
		.cb_args = NULL
	};
	esp_ping_handle_t ping;

	esp_ping_new_session(&ping_config, &cbs, &ping);
	esp_ping_start(ping);

	ESP_LOGI(PING_TAG, "ping start");
	return ESP_OK;
}
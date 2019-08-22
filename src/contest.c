#include <stdio.h>
#include <string.h>
#include <zephyr.h>

#include <lte_lc.h>
#include <net/mqtt.h>
#include <net/socket.h>

// Simplistic test app
// Initialize and connect modem + disable eDRX and PSM
// Send a udp message on the form "MsgId <counter>" in a set interval
// Use netcat (nc -uvkl <port number>) as server and set server, host and interval in the defines below
//
// See https://devzone.nordicsemi.com/f/nordic-q-a/51223/lte-m-ip-connectivity-available-only-3-seconds-every-11-seconds
//  Jesper Tiberg aug 2019

char msgbuf[100];
static int sock;
static struct sockaddr_storage server;

#if defined(CONFIG_BSD_LIBRARY)

/**@brief Recoverable BSD library error. */
void bsd_recoverable_error_handler(uint32_t err)
{
  printk("bsdlib recoverable error: %u\n", err);
}

/**@brief Irrecoverable BSD library error. */
void bsd_irrecoverable_error_handler(uint32_t err)
{
  printk("bsdlib irrecoverable error: %u\n", err);

  __ASSERT_NO_MSG(false);
}

#endif /* defined(CONFIG_BSD_LIBRARY) */

static void modem_configure(void)
{
  int err;

  printk("LTE Link Init ...\n");
  err = lte_lc_init_and_connect();
  __ASSERT(err == 0, "ERROR: LTE link could not be established %d\n", err);

  printk("LTE Link Connected!\n");

  printk("Set PSM mode\n");
  err = lte_lc_psm_req(false);
  __ASSERT(err == 0, "ERROR: set edrx %d\n", err);

  printk("Set EDRX mode\n");
  err = lte_lc_edrx_req(false);
  __ASSERT(err == 0, "ERROR: set edrx %d\n", err);
}

/**@brief Resolves the configured hostname. */
static int server_resolve(void)
{
  int err;
  struct addrinfo *result;
  struct addrinfo hints = {
      .ai_family = AF_INET,
      .ai_socktype = SOCK_DGRAM};
  char ipv4_addr[NET_IPV4_ADDR_LEN];

  err = getaddrinfo(CONFIG_UDP_SERVER_HOST, NULL, &hints, &result);
  if (err != 0)
  {
    printk("ERROR: getaddrinfo failed %d\n", err);
    return -EIO;
  }

  if (result == NULL)
  {
    printk("ERROR: Address not found\n");
    return -ENOENT;
  }

  /* IPv4 Address. */
  struct sockaddr_in *server4 = ((struct sockaddr_in *)&server);

  server4->sin_addr.s_addr =
      ((struct sockaddr_in *)result->ai_addr)->sin_addr.s_addr;
  server4->sin_family = AF_INET;
  server4->sin_port = htons(CONFIG_UDP_SERVER_PORT);

  inet_ntop(AF_INET, &server4->sin_addr.s_addr, ipv4_addr,
            sizeof(ipv4_addr));
  printk("IPv4 Address found %s\n", ipv4_addr);

  /* Free the address. */
  freeaddrinfo(result);

  return 0;
}

int init_udp_socket()
{
  int err = server_resolve();
  if (err < 0)
  {
    printk("Resolve failed : %d\n", errno);
    return -errno;
  }

  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (sock < 0)
  {
    printk("Failed to create CoAP socket: %d.\n", errno);
    return -errno;
  }

  err = connect(sock, (struct sockaddr *)&server,
                sizeof(struct sockaddr_in));
  if (err < 0)
  {
    printk("Connect failed : %d\n", errno);
    return -errno;
  }

  return 0;
}

int send_udp_msg(int msg_id)
{
  sprintf(msgbuf, "MsgId %d\n", msg_id);
  int ret = send(sock, msgbuf, strlen(msgbuf), 0);
  printk("Sent 'MsgId %d' to %s:%d. ret=%d\n", msg_id, CONFIG_UDP_SERVER_HOST, CONFIG_UDP_SERVER_PORT, ret);
  return 0;
}

int main(void)
{
  printk("Start UDP connection test\n");
  printk("Initializing Modem\n");
  modem_configure();
  printk("Setup UDP socket\n");
  int err = init_udp_socket();
  __ASSERT(err == 0, "ERROR: setting up udp socket");

  printk("Start sending messages to %s:%d, interval:%d\n", CONFIG_UDP_SERVER_HOST, CONFIG_UDP_SERVER_PORT, CONFIG_SEND_INTERVAL_MILLIS);


  for (int i = 0;; i++)
  {
    send_udp_msg(i);
    k_sleep(CONFIG_SEND_INTERVAL_MILLIS);
  }
}

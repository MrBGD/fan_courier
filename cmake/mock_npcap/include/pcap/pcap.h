/*
 * Mock npcap SDK headers for CI builds.
 * Stub declarations aligned with mock_pcap.c implementations.
 * For real builds, the npcap SDK is fetched from npcap.com.
 */
#ifndef PCAP_H
#define PCAP_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pcap pcap_t;
typedef struct pcap_dumper pcap_dumper_t;
typedef struct pcap_if pcap_if_t;
typedef struct pcap_addr pcap_addr_t;
typedef int bpf_u_int32;
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;

#define PCAP_ERRBUF_SIZE 256
#define DLT_EN10MB 1

struct timeval {
    long tv_sec;
    long tv_usec;
};

struct pcap_pkthdr {
    struct timeval ts;
    bpf_u_int32 caplen;
    bpf_u_int32 len;
};

struct pcap_stat {
    u_int ps_recv;
    u_int ps_drop;
    u_int ps_ifdrop;
};

struct pcap_rmtauth {
    int type;
    char *username;
    char *password;
};

struct pcap_send_queue {
    u_int len;
    u_int maxlen;
    u_char *buffer;
};
typedef struct pcap_send_queue pcap_send_queue;

struct bpf_stat {
    u_int bs_recv;
    u_int bs_drop;
};

typedef void (*pcap_handler)(u_char *user, const struct pcap_pkthdr *h, const u_char *sp);

enum pcap_direction {
    PCAP_D_INOUT = 0,
    PCAP_D_IN    = 1,
    PCAP_D_OUT   = 2
};
typedef enum pcap_direction pcap_direction_t;

struct bpf_program {
    int   bf_len;
    void *bf_insns;
};

struct bpf_insn {
    unsigned short code;
    unsigned char  jt;
    unsigned char  jf;
    int            k;
};

/* Core open/close */
char    *pcap_lookupdev(char *errbuf);
int      pcap_lookupnet(const char *device, bpf_u_int32 *netp, bpf_u_int32 *maskp, char *errbuf);
pcap_t  *pcap_open(const char *source, int snaplen, int flags, int read_timeout, struct pcap_rmtauth *auth, char *errbuf);
pcap_t  *pcap_open_dead(int linktype, int snaplen);
pcap_t  *pcap_open_dead_with_tstamp_precision(int linktype, int snaplen, u_int precision);
pcap_t  *pcap_open_offline_with_tstamp_precision(const char *fname, u_int precision, char *errbuf);
pcap_t  *pcap_create(const char *device, char *errbuf);
int      pcap_activate(pcap_t *p);
void     pcap_close(pcap_t *p);

/* Capture loop */
int           pcap_loop(pcap_t *p, int cnt, pcap_handler callback, u_char *user);
int           pcap_dispatch(pcap_t *p, int cnt, pcap_handler callback, u_char *user);
const u_char *pcap_next(pcap_t *p, struct pcap_pkthdr *h);
void          pcap_breakloop(pcap_t *p);

/* Send */
int pcap_sendpacket(pcap_t *p, const u_char *buf, int size);

/* Stats */
int              pcap_stats(pcap_t *p, struct pcap_stat *ps);
struct pcap_stat *pcap_stats_ex(pcap_t *p, int *pcap_stat_size);

/* Filter */
int  pcap_setfilter(pcap_t *p, struct bpf_program *fp);
int  pcap_compile(pcap_t *p, struct bpf_program *fp, const char *str, int optimize, bpf_u_int32 netmask);
int  pcap_compile_nopcap(int snaplen_arg, int linktype_arg, struct bpf_program *fp, const char *str, int optimize, bpf_u_int32 mask);
void pcap_freecode(struct bpf_program *fp);
int  pcap_offline_filter(const struct bpf_program *fp, const struct pcap_pkthdr *h, const u_char *pkt);

/* Direction */
int pcap_setdirection(pcap_t *p, pcap_direction_t d);

/* Error / version */
char       *pcap_geterr(pcap_t *p);
void        pcap_perror(pcap_t *p, const char *prefix);
const char *pcap_strerror(int error);
const char *pcap_lib_version(void);

/* Data-link */
int         pcap_datalink(pcap_t *p);
int         pcap_datalink_ext(pcap_t *p);
int         pcap_list_datalinks(pcap_t *p, int **dlt_buf);
int         pcap_set_datalink(pcap_t *p, int dlt);
void        pcap_free_datalinks(int *dlt_list);
const char *pcap_datalink_val_to_name(int dlt);
const char *pcap_datalink_val_to_description(int dlt);

/* Remote source helpers */
int pcap_createsrcstr(char *source, int type, const char *host, const char *port, const char *name, char *errbuf);
int pcap_parsesrcstr(const char *source, int *type, char *host, char *port, char *name, char *errbuf);

/* Device enumeration */
int  pcap_findalldevs_ex(const char *source, struct pcap_rmtauth *auth, pcap_if_t **alldevs, char *errbuf);
int  pcap_findalldevs(pcap_if_t **alldevs, char *errbuf);
void pcap_freealldevs(pcap_if_t *alldevs);

/* Dump */
pcap_dumper_t *pcap_dump_open(pcap_t *p, const char *fname);
pcap_dumper_t *pcap_dump_open_append(pcap_t *p, const char *fname);
void           pcap_dump(u_char *user, const struct pcap_pkthdr *h, const u_char *sp);
int            pcap_dump_flush(pcap_dumper_t *p);
void           pcap_dump_close(pcap_dumper_t *p);

/* Configuration */
int pcap_set_snaplen(pcap_t *p, int snaplen);
int pcap_set_promisc(pcap_t *p, int promisc);
int pcap_set_timeout(pcap_t *p, int to_ms);
int pcap_set_buffer_size(pcap_t *p, int buffer_size);
int pcap_set_immediate_mode(pcap_t *p, int mode);
int pcap_setmintocopy(pcap_t *p, int size);
int pcap_setmode(pcap_t *p, int mode);

/* Timestamps */
int  pcap_list_tstamp_types(pcap_t *p, int **tstamp_typesp);
void pcap_free_tstamp_types(int *tstamp_types);
int  pcap_set_tstamp_type(pcap_t *p, int tstamp_type);
int  pcap_get_tstamp_precision(pcap_t *p);
int  pcap_set_tstamp_precision(pcap_t *p, int tstamp_precision);

/* Non-blocking */
int pcap_get_selectable_fd(pcap_t *p);
int pcap_get_nonblock(pcap_t *p, char *errbuf);
int pcap_set_nonblock(pcap_t *p, int nonblock, char *errbuf);

/* Send queue */
pcap_send_queue *pcap_sendqueue_alloc(u_int memsize);
void             pcap_sendqueue_destroy(pcap_send_queue *queue);
int              pcap_sendqueue_queue(pcap_send_queue *queue, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data);
u_int            pcap_sendqueue_transmit(pcap_t *p, pcap_send_queue *queue, int sync);

#define pcap_inject(p, buf, size) pcap_sendpacket(p, buf, size)

#ifdef __cplusplus
}
#endif

#endif /* PCAP_H */

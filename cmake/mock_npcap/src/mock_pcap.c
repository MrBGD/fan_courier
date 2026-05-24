#include "pcap/pcap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *pcap_lookupdev(char *errbuf) { return NULL; }
int pcap_lookupnet(const char *device, bpf_u_int32 *netp, bpf_u_int32 *maskp, char *errbuf) { return 0; }
pcap_t *pcap_open(const char *source, int snaplen, int flags, int read_timeout, struct pcap_rmtauth *auth, char *errbuf) { return NULL; }
pcap_t *pcap_open_dead(int linktype, int snaplen) { return NULL; }
pcap_t *pcap_open_dead_with_tstamp_precision(int linktype, int snaplen, u_int precision) { return NULL; }
pcap_t *pcap_open_offline_with_tstamp_precision(const char *fname, u_int precision, char *errbuf) { return NULL; }
pcap_t *pcap_create(const char *device, char *errbuf) { return NULL; }
int pcap_activate(pcap_t *p) { return 0; }
void pcap_close(pcap_t *p) {}
int pcap_loop(pcap_t *p, int cnt, pcap_handler callback, u_char *user) { return 0; }
int pcap_dispatch(pcap_t *p, int cnt, pcap_handler callback, u_char *user) { return 0; }
const u_char *pcap_next(pcap_t *p, struct pcap_pkthdr *h) { return NULL; }
int pcap_sendpacket(pcap_t *p, const u_char *buf, int size) { return 0; }
int pcap_stats(pcap_t *p, struct pcap_stat *ps) { return -1; }
struct pcap_stat *pcap_stats_ex(pcap_t *p, int *pcap_stat_size) { return NULL; }
void pcap_breakloop(pcap_t *p) {}
int pcap_setfilter(pcap_t *p, struct bpf_program *fp) { return -1; }
int pcap_setdirection(pcap_t *p, pcap_direction_t d) { return 0; }
char *pcap_geterr(pcap_t *p) { return (char *)"mock error"; }
void pcap_perror(pcap_t *p, const char *prefix) {}
const char *pcap_strerror(int error) { return "mock error"; }
const char *pcap_lib_version(void) { return "libpcap mock version (stub)"; }
int pcap_compile(pcap_t *p, struct bpf_program *fp, const char *str, int optimize, bpf_u_int32 netmask) { return -1; }
int pcap_compile_nopcap(int snaplen_arg, int linktype_arg, struct bpf_program *fp, const char *str, int optimize, bpf_u_int32 mask) { return -1; }
void pcap_freecode(struct bpf_program *fp) {}
int pcap_offline_filter(const struct bpf_program *fp, const struct pcap_pkthdr *h, const u_char *pkt) { return 0; }
int pcap_datalink(pcap_t *p) { return DLT_EN10MB; }
int pcap_datalink_ext(pcap_t *p) { return DLT_EN10MB; }
int pcap_list_datalinks(pcap_t *p, int **dlt_buf) { return -1; }
int pcap_set_datalink(pcap_t *p, int dlt) { return -1; }
void pcap_free_datalinks(int *dlt_list) {}
const char *pcap_datalink_val_to_name(int dlt) { return "mock"; }
const char *pcap_datalink_val_to_description(int dlt) { return "mock"; }
int pcap_createsrcstr(char *source, int type, const char *host, const char *port, const char *name, char *errbuf) { return -1; }
int pcap_parsesrcstr(const char *source, int *type, char *host, char *port, char *name, char *errbuf) { return -1; }
int pcap_findalldevs_ex(const char *source, struct pcap_rmtauth *auth, pcap_if_t **alldevs, char *errbuf) { return -1; }
int pcap_findalldevs(pcap_if_t **alldevs, char *errbuf) { return -1; }
void pcap_freealldevs(pcap_if_t *alldevs) {}
pcap_dumper_t *pcap_dump_open(pcap_t *p, const char *fname) { return NULL; }
pcap_dumper_t *pcap_dump_open_append(pcap_t *p, const char *fname) { return NULL; }
void pcap_dump(u_char *user, const struct pcap_pkthdr *h, const u_char *sp) {}
int pcap_dump_flush(pcap_dumper_t *p) { return 0; }
void pcap_dump_close(pcap_dumper_t *p) {}
int pcap_set_snaplen(pcap_t *p, int snaplen) { return 0; }
int pcap_set_promisc(pcap_t *p, int promisc) { return 0; }
int pcap_set_timeout(pcap_t *p, int to_ms) { return 0; }
int pcap_set_buffer_size(pcap_t *p, int buffer_size) { return 0; }
int pcap_set_immediate_mode(pcap_t *p, int mode) { return 0; }
int pcap_list_tstamp_types(pcap_t *p, int **tstamp_typesp) { return 0; }
void pcap_free_tstamp_types(int *tstamp_types) {}
int pcap_set_tstamp_type(pcap_t *p, int tstamp_type) { return 0; }
int pcap_get_tstamp_precision(pcap_t *p) { return 0; }
int pcap_set_tstamp_precision(pcap_t *p, int tstamp_precision) { return 0; }
pcap_send_queue* pcap_sendqueue_alloc(u_int memsize) { return NULL; }
void pcap_sendqueue_destroy(pcap_send_queue *queue) {}
int pcap_sendqueue_queue(pcap_send_queue *queue, const struct pcap_pkthdr *pkt_header, const u_char *pkt_data) { return 0; }
u_int pcap_sendqueue_transmit(pcap_t *p, pcap_send_queue *queue, int sync) { return 0; }
int pcap_setmode(pcap_t *p, int mode) { return 0; }
int pcap_get_selectable_fd(pcap_t *p) { return -1; }
int pcap_get_nonblock(pcap_t *p, char *errbuf) { return 0; }
int pcap_set_nonblock(pcap_t *p, int nonblock, char *errbuf) { return 0; }
int pcap_setmintocopy(pcap_t *p, int size) { return 0; }

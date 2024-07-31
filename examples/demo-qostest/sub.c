#include "dds/dds.h"
#include "TestData.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define BYTES_PER_SEC_TO_MEGABITS_PER_SEC 125000.0
#define MAX_SAMPLES 1000
#define CRC8_POLY 0x07

typedef struct HandleEntry
{
  dds_instance_handle_t handle;
  unsigned long long count;
  struct HandleEntry * next;
} HandleEntry;

typedef struct HandleMap
{
  HandleEntry *entries;
} HandleMap;

static HandleMap * imap;
static unsigned long long lostPackNum = 0;
static unsigned long long wrongPackNum = 0;
static unsigned long long total_bytes = 0;
static unsigned long long total_samples = 0;
static uint8_t crc = 0x00;
static uint8_t crc8table[256];

static dds_time_t startTime = 0;

static uint32_t payloadSize = 0;
static int8_t pollingDelay = -1;

static TestQoSModule_DataType data [MAX_SAMPLES];
static dds_sample_info_t info[MAX_SAMPLES];
static void * samples[MAX_SAMPLES];

static dds_entity_t waitSet;

static HandleMap * HandleMap__alloc (void);
static void HandleMap__free (HandleMap *map);
static HandleEntry * store_handle (HandleMap *map, dds_instance_handle_t key);
static HandleEntry * retrieve_handle (HandleMap *map, dds_instance_handle_t key);

static int parse_args(int argc, char **argv, uint32_t *maxCycles, int8_t *pollingDelay, char **partitionName);
static uint32_t do_take (dds_entity_t reader);
static void data_take_handler (dds_entity_t reader, void *arg);
static dds_entity_t prepare_dds(dds_entity_t *reader, const char *partitionName);
static void process_samples(dds_entity_t reader, uint32_t maxCycles);
static void finalize_dds(dds_entity_t participant);
static void generate_crc8_table(uint8_t * crc8table);
static uint8_t crc8_checktable(TestQoSModule_DataType *sample);

#if !DDSRT_WITH_FREERTOS && !__ZEPHYR__
/* crtl+c interrupt*/
static volatile sig_atomic_t done = false;
static void sigint (int sig)
{
  (void) sig;
  done = true;
}
#else
static bool done = false;
#endif


int main (int argc, char **argv)
{
  uint32_t maxCycles = 0;
  char *partitionName = "QoStest_example";

  dds_entity_t participant;
  dds_entity_t reader;
  //// 按照格式输出 CRC8 表格的值
  // 	for (int i = 0; i < 16; ++i) {
	// 		for (int j = 0; j < 16; ++j) {
	// 				printf("%d ",crc8table[i+16*j]);
	// 		}
	// }

  generate_crc8_table(crc8table);
	if (parse_args(argc, argv, &maxCycles, &pollingDelay, &partitionName) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }

	participant = prepare_dds(&reader, partitionName);

  printf ("=== [Subscriber] Waiting for samples...\n");
  fflush (stdout);

	#if !DDSRT_WITH_FREERTOS && !__ZEPHYR__
  signal (SIGINT, sigint);
	#endif

  process_samples(reader, maxCycles);
	(void) dds_set_status_mask (reader, 0);
  HandleMap__free (imap);
	finalize_dds(participant);
	return EXIT_SUCCESS;
}

static int parse_args(int argc, char **argv, uint32_t *maxCycles, int8_t *pollingDelay, char **partitionName)
{
  if (argc == 2 && (strcmp (argv[1], "-h") == 0 || strcmp (argv[1], "--help") == 0))
  {
    printf ("Usage (parameters must be supplied in order):\n");
    printf ("./subscriber [maxCycles (0 = infinite)] [pollingDelay (ms, 0 = waitset, -1 = listener)] [partitionName]\n");
    printf ("Defaults:\n");
    printf ("./subscriber 0 -1 \"QoStest example\"\n");
    return EXIT_FAILURE;
  }

  if (argc > 1)
  {
    *maxCycles = (uint32_t) atoi (argv[1]); /* The number of times to output statistics before terminating */
  }
  if (argc > 2)
  {
    *pollingDelay = (int8_t) atoi (argv[2]); /* The number of ms to wait between reads (0 = waitset, -1 = listener) */
  }
  if (argc > 3)
  {
    *partitionName = argv[3]; /* The name of the partition */
  }
	printf ("RUNNING ./subscriber [maxCycles (%d)] [pollingDelay \"%s\" %d ms] [partitonName \"%s\"]\n",
					*maxCycles, (*pollingDelay == -1) ? "listener" : "waitset", *pollingDelay, *partitionName);
  fflush (stdout);
  return EXIT_SUCCESS;	
}

static HandleMap * HandleMap__alloc (void)
{
  HandleMap * map = malloc (sizeof (*map));
  assert(map);
  memset (map, 0, sizeof (*map));
  return map;
}

static void HandleMap__free (HandleMap *map)
{
  HandleEntry * entry;

  while (map->entries)
  {
    entry = map->entries;
    map->entries = entry->next;
    free (entry);
  }
  free (map);
}

static HandleEntry * store_handle (HandleMap *map, dds_instance_handle_t key)
{
  HandleEntry * entry = malloc (sizeof (*entry));
  assert(entry);
  memset (entry, 0, sizeof (*entry));

  entry->handle = key;
  entry->next = map->entries;
  map->entries = entry;

  return entry;
}

static HandleEntry * retrieve_handle (HandleMap *map, dds_instance_handle_t key)
{
  HandleEntry * entry = map->entries;

  while (entry)
  {
    if (entry->handle == key)
    {
      break;
    }
    entry = entry->next;
  }
  return entry;
}

static void data_take_handler (dds_entity_t reader, void *arg)
{
  (void) do_take (reader);
}

static uint32_t do_take(dds_entity_t reader){
	uint32_t samples_received;
	dds_instance_handle_t ph = 0;
  HandleEntry * current = NULL;
  TestQoSModule_DataType * this_sample;
	if (startTime == 0){
    	startTime = dds_time ();
  }
  samples_received = dds_take (reader, samples, info, MAX_SAMPLES, MAX_SAMPLES);

  if (samples_received < 0){
    DDS_FATAL("dds_take: %s\n", dds_strretcode(-samples_received));
  }
  for (int i = 0; !done && i < samples_received; i++){
		if (info[i].valid_data){
			ph = info[i].publication_handle;
      current = retrieve_handle (imap, ph);
      this_sample = &data[i];
      crc = crc8_checktable(this_sample);
      // printf("crc=%d\t",crc);
      if (current == NULL)
      {
        current = store_handle (imap, ph);
        current->count = this_sample->count;
				payloadSize = this_sample->payload._length;
        // printf("crc=%d/t",crc);
      }

      else if (this_sample->crcCode != crc)
      {
        printf("%d-%d- \t",this_sample->crcCode,crc);
        wrongPackNum ++;
      }

      if (this_sample->count != current->count)
      {
        lostPackNum++;
      }

      current->count = this_sample->count + 1;
      total_bytes += payloadSize + 16 + 1;
      total_samples++;
		}
	}
}

static dds_entity_t prepare_dds(dds_entity_t *reader, const char *partitionName){
	dds_return_t rc;
	dds_entity_t topic;
	dds_entity_t participant;
	dds_entity_t subscriber;
	dds_listener_t *subListenner;

	const dds_qos_t *subQoS = dds_create_qos();
  const dds_qos_t *rdQoS = dds_create_qos();
	const dds_qos_t *tQoS = dds_create_qos();
	const char *subPart[1] = {partitionName};
	dds_qos_provider_t *provider;

	dds_return_t ret = dds_create_qos_provider ("../../../qos_config.xml", &provider);
	assert (ret == DDS_RETCODE_OK);

	/*participant create*/
	participant = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
  if (participant < 0)
    DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));
  // dds_qset_reliability (tQoS, DDS_RELIABILITY_RELIABLE, DDS_SECS (10));
  // dds_qset_reliability (tQoS, DDS_RELIABILITY_BEST_EFFORT, DDS_SECS (10));
  // dds_qset_history (tQoS, DDS_HISTORY_KEEP_ALL, 0);
  // dds_qset_history (tQoS, DDS_HISTORY_KEEP_LAST, MAX_SAMPLES*60);
  // dds_qset_resource_limits (tQoS, MAX_SAMPLES, DDS_LENGTH_UNLIMITED, DDS_LENGTH_UNLIMITED);

	ret = dds_qos_provider_get_qos (provider, DDS_TOPIC_QOS, "myqoslib::my_profile::my_topic", &tQoS);
	assert (ret == DDS_RETCODE_OK);
  topic = dds_create_topic (participant, &TestQoSModule_DataType_desc, "QoStest", tQoS, NULL);
  if (topic < 0)
    DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));
	
	/*Subscriber create*/
  // dds_qset_partition (subQoS, 1, subPart);
  ret = dds_qos_provider_get_qos (provider, DDS_SUBSCRIBER_QOS, "myqoslib::my_profile", &subQoS);
  assert (ret == DDS_RETCODE_OK);

  subscriber = dds_create_subscriber (participant, subQoS, NULL);
  if (subscriber < 0)
    DDS_FATAL("dds_create_subscriber: %s\n", dds_strretcode(-subscriber));
  // dds_delete_qos (subQoS);
	// dds_delete_qos (tQoS);
	/*Data set*/
  imap = HandleMap__alloc ();
  memset (data, 0, sizeof (data));
  for (unsigned int i = 0; i < MAX_SAMPLES; i++)
  {
    samples[i] = &data[i];
  }
	/*waitset create*/
	waitSet = dds_create_waitset (participant);
  if (waitSet < 0)
    DDS_FATAL("dds_create_waitset: %s\n", dds_strretcode(-waitSet));
	/*reader create*/
  ret = dds_qos_provider_get_qos (provider, DDS_READER_QOS, "myqoslib::my_profile", &rdQoS);
	assert (ret == DDS_RETCODE_OK);

	*reader = dds_create_reader (subscriber, topic, rdQoS, NULL);
  if (*reader < 0)
    DDS_FATAL("dds_create_reader: %s\n", dds_strretcode(-*reader));

	if (pollingDelay < 0){
		subListenner = dds_create_listener(NULL);
 	  dds_lset_data_available(subListenner, data_take_handler);
		rc = dds_set_listener(*reader, subListenner);
		if (rc < 0)
			DDS_FATAL("dds_set_listener: %s\n", dds_strretcode(-rc));
		dds_delete_listener(subListenner);
	}
	else if (pollingDelay == 0){
    rc = dds_waitset_attach (waitSet, *reader, *reader);
    if (rc < 0)
      DDS_FATAL("dds_waitset_attach: %s\n", dds_strretcode(-rc));
  }
	else{
		rc = dds_waitset_attach (waitSet, waitSet, waitSet);
		if (rc < 0)
			DDS_FATAL("dds_waitset_attach: %s\n", dds_strretcode(-rc));
	}
  dds_delete_qos_provider (provider);
  return participant;
}

static void process_samples(dds_entity_t reader, uint32_t maxCycles){
	dds_return_t rc;
  unsigned long long prev_bytes = 0;
  unsigned long long prev_samples = 0;
	uint32_t cycles = 0;
  dds_time_t deltaT;
	dds_time_t prev_time = 0;
  dds_time_t time_now = 0;
  double deltaTime = 0;
  bool first_batch = true;

	while(!done && (maxCycles == 0 || cycles < maxCycles)){
		if (pollingDelay > 0)
      dds_sleepfor (DDS_MSECS (pollingDelay));

		if (pollingDelay >= 0)
      do_take(reader);

    time_now = dds_time();
		if (!first_batch)
    {
      deltaT = time_now - prev_time;
      deltaTime = (double) deltaT / DDS_NSECS_IN_SEC;

      if (deltaTime >= 1.0 && total_samples != prev_samples)
      {
        printf ("=== [Subscriber] %5.3fSec |Payload size: %u | Total received: %llu samples, %llu bytes| Out of order: %llu samples "
                "Transfer rate: %.2lf samples/s, %.2lf Mbit/s\n",
                deltaTime, payloadSize, total_samples, total_bytes, lostPackNum,(double)(total_samples - prev_samples) / deltaTime,
								((total_bytes - prev_bytes) / BYTES_PER_SEC_TO_MEGABITS_PER_SEC) / deltaTime);
        fflush (stdout);
        cycles++;
        prev_time = time_now;
        prev_bytes = total_bytes;
        prev_samples = total_samples;
      }
    }
    else
    {
      prev_time = time_now;
      first_batch = false;
    }
	}
	deltaT = time_now - startTime;
	deltaTime = (double) (deltaT / DDS_NSECS_IN_SEC);
	printf("----------------------------------------------------\n");
	printf ("Total Time: %.2f Seconds\n", deltaTime);
	printf ("Total received: %llu samples, %llu bytes\n", total_samples, total_bytes);
	printf ("Out of order: %llu samples\n", lostPackNum);
	printf ("Out of order rate: %.2lf%% \n", 100.0* (double)lostPackNum / (double)total_samples);
  printf ("Wrong Sample: %llu samples\n", wrongPackNum);
	printf ("Average transfer rate: %.2lf samples/s, ", (double)total_samples / deltaTime);
	printf ("%.2lf Mbit/s\n", (double)(total_bytes / BYTES_PER_SEC_TO_MEGABITS_PER_SEC) / deltaTime);
	fflush (stdout);
}

static void finalize_dds(dds_entity_t participant){
  dds_return_t rc;
  for (unsigned int i = 0; i < MAX_SAMPLES; i++)
  {
    TestQoSModule_DataType_free (&data[i], DDS_FREE_CONTENTS);
  }
	if (pollingDelay > 0){
  	rc = dds_waitset_detach (waitSet, waitSet);
		if (rc < 0)
			DDS_FATAL("dds_waitset_detach: %s\n", dds_strretcode(-rc));
	}
  rc = dds_delete (waitSet);
  if (rc < 0)
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-rc));
  rc = dds_delete (participant);
  if (rc < 0)
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-rc));
}

static void generate_crc8_table(uint8_t * crc8table) {
    for (int i = 0; i < 256; i++) {
        crc8table[i] = i;
        for (int j = 8; j != 0; j--) {
            if ((crc8table[i] & 0x80) != 0) {
                crc8table[i] = (crc8table[i] << 1) ^ CRC8_POLY;
            } else {
                crc8table[i] <<= 1;
            }
        }
    }
}
static uint8_t crc8_checktable(TestQoSModule_DataType *sample) {
    uint8_t crc = 0x00; // CRC初始化
    crc = crc ^ 0xFF;
    for (int pos = 0; pos < sample->payload._length; pos++) {
      // printf("%d ",sample->payload._buffer[pos]);
      crc = crc8table[(crc ^ sample->payload._buffer[pos]) & 0xFF];
    }
    return crc;
}
#include "dds/dds.h"
#include "TestData.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>

#define MAX_SAMPLES 1000
#define CRC8_POLY 0x07
static uint8_t crc8table[256];
static uint8_t crc = 0x00;

static bool done = false;
static int64_t time_now; 
/* crtl+c interrupt*/
#if !DDSRT_WITH_FREERTOS && !__ZEPHYR__
static void sigint (int sig)
{
  (void)sig;
  done = true;
}
#endif

static int parse_args(int argc, char **argv, uint32_t *payloadSize, uint32_t *burstSize,
    uint32_t *burstInterval, uint32_t *timeOut, char **partitionName);
static int prepare_dds(dds_entity_t *writer, dds_entity_t *participant, const char *partitionName);
static dds_return_t wait_for_reader(dds_entity_t writer, dds_entity_t participant);
static dds_return_t finalize_dds(dds_entity_t writer, dds_entity_t participant, TestQoSModule_DataType sample);
static void start_writing(dds_entity_t writer, TestQoSModule_DataType *sample, uint32_t burstSize, uint32_t burstInterval, uint32_t timeOut);
// static int64_t get_current_millis();
static void random_datawrite(TestQoSModule_DataType *sample);
static unsigned char crc8(TestQoSModule_DataType *sample);
static void generate_crc8_table(uint8_t * crc8table);
static uint8_t crc8_checktable(TestQoSModule_DataType *sample);

int main (int argc, char **argv)
{
	uint32_t payloadSize = 8192;
	uint32_t burstInterval = 0;
	uint32_t burstSize = 1;
	uint32_t timeOut = 0;
  char * partitionName = "QoStest_example";
	dds_entity_t participant;
	dds_entity_t writer;
	dds_return_t rc;
	TestQoSModule_DataType sample;
	generate_crc8_table(crc8table);
	// dds_qos_provider_t *provider;

	// for (int i = 0; i < 16; ++i) {
	// 		for (int j = 0; j < 16; ++j) {
	// 				// 按照格式输出 CRC8 表格的值
	// 				printf("%d ",crc8table[i+16*j]);
	// 		}
	// }
  if (parse_args(argc, argv, &payloadSize, &burstSize, &burstInterval, &timeOut, &partitionName) == EXIT_FAILURE){
		return EXIT_FAILURE;
	}

	prepare_dds(&writer, &participant, partitionName);
/* Write Data IN*/
  sample.count = 0;
	sample.timestamp = dds_time();
  sample.payload._buffer = dds_alloc (payloadSize);
  sample.payload._length = payloadSize;
  sample.payload._release = true;
  for (uint32_t i = 0; i < payloadSize; i++) {
    sample.payload._buffer[i] = 0xaa;
  }
	sample.crcCode = crc;
	// printf("size of count:%ld\n",sizeof(sample.count));
	// printf("size of timestamp:%ld\n",sizeof(sample.timestamp));
	// printf("size of crcCode:%ld\n",sizeof(sample.crcCode));
	rc = wait_for_reader(writer, participant);
	if (rc == 0){
		printf ("=== [Publisher]  Did not discover a reader,ENDING......\n");
    fflush (stdout);	
		finalize_dds(writer, participant, sample);
		return EXIT_FAILURE;
	}

	#if !DDSRT_WITH_FREERTOS && !__ZEPHYR__
  signal (SIGINT, sigint);
	#endif

	start_writing(writer, &sample, burstSize, burstInterval, timeOut);
	
  finalize_dds(writer, participant, sample);
	return EXIT_SUCCESS;
}

static int parse_args(int argc, char **argv, uint32_t *payloadSize, uint32_t *burstSize,
    uint32_t *burstInterval, uint32_t *timeOut, char **partitionName){
  if (argc == 2 && (strcmp (argv[1], "-h") == 0 || strcmp (argv[1], "--help") == 0))
  {
    printf ("Usage (parameters must be supplied in order):\n");
    printf ("./pub [payloadSize (bytes)] [burstSize (samples)[1-1000]] [burstInterval (ms)] [timeOut (seconds)] [partitionName]\n");
    printf ("Defaults:\n");
    printf ("./pub 8192 1 0 0 \"QosTest example\"\n");
    return EXIT_FAILURE;
  }
  if (argc > 1){
		*payloadSize = (uint32_t) atoi (argv[1]);
	}
	if (argc > 2){
		*burstSize = (uint32_t) atoi (argv[2]);
	}
	if (argc > 3){
		*burstInterval = (uint32_t) atoi (argv[3]);
	}
	if (argc > 4){
		*timeOut = (uint32_t) atoi (argv[4]);
	}
	if (argc > 5){
		*partitionName = (argv[5]);
	}

  printf ("\npayloadSize: %"PRIu32" bytes||| burstSize: %"PRIu32"||| burstInterval: %"PRIu32"ms||| timeOut: %"PRIu32" seconds||| partitionName: \"%s\"\n",
    *payloadSize, *burstSize, *burstInterval, *timeOut, *partitionName);
  fflush (stdout);

  if (*burstSize > 1000){
    printf("\nwarning: burstSize too big, May lead to data loss!!!\n");
    fflush(stdout);
  }
	return EXIT_SUCCESS;
    }

static int prepare_dds(dds_entity_t *writer, dds_entity_t *participant, const char *partitionName){
	dds_entity_t topic;
  dds_entity_t publisher;
  const char *pubParts[1] = {partitionName};
  const dds_qos_t *tQos;
  const dds_qos_t *pbQos;
  const dds_qos_t *wrQos;
	dds_qos_provider_t *provider;

	dds_return_t ret = dds_create_qos_provider ("../../../qos_config.xml", &provider);
	assert (ret == DDS_RETCODE_OK);

	*participant = dds_create_participant (DDS_DOMAIN_DEFAULT, NULL, NULL);
  if (*participant < 0)
    DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-*participant));

	tQos = dds_create_qos(); 
  // dds_qset_reliability (tQos, DDS_RELIABILITY_RELIABLE, DDS_SECS (10));
  // dds_qset_reliability (tQos, DDS_RELIABILITY_BEST_EFFORT, DDS_SECS (10));
  // dds_qset_history (tQos, DDS_HISTORY_KEEP_ALL, 0);
  // dds_qset_history (tQos, DDS_HISTORY_KEEP_LAST, MAX_SAMPLES*60);  
  // dds_qset_resource_limits (tQos, MAX_SAMPLES, DDS_LENGTH_UNLIMITED, DDS_LENGTH_UNLIMITED);

	ret = dds_qos_provider_get_qos (provider, DDS_TOPIC_QOS, "myqoslib::my_profile::my_topic", &tQos);
	assert (ret == DDS_RETCODE_OK);

	topic = dds_create_topic (*participant, &TestQoSModule_DataType_desc, "QoStest", tQos, NULL);
  if (topic < 0)
    DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));
  // dds_delete_qos (tQos);
	// delete tQos;
	pbQos = dds_create_qos();

	// dds_qset_partition(pbQos, 1, pubParts);
	ret = dds_qos_provider_get_qos (provider, DDS_PUBLISHER_QOS, "myqoslib::my_profile", &pbQos);
  assert (ret == DDS_RETCODE_OK);

	publisher = dds_create_publisher (*participant, pbQos, NULL);
  if (publisher < 0)
  	DDS_FATAL("dds_create_publisher: %s\n", dds_strretcode(-publisher));
  // dds_delete_qos (pbQos);

	wrQos = dds_create_qos();
	ret = dds_qos_provider_get_qos (provider, DDS_WRITER_QOS, "myqoslib::my_profile", &wrQos);
	assert (ret == DDS_RETCODE_OK);
  // dds_qset_writer_batching (wrQos, true);
	// dds_qset_writer_batching (wrQos, false);
  *writer = dds_create_writer (publisher, topic, wrQos, NULL);
  if (*writer < 0)
    DDS_FATAL("dds_create_writer: %s\n", dds_strretcode(-*writer));
  // dds_delete_qos (wrQos);
	dds_delete_qos_provider (provider);
	return EXIT_SUCCESS;
}

static dds_return_t wait_for_reader(dds_entity_t writer, dds_entity_t participant){
	printf ("\n=== [Publisher]  Waiting for a reader ...\n");
  fflush (stdout);

  dds_return_t rc;
  dds_entity_t waitset;

	rc = dds_set_status_mask(writer, DDS_PUBLICATION_MATCHED_STATUS);
	if (rc < 0)
  	DDS_FATAL("dds_set_status_mask: %s\n", dds_strretcode(-rc));
  
	waitset = dds_create_waitset(participant);
  if (waitset < 0)
    DDS_FATAL("dds_create_waitset: %s\n", dds_strretcode(-waitset));

  rc = dds_waitset_attach(waitset, writer, (dds_attach_t) NULL);
  if (rc < 0)
    DDS_FATAL("dds_waitset_attach: %s\n", dds_strretcode(-rc));

  rc = dds_waitset_wait(waitset, NULL, 0, DDS_SECS(30));
  if (rc < 0)
    DDS_FATAL("dds_waitset_wait: %s\n", dds_strretcode(-rc));
	
	return rc;
}

static dds_return_t finalize_dds(dds_entity_t writer, dds_entity_t participant, TestQoSModule_DataType sample){
	dds_return_t rc;
	// printf("%ld",sample.count);
	if (sample.count != 0)
	{
		rc = dds_dispose(writer,&sample);
		if (rc < 0)
			DDS_FATAL("dds_dispose: %s\n", dds_strretcode(-rc));

		dds_free (sample.payload._buffer);
	}
  rc = dds_delete (participant);
  if (rc < 0)
    DDS_FATAL("dds_delete: %s\n", dds_strretcode(-rc));
	return rc;
}

static void start_writing(dds_entity_t writer, TestQoSModule_DataType *sample, uint32_t burstSize, uint32_t burstInterval, uint32_t timeOut){
	bool outTime = false;
  dds_time_t pubStart = dds_time ();
  dds_time_t now;
  dds_time_t deltaT;
  dds_return_t rc;

  dds_time_t burstStart = pubStart;
	
  unsigned int burstCount = 0;
	if (!done)
	{
  printf ("=== [Publisher]  Writing samples...\n");
  fflush (stdout);
	dds_write_flush (writer);

	while (!done && !outTime)
	{
		if (burstCount < burstSize)
		{
			rc = dds_write(writer, sample);
			if (rc == DDS_RETCODE_TIMEOUT){				
				outTime = true;
			}
			else if (rc < 0){
				DDS_FATAL("dds_write: %s\n", dds_strretcode(-rc));
			}
			else
			{
				burstCount++;
				random_datawrite(sample);
				// crc = crc8(sample);
				crc = crc8_checktable(sample);
				// printf("%d ",crc);
				sample->crcCode = crc;
				// printf("crc=%d/t",crc);
				sample->timestamp = dds_time();
				sample->count++;
			}
		}
		else if (burstInterval)
		{
			now = dds_time();
			deltaT = now - burstStart;
			if (deltaT < DDS_MSECS(burstInterval))
			{
				dds_write_flush (writer);
				dds_sleepfor (DDS_MSECS (burstInterval) - deltaT);
			}
			burstStart = dds_time ();
      burstCount = 0;
		}
		else
		{
			burstCount = 0;
		}
		if (timeOut)
    {
			now = dds_time ();
			deltaT = now - pubStart;
			if ((deltaT) > DDS_SECS (timeOut))
			{
				outTime = true;
			}
    }
	}
  printf ("=== [Publisher]  %s, %llu samples written.\n", done ? "Terminated" : "Timed out", (unsigned long long) sample->count);
  fflush (stdout);
	}
}

// static int64_t get_current_millis() {
//     struct timeval time;
//     gettimeofday(&time, NULL);
//     int64_t millis = (int64_t)(time.tv_sec) * 1000 + (int64_t)(time.tv_usec) / 1000;
//     return millis;
// }

static void random_datawrite(TestQoSModule_DataType *sample){
	srand((unsigned)time(NULL));
	for (uint32_t i = 0;i < sample->payload._length; i ++ ){
		uint8_t random_byte = (uint8_t)rand() & 0xFF;
		sample->payload._buffer[i] = random_byte;
	}
}

static unsigned char crc8(TestQoSModule_DataType *sample){
	unsigned char crc = 0;
	for (int pos = 0; pos < sample->payload._length; pos++) {
		crc ^= sample->payload._buffer[pos];
		for (int i = 8; i != 0; i--) { // 处理每一位
				if ((crc & 0x80) != 0) { // 低位循环左移，同时与高位进行异或操作
						crc = (crc << 1) ^ CRC8_POLY;
				} else { // 如果最左边的位是0
						crc <<= 1;
				}
		}
}
	// printf("%d\n",crc);
	return crc;
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
        crc = crc8table[(crc ^ sample->payload._buffer[pos]) & 0xFF];
    }
    return crc;
}
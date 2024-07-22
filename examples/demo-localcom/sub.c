#include "dds/dds.h"
#include "DemoData.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define MAX_SAMPLES 1

int main()
{
    dds_entity_t participant;
    dds_entity_t topic;
    dds_entity_t reader;
    dds_return_t rc;
    dds_qos_t *qos;
    DemoData_Msg *msg;
    void *samples[MAX_SAMPLES];
    dds_sample_info_t infos[MAX_SAMPLES];

    participant = dds_create_participant (1, NULL, NULL);
    if (participant < 0)
        DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

    topic = dds_create_topic(participant, &DemoData_Msg_desc, "Demo1_topic", NULL, NULL);
    if (topic < 0)
        DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));

    qos = dds_create_qos ();
    dds_qset_reliability(qos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));
    
    reader = dds_create_reader(participant, topic, qos, NULL);
    if (reader < 0)
        DDS_FATAL("dds_create_reader: %s\n", dds_strretcode(-reader));
    dds_delete_qos(qos);
    
    printf ("\n=== [Subscriber] Waiting for a sample ...\n");
    fflush (stdout);

    samples[0] = DemoData_Msg__alloc ();
    while (true)
    {
        // rc = dds_read (reader, samples, infos, MAX_SAMPLES, MAX_SAMPLES);
        rc = dds_take (reader, samples, infos, MAX_SAMPLES, MAX_SAMPLES);
        if (rc < 0)
        DDS_FATAL("dds_read: %s\n", dds_strretcode(-rc));

        /* Check if we read some data and it is valid. */
        if ((rc > 0) && (infos[0].valid_data))
        {
        msg = (DemoData_Msg*) samples[0];
        printf ("=== [Subscriber] Received : ");
        printf ("Message (%"PRId32", %s, %"PRIu32")\n", msg->userID, msg->message, msg->ntime);
        fflush (stdout);
        // DemoData_Msg_free(samples[0],DDS_FREE_KEY);
        }
        // 
        dds_sleepfor (DDS_MSECS (20));
    }
    DemoData_Msg_free(samples[0],DDS_FREE_KEY);

}
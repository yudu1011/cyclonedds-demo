#include "dds/dds.h"
#include "DemoData.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

void get_strtime(char *);
int main()
{
    dds_entity_t participant;
    dds_entity_t topic;
    dds_entity_t writer;
    dds_return_t rc;
    dds_qos_t *qos;
    DemoData_Msg msg;
    uint32_t status = 0;

    qos = dds_create_qos();

    participant = dds_create_participant(1, NULL, NULL);
    if (participant < 0)
        DDS_FATAL("dds_create_participant: %s\n", dds_strretcode(-participant));

    topic = dds_create_topic(participant, &DemoData_Msg_desc, "Demo1_topic", qos, NULL);
    if (topic < 0)
        DDS_FATAL("dds_create_topic: %s\n", dds_strretcode(-topic));
    
    dds_qset_reliability(qos, DDS_RELIABILITY_RELIABLE, DDS_SECS(10));
    writer = dds_create_writer(participant, topic, qos, NULL);
    if (writer < 0)
        DDS_FATAL("dds_create_writer: %s\n", dds_strretcode(-writer));
    dds_delete_qos(qos);
    
    printf("=== [Publisher]  Waiting for a reader to be discovered ...\n");
    fflush (stdout);

    rc = dds_set_status_mask(writer, DDS_PUBLICATION_MATCHED_STATUS);
        if (rc != DDS_RETCODE_OK)
    DDS_FATAL("dds_set_status_mask: %s\n", dds_strretcode(-rc));

    while(!(status & DDS_PUBLICATION_MATCHED_STATUS))
    {
        rc = dds_get_status_changes(writer, &status);
        if (rc != DDS_RETCODE_OK)
            DDS_FATAL("dds_get_status_changes: %s\n", dds_strretcode(-rc));
        
        dds_sleepfor (DDS_MSECS (100));
    }
    while(true)
    {
    char time_str[100];
    get_strtime(time_str);
    msg.userID = 0;
    msg.message = "Testing";
    msg.ntime = time_str;

    printf ("=== [Publisher]  Writing : ");
    printf ("(%"PRId32", %s, %s)\n", msg.userID, msg.message, msg.ntime);
    fflush (stdout);

    rc = dds_write (writer, &msg);
    if (rc != DDS_RETCODE_OK)
        DDS_FATAL("dds_write: %s\n", dds_strretcode(-rc));
    else
        
    dds_sleepfor(DDS_MSECS(500));
    }

    rc = dds_delete (participant);
    if (rc != DDS_RETCODE_OK)
        DDS_FATAL("dds_delete: %s\n", dds_strretcode(-rc));

    return EXIT_SUCCESS;
}
void get_strtime(char *time_str)
{
    time_t nowtime = time(NULL);
    struct tm *local_time = localtime(&nowtime);
    strftime(time_str, 8 * sizeof(time_str), "%Y-%m-%d_%H:%M:%S", local_time);
}
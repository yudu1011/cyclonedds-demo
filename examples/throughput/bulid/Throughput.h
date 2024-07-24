/****************************************************************

  Generated by Eclipse Cyclone DDS IDL to C Translator
  File name: /home/byd/Desktop/cyclonedds/examples/throughput/bulid/Throughput.h
  Source: /home/byd/Desktop/cyclonedds/examples/throughput/Throughput.idl
  Cyclone DDS: V0.11.0

*****************************************************************/
#ifndef DDSC__HOME_BYD_DESKTOP_CYCLONEDDS_EXAMPLES_THROUGHPUT_BULID_THROUGHPUT_H_8D6DF3E89778136BDE4EA486D688E29F
#define DDSC__HOME_BYD_DESKTOP_CYCLONEDDS_EXAMPLES_THROUGHPUT_BULID_THROUGHPUT_H_8D6DF3E89778136BDE4EA486D688E29F

#include "dds/ddsc/dds_public_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DDS_SEQUENCE_OCTET_DEFINED
#define DDS_SEQUENCE_OCTET_DEFINED
typedef struct dds_sequence_octet
{
  uint32_t _maximum;
  uint32_t _length;
  uint8_t *_buffer;
  bool _release;
} dds_sequence_octet;

#define dds_sequence_octet__alloc() \
((dds_sequence_octet*) dds_alloc (sizeof (dds_sequence_octet)));

#define dds_sequence_octet_allocbuf(l) \
((uint8_t *) dds_alloc ((l) * sizeof (uint8_t)))
#endif /* DDS_SEQUENCE_OCTET_DEFINED */

typedef struct ThroughputModule_DataType
{
  uint64_t count;
  dds_sequence_octet payload;
} ThroughputModule_DataType;

extern const dds_topic_descriptor_t ThroughputModule_DataType_desc;

#define ThroughputModule_DataType__alloc() \
((ThroughputModule_DataType*) dds_alloc (sizeof (ThroughputModule_DataType)));

#define ThroughputModule_DataType_free(d,o) \
dds_sample_free ((d), &ThroughputModule_DataType_desc, (o))

#ifdef __cplusplus
}
#endif

#endif /* DDSC__HOME_BYD_DESKTOP_CYCLONEDDS_EXAMPLES_THROUGHPUT_BULID_THROUGHPUT_H_8D6DF3E89778136BDE4EA486D688E29F */

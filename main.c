#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>


#define HEADER_LEN_OFFSET 0u
#define ID_OFFSET 2u
#define SENSOR_NAME_LEN_OFFSET   5u
#define SENSOR_NAME_START_OFFSET 6u
#define DATA_FORMAT_OFFSET 8u
#define DATA_FORMAT_START_SHIFT 0u
#define DATA_FORMAT_LEN_SHIFT 10u
#define DATA_FORMAT_PARITY_SHIFT 20u
#define NUM_TEST_BYTES 128u


uint16_t get_id(const void * input_array)
{
  return *((uint16_t *)(input_array + ID_OFFSET));
}


void get_name(const void * input_array, char * output_name)
{
 uint8_t *header = (uint8_t *)input_array;
 uint16_t header_len = *((uint16_t *)header);
 uint16_t name_len = *(header+SENSOR_NAME_LEN_OFFSET);
 uint16_t name_offset = *((uint16_t *)(header+SENSOR_NAME_START_OFFSET));

 for (int i=0; i<name_len; i++){
 output_name[i] = *(header+header_len+name_offset+i);
 }
 output_name[name_len]='\0';
}


uint16_t get_data(const void * input_array, float * output_data)
{
 uint8_t *header = (uint8_t *)input_array;
 uint16_t header_len = *((uint16_t *)header);

 uint32_t data_format = *((uint32_t *)(header+DATA_FORMAT_OFFSET));

 uint16_t data_offset = data_format & 0x3FF;
 uint16_t num_values = (data_format >> 10) & 0x3FF;

for (int i=0; i< num_values; i++){
 output_data[i] = *((float *)(header+header_len+data_offset+4*i));
 }
 return num_values;
}


bool data_ok(const void * input_array)
{
 uint8_t *header = (uint8_t *)input_array;
 uint16_t header_len = *((uint16_t *)header);

 uint32_t data_format = *((uint32_t *)(header+DATA_FORMAT_OFFSET));

 uint16_t data_offset = data_format & 0x3FF;
 uint16_t num_values = (data_format >> DATA_FORMAT_LEN_SHIFT) & 0x3FF;

int data_count=0;

for (int i=0; i<4*num_values; i++){
 uint8_t test = *(header+header_len+data_offset+i);
  for (int j=0; j<8; j++) {
    data_count +=(test & (1 <<j)) ? 1 : 0;
  }
 }
 return (data_count %2 ==0);
}



// Test Code - leave guards around main (#ifndef TESTING_GUARD)

static void read_test_data_from_file(uint8_t test_bytes[])
{
    const char * FILENAME = "/home/coderpad/data/data_file.bin";
    FILE * p_file;
    size_t array_index = 0;

    p_file = fopen(FILENAME, "r");
    while ((!feof(p_file)) && (array_index < NUM_TEST_BYTES))
    {
        test_bytes[array_index++] = fgetc(p_file);
    }
    fclose(p_file);
}

static void test_get_id(uint8_t test_bytes[])
{
    const uint16_t EXPECTED_ID = 1u;
    uint16_t returned_id;

    returned_id = get_id(test_bytes);
    printf("get_id()\nreturned:%u\nexpected:%u\n[%s]\n\n",
        returned_id,
        EXPECTED_ID,
        (returned_id == EXPECTED_ID ? "correct" : "INCORRECT"));
}

static void test_get_name(uint8_t test_bytes[])
{
    const char * EXPECTED_NAME = "SENSOR_1";
    char returned_name[NUM_TEST_BYTES]; 

    get_name(test_bytes, returned_name);
    printf("get_name()\nreturned:%s\nexpected:%s\n[%s]\n\n",
        returned_name,
        EXPECTED_NAME,
        (strcmp(returned_name, EXPECTED_NAME) == 0 ? "correct" : "INCORRECT"));
}

static void test_get_data(uint8_t test_bytes[])
{
    const float EXPECTED_DATA[] =
    {
        1.3f,  1.4f,  1.5f,  1.6f,
        2.4f,  5.1f, -1.2f, -0.3f
    };
    const uint16_t EXPECTED_LEN = 8;

    float returned_data[2 * NUM_TEST_BYTES] = {0};
    uint16_t returned_len;

    returned_len = get_data(test_bytes, returned_data); 

    bool is_returned_data_correct = (returned_len == EXPECTED_LEN);

    puts("get_data");
    for (size_t i = 0; i < sizeof(EXPECTED_DATA) / sizeof(float); i++)
    {
        is_returned_data_correct &= (returned_data[i] == EXPECTED_DATA[i]);
        printf("[%lu] - returned: %f; expected:%f\n",
            i, returned_data[i], EXPECTED_DATA[i]);
    }
    printf("[%s]\n\n", (is_returned_data_correct ?"correct" : "INCORRECT"));
}


static void test_data_ok(uint8_t test_bytes[])
{
    const bool EXPECTED_OK = true;

    bool returned_data_ok;
    returned_data_ok = data_ok(test_bytes);

    printf("data_ok()\nreturned:%s\nexpected:%s\n\n", returned_data_ok?"true":"false", EXPECTED_OK?"true":"false");
    if (EXPECTED_OK == returned_data_ok)
    {
      printf("correct\n");
    }
    else
    {
      printf("INCORRECT\n");
    }
}

// Do not remove main from this guard!
#ifndef TESTING_GUARD
int main(void)
{
    uint8_t bytes[NUM_TEST_BYTES];

    read_test_data_from_file(bytes);
    test_get_id(bytes);
    test_get_name(bytes);
    test_get_data(bytes);
    test_data_ok(bytes);
    return 0;
}
#endif

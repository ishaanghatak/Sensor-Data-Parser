# Sensor-Data-Parser

## Description: 
Implementation of functions to parse data from an arbitrary sensor for a programming problem. Coducts testing using generated data.


## Functions:
- **get_id**: This function has a void pointer parameter to the sensor data. The function returns the sensor’s ID as a uint16_t. 

- **get_name**: This function has a void pointer parameter to the sensor data and has a char pointer parameter to an output array. The function writes the sensor’s name to the output array. The sensor’s name in the output array is null terminated to indicate a string. 

- **get_data**: This function has a void pointer parameter to the sensor data and has a float pointer parameter to an output array. The function writes the floating-point data to the output array and returns the number of floating point values as a uint16_t.

- **data_ok**: This function performs an even parity check on bytes that represent the floating-point data, and returns true if parity check is successful, otherwise it returns as false.


## Sensor Data:
### The sensor data contains bytes (uint8_t) representing two contiguous sections: 
- **Data Header**:  contains information about the sensor and describes how to get  data
- **Data Body**: contains the sensor name and data, and is variable in length

### Its structure is summarised below: 
- **0-1**: Header length (in bytes) 
- **2-3**: Sensor UID 
- **4-7**: Sensor name format 
- **8-11**: Data format 
- **Rest**: Ignore

#### The header begins with a uint16_t, which indicates the total number of bytes in the header.

#### The header continues with a uint16_t, which represents a unique ID for the sensor. 

#### The header continues with four bytes, which describe the format of the sensor’s name: 
- The first byte is padding and can be ignored. 
- The next byte is a uint8_t, which represents the number of characters in the sensor’s name (in bytes). 
- The next two bytes are a uint16_t, which indicates an offset into the data body. 


#### The data format is encoded in 32-bit: 
##### Bits Description 
- **0-9**: Offset indicating the start of the floating-point values in the data body 
- **10-19**: The number of floating-point values 
- **20**: An even parity bit
- **21-31**: Padding 


### Parity Check:
#### The parity bit corresponds to an even parity check of the bytes that represent the floating-point data
The number of set bits (1s) in the bytes that represent the floating-point data counted:
- If the count is an odd number, the even parity bit is set 
- If the count is an even number, the even parity bit is clear 

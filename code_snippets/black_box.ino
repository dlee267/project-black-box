// importing libraries here

#define SINGLE_PACKET_MIN ...
#define SINGLE_PACKET_MAX ...

// ...
byte * b;
uint16_t temp;
const char START_FLAG = ...;
const char AUDIO_FLAG = ...;
bytes txMicBuffer[...];
int checksum;

int audio_buffer_getSize(void) {
  return ...
}
void sendAudioSerial(void) {
  int count = 0;
  // misleading var name, stores the size of the audio, the total sound bytes would be size * 2
  int storedSoundBytes = audio_buffer_getSize();

  // don't read out more than the max of our ring buffer
  // remember, we're also recording while we're sending
  while (count < storedSoundBytes) {
    if (audio_buffer_getSize() < SINGLE_PACKET_MIN) {
      break;
    }
    // read out max packet size at a time
    // for loop should be faster, since we can check our buffer size just once?
    int size = _min(audio_buffer_getSize(), SINGLE_PACKET_MAX);
    
    // PACKET HEADER
    // the header of the packet starts with a starting flag
    Serial.write(START_FLAG);
    Serial.write(START_FLAG);
    Serial.write(START_FLAG);
    // packet ID = AUDIO_FLAG, used to distringuish other packets
    Serial.write(AUDIO_FLAG);
    // checksum: optional for data loss correction
    checksum = 0;
    b = (byte *) &size;

    // the size is being sent all at once as an array of bytes (i32)
    Serial.write(b,4);

    for (int c = 0; c < size; c++) {
      temp = audio_buffer_get();

      // the array stores the 2 byte data
      txMicBuffer[2 * c] = (uint8_t)(temp & 0x00FF);
      txMicBuffer[2 * c + 1] = (uint8_t)((temp >> 8) & 0x00FF);
      checksum += temp;
    }
    count += size;
    // the serial writes the entire buffer, total size = (sample count * sample size)
    Serial.write(txMicBuffer, 2 * size);
    // checksum sent at the end
    b = (byte *) &checksum;
    Serial.write(b, 4);

  }
}

// ...
// NOTE: similar implementation for sendGeophone()

void setup(void) {
  ...
  Serial.begin(...); // my baud rate was set 921600 for testing
  // comment all wifi and udp setup
}

void loop(void) {
  sendAudioSerial();
}

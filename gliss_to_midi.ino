#include <MIDIUSB.h>

#define NUM_INPUTS 8
#define SMOOTHING_WINDOW 8  // Number of samples to average

const int inputPins[NUM_INPUTS] = {A0, A1, A2, A3, A4, A5, A6, A7};

// Ring buffers for smoothing
int smoothingBuffers[NUM_INPUTS][SMOOTHING_WINDOW] = {0};
int bufferIndices[NUM_INPUTS] = {0};
long bufferSums[NUM_INPUTS] = {0};

int prevCCValues[NUM_INPUTS] = { -1, -1, -1, -1, -1, -1, -1, -1 };

void setup() {
    // Nothing needed here for setup
}

void loop() {
    for (int i = 0; i < NUM_INPUTS; i++) {
        int raw = analogRead(inputPins[i]);

        // Update smoothing buffer
        bufferSums[i] -= smoothingBuffers[i][bufferIndices[i]];  // remove oldest
        smoothingBuffers[i][bufferIndices[i]] = raw;              // insert newest
        bufferSums[i] += raw;                                     // add newest
        bufferIndices[i] = (bufferIndices[i] + 1) % SMOOTHING_WINDOW;

        // Compute smoothed value
        int average = bufferSums[i] / SMOOTHING_WINDOW;
        int ccValue = map(average, 0, 1023, 0, 127);

        if (ccValue != prevCCValues[i]) {
            sendCC(i, ccValue);  // Send CC 0–7
            prevCCValues[i] = ccValue;
        }
    }
}

void sendCC(byte ccNumber, byte ccValue) {
    midiEventPacket_t event = {0x0B, 0xB0, ccNumber, ccValue};  // Control Change, channel 1
    MidiUSB.sendMIDI(event);
    MidiUSB.flush();
}

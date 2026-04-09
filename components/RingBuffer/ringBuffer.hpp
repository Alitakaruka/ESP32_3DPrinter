#include "algorithm"
#include "cstring"
#include "stdio.h"

template <typename T>
class RingBuffer {
  private:
    T*  data;
    int maxElements;
    int ResieveIndex  = 0;
    int TransmitIndex = 0;

  public:
    RingBuffer(int MaxElements);
    ~RingBuffer();

    T& GetNowElement();

    bool isEmpty();
    void Next();
    void AddElement(const T element, int bytes);
};

template <typename T>
bool RingBuffer<T>::isEmpty() {
    return TransmitIndex == ResieveIndex;
}

template <typename T>
RingBuffer<T>::RingBuffer(int MaxElements) {
    this->maxElements = MaxElements;
    this->data        = new T[MaxElements];
}

template <typename T>
RingBuffer<T>::~RingBuffer() {
    delete[] this->data;
}

template <typename T>
T& RingBuffer<T>::GetNowElement() {
    return this->data[TransmitIndex];
}

template <typename T>
void RingBuffer<T>::Next() {
    this->TransmitIndex = (TransmitIndex + 1) % maxElements;
}

template <typename T>
void RingBuffer<T>::AddElement(const T element, int bytes) {
    memcpy(this->data[ResieveIndex], element, bytes);
    ResieveIndex = (ResieveIndex + 1) % maxElements;
}

template <size_t N>
class Buffio {
  private:
    unsigned char data[N];
    size_t        readPos  = 0;
    size_t        writePos = 0;

  public:
    Buffio() = default;

    bool isEmpty() const { return readPos == writePos; }
    bool isFull() const { return ((writePos + 1) % N) == readPos; }

    size_t available() const {
        if(writePos >= readPos)
            return writePos - readPos;
        else
            return N - (readPos - writePos);
    }

    size_t freeSpace() const { return N - available() - 1; }

    int WriteNext(const uint8_t* buffer, int len) {
        if(len > (int)freeSpace())
            return -1;

        for(int i = 0; i < len; i++) {
            data[writePos] = buffer[i];
            writePos       = (writePos + 1) % N;
        }
        return len;
    }

    int WriteNext(const char* buffer, int len) {
        return WriteNext(reinterpret_cast<const uint8_t*>(buffer), len);
    }

    int ReadBytes(uint8_t* buffer, int len) {
        int counter = 0;
        while(len) {
            buffer[counter] = data[readPos];
            readPos         = (readPos + 1) % N;
            counter++;
        }
        return len;
    }

    int ReadBytes(char* buffer, int len) {
        return ReadBytes(reinterpret_cast<uint8_t*>(buffer), len);
    }

    int ReadLine(uint8_t* buffer, int maxLen, uint8_t delim) {
        int count = 0;
        while(!isEmpty() && maxLen > 0) {
            uint8_t c = data[readPos];
            readPos   = (readPos + 1) % N;
            if(c == delim)
                break;
            buffer[count++] = c;
            maxLen--;
        }
        return count;
    }

    int ReadLine(char* buffer, int maxLen, uint8_t delim) {
        return ReadLine(reinterpret_cast<uint8_t*>(buffer), maxLen, delim);
    }

    int ReadLine(char* buffer, int maxLen, const char* delim) {
        int    count    = 0;
        size_t delimLen = strlen(delim);
        while(maxLen) {
            if(isEmpty()) {
                continue;
            }
            buffer[count] = data[readPos];
            readPos       = (readPos + 1) % N;
            count++;
            maxLen--;

            if(count >= (int)delimLen &&
               memcmp(&buffer[count - delimLen], delim, delimLen) == 0) {
                buffer[count - delimLen] = '\0';
                return count - delimLen;
            }
        }
        return count;
    }
};

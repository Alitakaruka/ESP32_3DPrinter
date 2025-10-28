#include "stdio.h"

template<typename T>
class RingBuffer
{
private:
    T* data;
    int maxElements;
    int ResieveIndex = 0;
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
bool RingBuffer<T>::isEmpty(){
    return TransmitIndex == ResieveIndex;
}

template<typename T>
RingBuffer<T>::RingBuffer(int MaxElements) {
    this->maxElements = MaxElements;
    this->data = new T[MaxElements];
}

template<typename T>
RingBuffer<T>::~RingBuffer(){
    delete[] this->data;
}

template <typename T>
T& RingBuffer<T>::GetNowElement(){
    return this->data[TransmitIndex];
}

template <typename T>
void RingBuffer<T>::Next(){
    this->TransmitIndex = (TransmitIndex + 1) % maxElements;
}

template<typename T>
void RingBuffer<T>::AddElement(const T element,int bytes){
    memcpy(this->data[ResieveIndex],element,bytes);
    ResieveIndex = (ResieveIndex + 1) % maxElements;
}



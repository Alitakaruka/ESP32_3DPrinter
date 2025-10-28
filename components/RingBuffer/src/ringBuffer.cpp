#include "ringBuffer.hpp"

// template<typename T>
// RingBuffer<T>::RingBuffer(int MaxElements){
//     this->data = new T[MaxElements];
// }

// template<typename T>
// RingBuffer<T>::~RingBuffer(){
//     delete[] this->data;
// }

// template <typename T>
// T& RingBuffer<T>::NowElement(){
//     return this->data[TransmitIndex];
// }

// template <typename T>
// void RingBuffer<T>::Next(){
//     this->TransmitIndex = (TransmitIndex + 1) % maxElements;
// }

// template<typename T>
// void RingBuffer<T>::AddElement(const T element){
//     this->data[ResieveIndex] = element;
//     ResieveIndex = (ResieveIndex + 1) % maxElements;
// }

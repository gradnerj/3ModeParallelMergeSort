#include <iostream>
#include <random>
#include <string>
#include <chrono>

using std::cin;
using std::cout;
using std::endl;
using std::string;

class BaseSort {
public:
  BaseSort(const unsigned int capacity);
  string getName();
  void loadRandomValues();
  void print() const;
  virtual void sort() = 0; // Pure virtual function (also makes the class abstract)
  bool isSorted() const;
protected:
  unsigned int* arr{ nullptr };
  unsigned int capacity{ 20 };
  string name;
private:
};


BaseSort::BaseSort(const unsigned int capacity) {
  arr = new unsigned int[capacity];
  this->capacity = capacity;
}

string BaseSort::getName() {
  return name;
}

void BaseSort::loadRandomValues() {
  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> distrib(0, 100);
  for (unsigned int i = 0; i < capacity; ++i) {
    arr[i] = distrib(gen);
  }
}

void BaseSort::sort() {
  cout << "No dummy, this shouldn't be called" << endl;
}

bool BaseSort::isSorted() const {
  bool sorted{ true };
  unsigned int index = 0;
  while (sorted && index < capacity - 1) {
    if ((arr[index] > arr[index + 1])) {
      sorted = false;
    }
    index++;
  }
  return sorted;
}
void BaseSort::print() const {
  if (capacity < 200) {
    for (unsigned int i = 0; i < capacity; ++i) {
      cout << arr[i] << " ";
    }
    cout << endl;
  }
}


class Merge : public BaseSort {
public:
  Merge(const unsigned int capacity) : BaseSort(capacity) {
    this->name = "Merge";
  }
  void sort();
private:
  void sort(unsigned int firstIndex, unsigned int lastIndex);
};

void Merge::sort() {
  sort(0, this->capacity);
}

void Merge::sort(unsigned int firstIndex, unsigned int lastIndex) {
  if (firstIndex < lastIndex - 1) {
    // Find a middle
    unsigned int middleIndex = (lastIndex - firstIndex) / 2 + firstIndex;

    sort(firstIndex, middleIndex);
    sort(middleIndex, lastIndex);

    unsigned int leftSize = middleIndex - firstIndex;
    unsigned int* leftArray = new unsigned int[leftSize];
    for (unsigned int i = 0; i < leftSize; i++) {
      leftArray[i] = this->arr[firstIndex + i];
    }
    unsigned int rightSize = lastIndex - middleIndex;
    unsigned int* rightArray = new unsigned int[rightSize];
    for (unsigned int i = 0; i < rightSize; i++) {
      rightArray[i] = this->arr[middleIndex + i];
    }
    unsigned int i = 0;
    unsigned int j = 0;
    unsigned int k = firstIndex;

    while (i < leftSize && j < rightSize) {
      if (leftArray[i] <= rightArray[j]) {
        this->arr[k] = leftArray[i];
        i++;
      }
      else {
        this->arr[k] = rightArray[j];
        j++;
      }
      k++;
    }
    while (i < leftSize) {
      this->arr[k] = leftArray[i];
      i++; 
      k++;
    }
    while (j < rightSize) {
      this->arr[k] = rightArray[j];
      j++; 
      k++;
    }
    delete[] leftArray;
    delete[] rightArray;
  }
}


void runMySort(BaseSort&& obj) {
  cout << "Processing the " << obj.getName() << " sort" << endl;
  obj.loadRandomValues();
  //obj.print();
  auto t1 = std::chrono::high_resolution_clock::now();
  obj.sort();
  auto t2 = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double, std::milli> fp_ms = t2 - t1;
  //obj.print();
  if (obj.isSorted()) {
    cout << "Sorted!" << endl;
  }
  else {
    cout << "Not sorted" << endl;
  }
  cout << "Time elapsed " << fp_ms.count() << " milliseconds" << endl;
}

int main() {

  runMySort(Merge(33'554'432));
 // runMySort(Quick(100000));
 // runMySort(Selection(100000));
 // runMySort(Insertion(100000));
 // runMySort(Bubble(100000));

  cin.get();
  return 0;

}




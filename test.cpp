
#include <iostream>
#include <vector>
#include "src/priority_queue.hpp"

int main() {
    // Test basic operations
    sjtu::priority_queue<int> pq;
    
    // Test empty
    std::cout << "Empty: " << (pq.empty() ? "true" : "false") << std::endl;
    std::cout << "Size: " << pq.size() << std::endl;
    
    // Test push
    pq.push(3);
    pq.push(1);
    pq.push(4);
    pq.push(1);
    pq.push(5);
    
    std::cout << "Size after pushes: " << pq.size() << std::endl;
    std::cout << "Top: " << pq.top() << std::endl;
    
    // Test pop
    pq.pop();
    std::cout << "Top after pop: " << pq.top() << std::endl;
    std::cout << "Size after pop: " << pq.size() << std::endl;
    
    // Test another queue
    sjtu::priority_queue<int> pq2;
    pq2.push(2);
    pq2.push(7);
    pq2.push(1);
    
    std::cout << "pq2 top: " << pq2.top() << std::endl;
    std::cout << "pq2 size: " << pq2.size() << std::endl;
    
    // Test merge
    pq.merge(pq2);
    std::cout << "pq top after merge: " << pq.top() << std::endl;
    std::cout << "pq size after merge: " << pq.size() << std::endl;
    std::cout << "pq2 size after merge: " << pq2.size() << std::endl;
    
    // Test copy constructor
    sjtu::priority_queue<int> pq3(pq);
    std::cout << "pq3 top: " << pq3.top() << std::endl;
    std::cout << "pq3 size: " << pq3.size() << std::endl;
    
    // Test assignment operator
    sjtu::priority_queue<int> pq4;
    pq4 = pq3;
    std::cout << "pq4 top: " << pq4.top() << std::endl;
    std::cout << "pq4 size: " << pq4.size() << std::endl;
    
    // Test popping all elements
    while (!pq.empty()) {
        std::cout << pq.top() << " ";
        pq.pop();
    }
    std::cout << std::endl;
    
    return 0;
}

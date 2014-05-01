/**
 * shared_ptr.cc
 *  Copyright Gaurav Rastogi, 2013
 *  Created on: Apr 30, 2014
 *      Author: grastogi
 */

#include<iostream>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace std;

class Base : public boost::enable_shared_from_this<Base> {
public:
    virtual void run() {
        std::cout << "I am Base" << std::endl;
        return;
    }
    virtual ~Base() {
        return;
    }
};

class Derived : public Base {
public:
    virtual void run() {
        std::cout << "I am derived " << std::endl;
    }
    virtual ~Derived() {
        return;
    }
};

typedef boost::shared_ptr<Base> BaseSharedPtr;
typedef boost::shared_ptr<Derived> DerivedSharedPtr;


int main( int argc, const char* argv[]) {
    DerivedSharedPtr d(new Derived());
    BaseSharedPtr b = d;
    b->run();
    return 0;
}

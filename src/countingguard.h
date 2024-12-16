#ifndef COUNTINGGUARD_H
#define COUNTINGGUARD_H

class CountingGuard {
private:
    unsigned& counter;

public:
    CountingGuard(unsigned& counter) : counter(counter) {
        counter++;
    }
    virtual ~CountingGuard() {
        if (counter > 0)
            counter--;
    }
};

#endif // COUNTINGGUARD_H

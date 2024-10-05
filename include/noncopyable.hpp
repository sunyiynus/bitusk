#ifndef NONCOPYABLE_HPP
#define NONCOPYABLE_HPP


class noncopyable {
protected:
    noncopyable() {}
    ~noncopyable() {}
private:
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
};

#endif // NONCOPYABLE_HPP
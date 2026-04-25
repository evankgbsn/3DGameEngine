#ifndef LISTENER_H
#define LISTENER_H

class Listener
{
public:

	Listener();

	~Listener();

private:

	Listener(const Listener&) = delete;

	Listener& operator=(const Listener&) = delete;

	Listener(Listener&&) = delete;

	Listener& operator=(Listener&&) = delete;

};

#endif // LISTENER_H
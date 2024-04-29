#ifndef COLLIDER_H
#define COLLIDER_H

class Collider
{
public:
	
	bool Visible() const;

protected:

	Collider();

	~Collider();

	virtual void Update() = 0;

	virtual void ToggleVisibility() = 0;

	bool visible;

private:

	Collider(const Collider&) = delete;

	Collider& operator=(const Collider&) = delete;
};

#endif // COLLIDER_H
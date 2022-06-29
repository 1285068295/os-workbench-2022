static void my_static();
struct stru{
	void (*f)();
};
void use_my_static(struct stru *h);